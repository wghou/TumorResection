#include"MyMath.h"

#include<math.h>

Vector3f Vector3f::Cross(const Vector3f& v) const
{
	const double _x = y * v.z - z * v.y;
	const double _y = z * v.x - x * v.z;
	const double _z = x * v.y - y * v.x;

	return Vector3f(_x, _y, _z);
}

Vector3f& Vector3f::Normalize()
{
	const double Length = sqrt(x * x + y * y + z * z);

	x /= Length;
	y /= Length;
	z /= Length;

	return *this;
}