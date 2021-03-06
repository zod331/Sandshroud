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

#include "platform.h"
#include "CoordinateFrame.h"
#include "Quat.h"
#include "Matrix4.h"
#include "Box.h"
#include "AABox.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Ray.h"
#include "Capsule.h"
#include "Cylinder.h"
#include "UprightFrame.h"
#include "stringutils.h"

namespace G3D {

CoordinateFrame::CoordinateFrame(const class UprightFrame& f) {
	*this = f.toCoordinateFrame();
}


CoordinateFrame::CoordinateFrame() : 
	rotation(Matrix3::identity()), translation(Vector3::zero()) {
}

CoordinateFrame CoordinateFrame::fromXYZYPRRadians(float x, float y, float z, float yaw, 
												   float pitch, float roll) {
	Matrix3 rotation = Matrix3::fromAxisAngle(Vector3::unitY(), yaw);
	
	rotation = Matrix3::fromAxisAngle(rotation.column(0), pitch) * rotation;
	rotation = Matrix3::fromAxisAngle(rotation.column(2), roll) * rotation;

	const Vector3 translation(x, y, z);
	
	return CoordinateFrame(rotation, translation);
}


void CoordinateFrame::getXYZYPRRadians(float& x, float& y, float& z, 
									   float& yaw, float& pitch, float& roll) const {
	x = translation.x;
	y = translation.y;
	z = translation.z;
	
	const Vector3& look = lookVector();

	if (abs(look.y) > 0.99f) {
		// Looking nearly straight up or down

		yaw   = G3D::pi() + atan2(look.x, look.z);
		pitch = asin(look.y);
		roll  = 0.0f;
		
	} else {

		// Yaw cannot be affected by others, so pull it first
		yaw = G3D::pi() + atan2(look.x, look.z);
		
		// Pitch is the elevation of the yaw vector
		pitch = asin(look.y);
		
		Vector3 actualRight = rightVector();
		Vector3 expectedRight = look.cross(Vector3::unitY());

		roll = 0;//acos(actualRight.dot(expectedRight));  TODO
	}
}


void CoordinateFrame::getXYZYPRDegrees(float& x, float& y, float& z, 
									   float& yaw, float& pitch, float& roll) const {
	getXYZYPRRadians(x, y, z, yaw, pitch, roll);
	yaw   = toDegrees(yaw);
	pitch = toDegrees(pitch);
	roll  = toDegrees(roll);
}
	

CoordinateFrame CoordinateFrame::fromXYZYPRDegrees(float x, float y, float z, 
												   float yaw, float pitch, float roll) {
	return fromXYZYPRRadians(x, y, z, toRadians(yaw), toRadians(pitch), toRadians(roll));
}


Ray CoordinateFrame::lookRay() const {
	return Ray::fromOriginAndDirection(translation, lookVector());
}


bool CoordinateFrame::fuzzyEq(const CoordinateFrame& other) const {

	for (int c = 0; c < 3; ++c) {
		for (int r = 0; r < 3; ++r) {
			if (! G3D::fuzzyEq(other.rotation[r][c], rotation[r][c])) {
				return false;
			}
		}
		if (! G3D::fuzzyEq(translation[c], other.translation[c])) {
			return false;
		}
	}

	return true;
}


bool CoordinateFrame::fuzzyIsIdentity() const {
	const Matrix3& I = Matrix3::identity();

	for (int c = 0; c < 3; ++c) {
		for (int r = 0; r < 3; ++r) {
			if (fuzzyNe(I[r][c], rotation[r][c])) {
				return false;
			}
		}
		if (fuzzyNe(translation[c], 0)) {
			return false;
		}
	}

	return true;
}


bool CoordinateFrame::isIdentity() const {
	return 
		(translation == Vector3::zero()) &&
		(rotation == Matrix3::identity());
}


Matrix4 CoordinateFrame::toMatrix4() const {
	return Matrix4(*this);
}


std::string CoordinateFrame::toXML() const {
	return format(
		"<COORDINATEFRAME>\n  %lf,%lf,%lf,%lf,\n  %lf,%lf,%lf,%lf,\n  %lf,%lf,%lf,%lf,\n  %lf,%lf,%lf,%lf\n</COORDINATEFRAME>\n",
		rotation[0][0], rotation[0][1], rotation[0][2], translation.x,
		rotation[1][0], rotation[1][1], rotation[1][2], translation.y,
		rotation[2][0], rotation[2][1], rotation[2][2], translation.z,
		0.0, 0.0, 0.0, 1.0);
}


Plane CoordinateFrame::toObjectSpace(const Plane& p) const {
	Vector3 N, P;
	double d;
	p.getEquation(N, d);
	P = N * (float)d;
	P = pointToObjectSpace(P);
	N = normalToObjectSpace(N);
	return Plane(N, P);
}


Plane CoordinateFrame::toWorldSpace(const Plane& p) const {
	Vector3 N, P;
	double d;
	p.getEquation(N, d);
	P = N * (float)d;
	P = pointToWorldSpace(P);
	N = normalToWorldSpace(N);
	return Plane(N, P);
}


Triangle CoordinateFrame::toObjectSpace(const Triangle& t) const {
	return Triangle(pointToObjectSpace(t.vertex(0)),
		pointToObjectSpace(t.vertex(1)),
		pointToObjectSpace(t.vertex(2)));
}


Triangle CoordinateFrame::toWorldSpace(const Triangle& t) const {
	return Triangle(pointToWorldSpace(t.vertex(0)),
		pointToWorldSpace(t.vertex(1)),
		pointToWorldSpace(t.vertex(2)));
}


Cylinder CoordinateFrame::toWorldSpace(const Cylinder& c) const {
	return Cylinder(
		pointToWorldSpace(c.point(0)), 
		pointToWorldSpace(c.point(1)), 
		c.radius());
}


Capsule CoordinateFrame::toWorldSpace(const Capsule& c) const {
	return Capsule(
		pointToWorldSpace(c.point(0)), 
		pointToWorldSpace(c.point(1)), 
		c.radius());
}


Box CoordinateFrame::toWorldSpace(const AABox& b) const {
	Box b2(b);
	return toWorldSpace(b2);
}


Box CoordinateFrame::toWorldSpace(const Box& b) const {
	Box out(b);

	for (int i = 0; i < 8; ++i) {
		out._corner[i] = pointToWorldSpace(b._corner[i]);
		ASSERT(! isNaN(out._corner[i].x));
	}

	for (int i = 0; i < 3; ++i) {
		out._axis[i] = vectorToWorldSpace(b._axis[i]);
	}

	out._center = pointToWorldSpace(b._center);

	return out;
}


Box CoordinateFrame::toObjectSpace(const Box &b) const {
	return inverse().toWorldSpace(b);
}


Box CoordinateFrame::toObjectSpace(const AABox& b) const {
	return toObjectSpace(Box(b));
}

Sphere CoordinateFrame::toWorldSpace(const Sphere &b) const {
	return Sphere(pointToWorldSpace(b.center), b.radius);
}


Sphere CoordinateFrame::toObjectSpace(const Sphere &b) const {
	return Sphere(pointToObjectSpace(b.center), b.radius);
}


Ray CoordinateFrame::toWorldSpace(const Ray& r) const {
	return Ray::fromOriginAndDirection(pointToWorldSpace(r.origin()), vectorToWorldSpace(r.direction()));
}


Ray CoordinateFrame::toObjectSpace(const Ray& r) const {
	return Ray::fromOriginAndDirection(pointToObjectSpace(r.origin()), vectorToObjectSpace(r.direction()));
}


void CoordinateFrame::lookAt(const Vector3 &target) {
	lookAt(target, Vector3::unitY());
}


void CoordinateFrame::lookAt(
	const Vector3&  target,
	Vector3		 up) {

	up = up.direction();

	Vector3 look = (target - translation).direction();
	if (fabs(look.dot(up)) > .99f) {
		up = Vector3::unitX();
		if (fabs(look.dot(up)) > .99f) {
			up = Vector3::unitY();
		}
	}

	up -= look * look.dot(up);
	up.unitize();

	Vector3 z = -look;
	Vector3 x = -z.cross(up);
	x.unitize();

	Vector3 y = z.cross(x);

	rotation.setColumn(0, x);
	rotation.setColumn(1, y);
	rotation.setColumn(2, z);
}


CoordinateFrame CoordinateFrame::lerp(
	const CoordinateFrame&  other,
	float				   alpha) const {

	if (alpha == 1.0f) {
		return other;
	} else if (alpha == 0.0f) {
		return *this;
	} else {
		Quat q1 = Quat(this->rotation);
		Quat q2 = Quat(other.rotation);

		return CoordinateFrame(
			q1.slerp(q2, alpha).toRotationMatrix(),
			this->translation * (1 - alpha) + other.translation * alpha);
	}
} 


void CoordinateFrame::pointToWorldSpace(const Array<Vector3>& v, Array<Vector3>& vout) const {
	vout.resize(v.size());

	for (int i = v.size() - 1; i >= 0; --i) {
		vout[i] = pointToWorldSpace(v[i]);
	}
}


void CoordinateFrame::normalToWorldSpace(const Array<Vector3>& v, Array<Vector3>& vout) const  {
	vout.resize(v.size());

	for (int i = v.size() - 1; i >= 0; --i) {
		vout[i] = normalToWorldSpace(v[i]);
	}
}


void CoordinateFrame::vectorToWorldSpace(const Array<Vector3>& v, Array<Vector3>& vout) const {
	vout.resize(v.size());

	for (int i = v.size() - 1; i >= 0; --i) {
		vout[i] = vectorToWorldSpace(v[i]);
	}
}


void CoordinateFrame::pointToObjectSpace(const Array<Vector3>& v, Array<Vector3>& vout) const {
	vout.resize(v.size());

	for (int i = v.size() - 1; i >= 0; --i) {
		vout[i] = pointToObjectSpace(v[i]);
	}
}


void CoordinateFrame::normalToObjectSpace(const Array<Vector3>& v, Array<Vector3>& vout) const {
	vout.resize(v.size());

	for (int i = v.size() - 1; i >= 0; --i) {
		vout[i] = normalToObjectSpace(v[i]);
	}
}


void CoordinateFrame::vectorToObjectSpace(const Array<Vector3>& v, Array<Vector3>& vout) const {
	vout.resize(v.size());

	for (int i = v.size() - 1; i >= 0; --i) {
		vout[i] = vectorToObjectSpace(v[i]);
	}
}

} // namespace
