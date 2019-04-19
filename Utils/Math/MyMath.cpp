#include"MyMath.h"

#include<math.h>

Vector3d Vector3d::Cross(const Vector3d& v) const
{
	const double _x = y * v.z - z * v.y;
	const double _y = z * v.x - x * v.z;
	const double _z = x * v.y - y * v.x;

	return Vector3d(_x, _y, _z);
}

Vector3d& Vector3d::Normalize()
{
	const double Length = sqrt(x * x + y * y + z * z);

	x /= Length;
	y /= Length;
	z /= Length;

	return *this;
}