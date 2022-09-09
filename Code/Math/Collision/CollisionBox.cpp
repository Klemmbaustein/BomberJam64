#include "CollisionBox.h"
#include <iostream>

Collision::Box Collision::operator*(Box a, Vector3 b)
{
    return Box(a.minX * b.X, a.maxX * b.X, a.minY * b.Y, a.maxY * b.Y, a.minZ * b.Z, a.maxZ * b.Z);
}

Collision::Box Collision::operator+(Box a, Vector3 b)
{
    return Box(a.minX + b.X, a.maxX + b.X, a.minY + b.Y, a.maxY + b.Y, a.minZ + b.Z, a.maxZ + b.Z);
}

Vector3 Collision::Box::GetCenter()
{
	return Vector3(Vector3::Lerp(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ), 0.5));
}

Collision::Box Collision::Box::TransformBy(Transform Transform)
{
	Collision::Box OldBox = Box(minX, maxX, minY, maxY, minZ, maxZ) * Transform.Scale;
	Collision::Box RotatedBox = OldBox;
	float rot = std::abs(Vector3::Dot(Vector3::GetForwardVector(Transform.Rotation.RadiantsToDegrees()), Vector3(0, 0, 1)));
	RotatedBox.maxX = std::lerp(OldBox.maxX, OldBox.maxZ, rot);
	RotatedBox.minX = std::lerp(OldBox.minX, OldBox.minZ, rot);
	RotatedBox.maxZ = std::lerp(OldBox.maxZ, OldBox.maxX, rot);
	RotatedBox.minZ = std::lerp(OldBox.minZ, OldBox.minX, rot);
	RotatedBox = RotatedBox + Transform.Location;
	return RotatedBox;
}
