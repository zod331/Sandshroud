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

#ifndef VECTOR3INT16_H
#define VECTOR3INT16_H

#include "platform.h"
#include "g3dmath.h"
#include "HashTrait.h"

#ifdef _MSC_VER
// Turn off "conditional expression is constant" warning; MSVC generates this
// for debug assertions in inlined methods.
#pragma warning (disable : 4127)
#endif


namespace G3D {

/**
 \class Vector3int16
 A Vector3 that packs its fields into uint16s.
 */
G3D_BEGIN_PACKED_CLASS(2)
class Vector3int16 {
private:
	// Hidden operators
	bool operator<(const Vector3int16&) const;
	bool operator>(const Vector3int16&) const;
	bool operator<=(const Vector3int16&) const;
	bool operator>=(const Vector3int16&) const;

public:
	int16			  x;
	int16			  y;
	int16			  z;

	Vector3int16() : x(0), y(0), z(0) {}
	Vector3int16(int16 _x, int16 _y, int16 _z) : x(_x), y(_y), z(_z) {}
	Vector3int16(const class Vector3& v);

	inline int16& operator[] (int i) {
		ASSERT(i <= 2);
		return ((int16*)this)[i];
	}

	inline const int16& operator[] (int i) const {
		ASSERT(i <= 2);
		return ((int16*)this)[i];
	}

	inline Vector3int16 operator+(const Vector3int16& other) const {
		return Vector3int16(x + other.x, y + other.y, z + other.z);
	}

	inline Vector3int16 operator-(const Vector3int16& other) const {
		return Vector3int16(x - other.x, y - other.y, z - other.z);
	}

	inline Vector3int16 operator*(const Vector3int16& other) const {
		return Vector3int16(x * other.x, y * other.y, z * other.z);
	}

	inline Vector3int16 operator*(const int s) const {
		return Vector3int16(int16(x * s), int16(y * s), int16(z * s));
	}

	inline Vector3int16& operator+=(const Vector3int16& other) {
		x += other.x;
		y += other.y;
		z += other.y;
		return *this;
	}

	inline Vector3int16& operator-=(const Vector3int16& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	inline Vector3int16& operator*=(const Vector3int16& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	inline bool operator== (const Vector3int16& rkVector) const {
		return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
	}

	inline bool operator!= (const Vector3int16& rkVector) const {
		return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
	}

	Vector3int16 max(const Vector3int16& v) const {
		return Vector3int16(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z));
	}

	Vector3int16 min(const Vector3int16& v) const {
		return Vector3int16(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z));
	}

	std::string toString() const;
}
G3D_END_PACKED_CLASS(2)

}

template <> struct HashTrait<G3D::Vector3int16> {
	static size_t hashCode(const G3D::Vector3int16& key) { return static_cast<size_t>(key.x + ((int)key.y << 5) + ((int)key.z << 10)); }
};

#endif
