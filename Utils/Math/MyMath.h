#ifndef MY_MATH_H_
#define MY_MATH_H_

#include<iostream>

struct Vector3f
{
	float x;
	float y;
	float z;

	Vector3f() { x = y = z = 0; }

	Vector3f(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f(const double* pdouble)
	{
		x = pdouble[0];
		y = pdouble[0];
		z = pdouble[0];
	}

	Vector3f(const float* pfloat)
	{
		x = pfloat[0];
		y = pfloat[0];
		z = pfloat[0];
	}

	Vector3f(float f)
	{
		x = y = z = f;
	}



	Vector3f& operator+=(const Vector3f& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;

		return *this;
	}

	Vector3f& operator-=(const Vector3f& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;

		return *this;
	}

	Vector3f& operator*=(double f)
	{
		x *= f;
		y *= f;
		z *= f;

		return *this;
	}

	Vector3f& operator=(const Vector3f& r)
	{
		x = r.x;
		y = r.y;
		z = r.z;

		return *this;
	}


	Vector3f operator+( const Vector3f &rhs)
	{
		Vector3f sum;
		sum.x = this->x + rhs.x;
		sum.y = this->y + rhs.y;
		sum.z = this->z + rhs.z;
		return sum;
	}

	Vector3f operator-(const Vector3f& rhs)
	{
		Vector3f sub;
		sub.x = this->x - rhs.x;
		sub.y = this->y - rhs.y;
		sub.z = this->z - rhs.z;
		return sub;
	}

	Vector3f operator*(double f)
	{
		Vector3f rlt;
		rlt.x = x*f;
		rlt.y = y*f;
		rlt.z = z*f;

		return rlt;
	}

	Vector3f Cross(const Vector3f& v) const;

	Vector3f& Normalize();

	void Print() const
	{
		printf("(%.02f, %.02f, %.02f)", x, y, z);
	}
};


class MyMatrix4f
{
public:
	float m[4][4];

	MyMatrix4f()
	{
		InitIdentity();
	}

	// constructor from Assimp matrix
	MyMatrix4f(double a00, double a01, double a02, double a03,
		double a10, double a11, double a12, double a13,
		double a20, double a21, double a22, double a23,
		double a30, double a31, double a32, double a33)
	{
		m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
		m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
		m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
		m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
	}

	void SetZero()
	{
		memset(m, 0, sizeof(m));
	}

	MyMatrix4f Transpose() const
	{
		MyMatrix4f n;

		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				n.m[i][j] = m[j][i];
			}
		}

		return n;
	}


	inline void InitIdentity()
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	inline MyMatrix4f operator*(const MyMatrix4f& Right) const
	{
		MyMatrix4f Ret;

		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				Ret.m[i][j] = m[i][0] * Right.m[0][j] +
					m[i][1] * Right.m[1][j] +
					m[i][2] * Right.m[2][j] +
					m[i][3] * Right.m[3][j];
			}
		}

		return Ret;
	}

	operator const float*() const
	{
		return &(m[0][0]);
	}

	inline MyMatrix4f operator*(const MyMatrix4f& Right)
	{
		memcpy(m, Right.m, sizeof(float) * 16);

		return *this;
	}

	void Print() const
	{
		for (int i = 0; i < 4; i++) {
			printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
		}
	}
};


#endif // !MY_MATH_H_
