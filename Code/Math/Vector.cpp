#include "Vector.h"
#include <iostream>
#include <numeric>
#include <format>
#include <iomanip>
#include <sstream>
#include <Math/Math.h>
#include <glm/gtx/euler_angles.hpp>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
Vector3 operator+(Vector3 a, Vector3 b)
{
	return Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
}

Vector3 operator-(Vector3 a, Vector3 b)
{
	return Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
}

Vector3 operator*(Vector3 a, Vector3 b)
{
	return Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
}

Vector3 operator*(Vector3 a, float b)
{
	return Vector3(a.X * b, a.Y * b, a.Z * b);
}

Vector3 operator/(Vector3 a, Vector3 b)
{
	return Vector3(a.X / b.X, a.Y / b.Y, a.Z / b.Z);
}

Vector2 operator+(Vector2 a, Vector2 b)
{
	return Vector2(a.X + b.X, a.Y + b.Y);
}

Vector2 operator-(Vector2 a, Vector2 b)
{
	return Vector2(a.X - b.X, a.Y - b.Y);
}

Vector2 operator*(Vector2 a, float b)
{
	return Vector2(a.X * b, a.Y * b);
}

Vector2 operator/(Vector2 a, Vector2 b)
{
	return Vector2(a.X / b.X, a.Y / b.Y);
}

Vector2 operator/(Vector2 a, float b)
{
	return Vector2(a.X / b, a.Y / b);
}

bool operator==(Vector2 a, float b)
{
	return a.X == b && a.Y == b;
}

bool operator==(Vector2 a, Vector2 b)
{
	return (a.X == b.X && a.Y == b.Y);
}

bool operator==(Vector3 a, Vector3 b)
{
	return a.X == b.X && a.Y == b.Y && b.Z == a.Z;
}

bool operator==(Transform a, Transform b)
{
	return a.Location == b.Location && a.Rotation == b.Rotation && a.Scale == b.Scale;
}

Transform operator+(Transform a, Transform b)
{
	return Transform(a.Location + b.Location, a.Rotation + b.Rotation, a.Scale * b.Scale);
}


Vector3 Vector3::Normalize()
{
	return Vector3(X, Y, Z) / Vector3(sqrt(X * X + Y * Y + Z * Z));
}

float Vector3::Length()
{
	return sqrt(X * X + Y * Y + Z * Z);
}

Vector3 Vector3::RadiantsToDegrees()
{
	return Vector3(glm::degrees(X), glm::degrees(Y), glm::degrees(Z));
}

Vector3 Vector3::DegreesToRadiants()
{
	return Vector3(glm::radians(X), glm::radians(Y), glm::radians(Z));
}

std::string Vector3::ToString()
{
	return std::string(std::to_string(X) + " " + std::to_string(Y) + " " + std::to_string(Z));
}


