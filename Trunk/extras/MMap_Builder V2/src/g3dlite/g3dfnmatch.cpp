/*
 * Sandshroud Hearthstone
 * Copyright (C) 2010 - 2011 Sandshroud <http://www.sandshroud.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "g3dfnmatch.h"

#ifdef G3D_WIN32

#include <ctype.h>
#include <string.h>
#include <stdio.h>

namespace G3D {

#define EOS '\0'

static const char *rangematch(const char *, char, int);

int g3dfnmatch(const char *pattern, const char *string, int flags)
{
	const char *stringstart;
	char c, test;

	for (stringstart = string;;)
		switch (c = *pattern++) {
		case EOS:
			if ((flags & FNM_LEADING_DIR) && *string == '/')
				return (0);
			return (*string == EOS ? 0 : FNM_NOMATCH);
		case '?':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && (flags & FNM_PATHNAME))
				return (FNM_NOMATCH);
			if (*string == '.' && (flags & FNM_PERIOD) &&
				(string == stringstart ||
				 ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);
			++string;
			break;
		case '*':
			c = *pattern;
			/* Collapse multiple stars. */
			while (c == '*')
				c = *++pattern;

			if (*string == '.' && (flags & FNM_PERIOD) &&
				(string == stringstart ||
				 ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);

			/* Optimize for pattern with * at end or before /. */
			if (c == EOS)
				if (flags & FNM_PATHNAME)
					return ((flags & FNM_LEADING_DIR) ||
							strchr(string, '/') == NULL ?
							0 : FNM_NOMATCH);
				else
					return (0);
			else if (c == '/' && flags & FNM_PATHNAME) {
				if ((string = strchr(string, '/')) == NULL)
					return (FNM_NOMATCH);
				break;
			}

			/* General case, use recursion. */
			while ((test = *string) != EOS) {
				if (!rangematch(pattern, *string, flags & ~FNM_PERIOD))
					return (0);
				if (test == '/' && flags & FNM_PATHNAME)
					break;
				++string;
			}
			return (FNM_NOMATCH);
		case '[':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && flags & FNM_PATHNAME)
				return (FNM_NOMATCH);
			if ((pattern =
				 rangematch(pattern, *string, flags)) == NULL)
				return (FNM_NOMATCH);
			++string;
			break;
		case '\\':
			if (!(flags & FNM_NOESCAPE)) {
				if ((c = *pattern++) == EOS) {
					c = '\\';
					--pattern;
				}
			}
			/* FALLTHROUGH */
		default:
			if (c == *string)
				;
			else if ((flags & FNM_CASEFOLD) &&
					 (tolower((unsigned char)c) ==
					  tolower((unsigned char)*string)))
				;
			else if ((flags & FNM_PREFIX_DIRS) && *string == EOS &&
					 ((c == '/' && string != stringstart) ||
					  (string == stringstart+1 && *stringstart == '/')))
				return (0);
			else
				return (FNM_NOMATCH);
			string++;
			break;
		}
	/* NOTREACHED */
}

static const char *
rangematch(const char *pattern, char test, int flags)
{
	int negate, ok;
	char c, c2;

	/*
	 * A bracket expression starting with an unquoted circumflex
	 * character produces unspecified results (IEEE 1003.2-1992,
	 * 3.13.2).  This implementation treats it like '!', for
	 * consistency with the regular expression syntax.
	 * J.T. Conklin (conklin@ngai.kaleida.com)
	 */
	if ( (negate = (*pattern == '!' || *pattern == '^')) )
		++pattern;

	if (flags & FNM_CASEFOLD)
		test = tolower((unsigned char)test);

	for (ok = 0; (c = *pattern++) != ']';) {
		if (c == '\\' && !(flags & FNM_NOESCAPE))
			c = *pattern++;
		if (c == EOS)
			return (NULL);

		if (flags & FNM_CASEFOLD)
			c = tolower((unsigned char)c);

		if (*pattern == '-'
			&& (c2 = *(pattern+1)) != EOS && c2 != ']') {
			pattern += 2;
			if (c2 == '\\' && !(flags & FNM_NOESCAPE))
				c2 = *pattern++;
			if (c2 == EOS)
				return (NULL);

			if (flags & FNM_CASEFOLD)
				c2 = tolower((unsigned char)c2);

			if ((unsigned char)c <= (unsigned char)test &&
				(unsigned char)test <= (unsigned char)c2)
				ok = 1;
		} else if (c == test)
			ok = 1;
	}
	return (ok == negate ? NULL : pattern);
}

}

#else

namespace G3D {
int g3dfnmatch(const char * a, const char *b, int c) {
	return fnmatch(a, b, c);
}
}

#endif

