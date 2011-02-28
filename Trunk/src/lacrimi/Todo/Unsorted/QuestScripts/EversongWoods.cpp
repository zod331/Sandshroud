/*
 * Sun++ Scripts for Sandshroud MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2007-2008 Moon++ Team <http://www.moonplusplus.info/>
 * Copyright (C) 2008-2009 Sun++ Team <http://www.sunscripting.com/>
 * Copyright (C) 2009-2011 Sandshroud <http://www.sandshroud.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptStdAfx.h"


bool PoweringOurDefenses(uint32 i, Spell *pSpell)
{
	if(!pSpell->u_caster->IsPlayer())
    return true;

	Player *plr = TO_PLAYER(pSpell->u_caster);
	if( !plr )
		return true;

	QuestLogEntry *qle = plr->GetQuestLogForEntry( 8490 );
	if( !qle )
		return true;

	// Angelis : Need to script the scourge attack

	if( qle && qle->GetMobCount(0) < qle->GetQuest()->required_mobcount[0] )
	{
		qle->SetMobCount(0, qle->GetMobCount(0)+1);
		qle->SendUpdateAddKill(0);
		qle->UpdatePlayerFields();
	}
	return true;
}

void SetupEversongWoods(ScriptMgr * mgr)
{
	mgr->register_dummy_spell(28247, &PoweringOurDefenses);
}