Vector3::Vector3(glm::vec3 xyz)
{
	X = xyz.x;
	Y = xyz.y;
	Z = xyz.z;
}

	Vector3 Vector3::Vec3ToVector(glm::vec3 In)
	{
		return Vector3(In.x, In.y, In.z);
	}


	Vector3 Vector3::GetForwardVector(Vector3 In)
	{
		return Vector3::GetScaledAxis(In.DegreesToRadiants(), 2);
	}
	Vector3 Vector3::SnapToGrid(Vector3 In, float GridSize)
	{
		Vector3 Result = Vector3((float)(int)In.X / GridSize, (float)(int)In.Y / GridSize, (float)(int)In.Z / GridSize);
		Result = Result * GridSize;
		return Result;
	}
	Vector3 Vector3::Cross(Vector3 a, Vector3 b)
	{
		return Vector3::Vec3ToVector(glm::normalize(glm::cross((glm::vec3)a, (glm::vec3)b)));
	}
	float Vector3::Dot(Vector3 a, Vector3 b)
	{
		std::vector<float> A = {a.X, a.Y, a.Z};
		std::vector<float> B = {b.X, b.Y, b.Z};
		return std::inner_product(std::begin(A), std::end(A), std::begin(B), 0.f);
	}
	Vector3 Vector3::LookAtFunction(Vector3 Start, Vector3 End, bool Radiants)
	{
		Vector3 Dir = (End - Start).Normalize();
		//if (Radiants)
			return Vector3(sin(Dir.Y), atan2(Dir.X, Dir.Z) + M_PI, 0);
		//else
		//	return Vector3(sin(Dir.Y), atan2(Dir.X, Dir.Z) + M_PI, 0).RadiantsToDegrees();
	}

	Vector3 Vector3::QuatToEuler(glm::quat quat)
	{
		glm::vec3 angles = glm::eulerAngles(quat);
		return Vector3(angles);
	}

	float Vector3::Distance(Vector3 a, Vector3 b)
	{
		Vector3 dif = b - a;
		return dif.Length();
	}

	Vector3 Vector3::GetUpVector(Vector3 In)
	{
		return Vector3::GetScaledAxis(In.DegreesToRadiants(), 2);
	}
	typedef float Float;
	typedef Float Axis[3];
	typedef Axis Axes[3];
	static void copy(const Axes& from, Axes& to)
	{
		for (size_t i = 0; i != 3; ++i) {
			for (size_t j = 0; j != 3; ++j) {
				to[i][j] = from[i][j];
			}
		}
	}
	static void mul(Axes& mat, Axes& b)
	{
		Axes result;
		for (size_t i = 0; i != 3; ++i) {
			for (size_t j = 0; j != 3; ++j) {
				Float sum = 0;
				for (size_t k = 0; k != 3; ++k) {
					sum += mat[i][k] * b[k][j];
				}
				result[i][j] = sum;
			}
		}
		copy(result, mat);
	}
	static void showAxis(const char* desc, const Axis& axis, Float sign)
	{
		std::cout << "  " << desc << " = (";
		for (size_t i = 0; i != 3; ++i) {
			if (i != 0) {
				std::cout << ",";
			}
			std::cout << axis[i] * sign;
		}
		std::cout << ")\n";
	}

	static void showAxes(const char* desc, Axes& axes)
	{
		std::cout << desc << ":\n";
		showAxis("front", axes[2], 1);
		showAxis("right", axes[0], -1);
		showAxis("up", axes[1], 1);
	}

	Vector3 Vector3::GetScaledAxis(Vector3 Rot, unsigned int Dir)
	{
		float x = -Rot.X;
		float y = Rot.Y;
		float z = -Rot.Z;
		Axes matX = {
		  {1,     0,     0 },
		  {0, cos(x),sin(x)},
		  {0,-sin(x),cos(x)}
		};
		Axes matY = {
		  {cos(y),0,-sin(y)},
		  {     0,1,      0},
		  {sin(y),0, cos(y)}
		};
		Axes matZ = {
		  { cos(z),sin(z),0},
		  {-sin(z),cos(z),0},
		  {      0,     0,1}
		};
		Axes axes = {
		  {1,0,0},
		  {0,1,0},
		  {0,0,1}
		};
		mul(axes, matX);
		mul(axes, matY);
		mul(axes, matZ);


		return Vector3(axes[Dir][2], axes[Dir][1], axes[Dir][0]).Normalize();
	}

	

	Vector3 Vector3::stov(std::string In)
	{
		if (In.size() == 0)
		{
			return Vector3();
		}
		std::string myString = In;
		std::stringstream iss(myString);

		float number;
		std::vector<float> myNumbers;
		while (iss >> number)
			myNumbers.push_back(number);
		if(myNumbers.size() >= 3)
			return Vector3(myNumbers.at(0), myNumbers.at(1), myNumbers.at(2));
		return Vector3();
	}
	Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float val)
	{
		return Vector3(std::lerp(a.X, b.X, val), std::lerp(a.Y, b.Y, val), std::lerp(a.Z, b.Z, val));
	}

Vector2& Vector2::operator+=(Vector2 b)
{
	X = X + b.X;
	Y = Y + b.Y;
	return *this;
}

glm::mat4 Transform::ToMatrix()
{
	glm::mat4 MatModel = glm::mat4(1.f);

	MatModel = glm::translate(MatModel, (glm::vec3)Location);
	MatModel = glm::rotate(MatModel, Rotation.Y, glm::vec3(0, 1, 0));
	MatModel = glm::rotate(MatModel, Rotation.Z, glm::vec3(0, 0, 1));
	MatModel = glm::rotate(MatModel, Rotation.X, glm::vec3(1, 0, 0));

	MatModel = glm::scale(MatModel, (glm::vec3)Scale);
	return MatModel;
}
