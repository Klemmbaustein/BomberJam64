#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/fwd.hpp>
struct Vector2
{
	float X = 0;
	float Y = 0;

	Vector2()
	{

	}

	Vector2(float xy)
	{
		X = xy;
		Y = xy;
	}

	Vector2(float x, float y)
	{
		X = x;
		Y = y;
	}
	Vector2& operator+=(Vector2 b);
};

struct Vector3
{
	float X = 0;
	float Y = 0;
	float Z = 0;

	Vector3(float x, float y, float z)
	{
		X = x;
		Y = y;
		Z = z;
	}
	Vector3() {}

	Vector3(float xyz)
	{
		X = xyz;
		Y = xyz;
		Z = xyz;
	}

	Vector3(Vector2 xy, float z)
	{
		X = xy.X;
		Y = xy.Y;
		Z = z;
	}
	Vector3(glm::vec3 xyz);

	operator glm::vec3()
	{
		return glm::vec3(X, Y, Z);
	}

	float operator[](int in)
	{
		return at(in);
	}

	Vector3 Normalize();
	float Length();

	Vector3& operator+=(Vector3 a)
	{
		X = a.X + X;
		Y = a.Y + Y;
		Z = a.Z + Z;
		return *this;
	}
	
	Vector3 RadiantsToDegrees();
	Vector3 DegreesToRadiants();
	std::string ToString();

	operator std::string() const
	{
		return std::to_string(X).append(" ").append(std::to_string(Y)).append(" ").append(std::to_string(Z));
	}
	float at(int Index)
	{
		switch (Index)
		{
		case 0:
			return X;
		case 1:
			return Y;
		case 2:
			return Z;
		default:
			return 0;
		}
	}
	static Vector3 Vec3ToVector(glm::vec3 In);
	static Vector3 GetForwardVector(Vector3 In);
	static Vector3 Cross(Vector3 a, Vector3 b);
	static Vector3 stov(std::string In);
	static Vector3 SnapToGrid(Vector3 In, float GridSize);
	static Vector3 Lerp(Vector3 a, Vector3 b, float val);
	static float Dot(Vector3 a, Vector3 b);
	static Vector3 LookAtFunction(Vector3 Start, Vector3 End, bool Radiants = false);
	static Vector3 QuatToEuler(glm::quat quat);
	static float Distance(Vector3 a, Vector3 b);
	static Vector3 GetUpVector(Vector3 In);
	static Vector3 GetScaledAxis(Vector3 Rot, unsigned int Dir);
};

Vector3 operator+(Vector3 a, Vector3 b);

Vector3 operator-(Vector3 a, Vector3 b);

Vector3 operator*(Vector3 a, Vector3 b);

Vector3 operator*(Vector3 a, float b);

Vector3 operator/(Vector3 a, Vector3 b);

Vector2 operator+(Vector2 a, Vector2 b);

Vector2 operator-(Vector2 a, Vector2 b);

Vector2 operator*(Vector2 a, float b);

Vector2 operator/(Vector2 a, Vector2 b);

Vector2 operator/(Vector2 a, float b);

bool operator==(Vector2 a, float b);

bool operator==(Vector2 a, Vector2 b);

bool operator==(Vector3 a, Vector3 b);


struct Transform
{
	Vector3 Location = Vector3(0.f);
	Vector3 Rotation = Vector3(0.f);
	Vector3 Scale = Vector3(1.f);

	Transform(Vector3 Loc, Vector3 Rot, Vector3 Scl)
	{
		Location = Loc;
		Rotation = Rot;
		Scale = Scl;
	}
	Transform()
	{

	}

	glm::mat4 ToMatrix();
};

bool operator==(Transform a, Transform b);

Transform operator+(Transform a, Transform b);

struct Vector4
{
	float X = 0;
	float Y = 0;
	float Z = 0;
	float W = 0;

	Vector4(float XYZW)
	{
		X = XYZW;
		Y = XYZW;
		Z = XYZW;
		W = XYZW;
	}

	Vector4(Vector2 XY, Vector2 ZW)
	{
		X = XY.X;
		Y = XY.Y;
		Z = ZW.X;
		W = ZW.Y;
	}

	Vector4()
	{

	}
	Vector4(float X, float Y, float Z, float W)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->W = W;
	}

	Vector4(Vector3 XYZ, float W)
	{
		X = XYZ.X;
		Y = XYZ.Y;
		Z = XYZ.Z;
		this->W = W;
	}
};
