#pragma once
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>
#include <vector>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t>
struct Vec2
{
	union
	{
		struct
		{
			t u, v;
		};
		struct
		{
			t x, y;
		};
		t raw[2];
	};
	Vec2() : u(0), v(0), x(0), y(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	inline Vec2<t> operator+(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator-(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline void operator=(const Vec2<t> &V)
	{
		u = V.u;
		v = V.v;
	}
	inline Vec2<t> operator*(const float f) const { return Vec2<t>(u * f, v * f); }

	inline t &operator[](const int idx)
	{
		if (idx == 1)
			return y;
		else
			return x;
	}

	template <class>
	friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v);
};

template <class t>
struct Vec3
{
	union
	{
		struct
		{
			t x, y, z;
		};
		struct
		{
			t ivert, iuv, inorm;
		};
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	inline Vec3<t> operator^(const Vec3<t> &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }

	inline Vec3<t> operator/(float f) const { return Vec3<t>(x / f, y / f, z / f); }

	inline t &operator[](const int idx)
	{
		if (idx <= 0)
			return x;
		else if (idx == 1)
			return y;
		else
			return z;
	}
	inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<t> &normalize(t l = 1)
	{
		*this = (*this) * (l / norm());
		return *this;
	}
	template <class>
	friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
};

template <class t>
struct Vec4
{
	t x, y, z, w;

	Vec4() : x(0), y(0), z(0), w(1){};
	Vec4(t _x, t _y, t _z, t _w) : x(_x), y(_y), z(_z), w(_w){};
	inline Vec4<t> operator^(const Vec3<t> &v) const { return Vec4<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, 1.f); }
	inline Vec4<t> operator+(const Vec4<t> &v) const { return Vec4<t>(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline Vec4<t> operator-(const Vec4<t> &v) const { return Vec4<t>(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline Vec4<t> operator*(float f) const { return Vec4<t>(x * f, y * f, z * f, w * f); }
	inline Vec4<t> operator/(float f) const
	{
		if (f == 0.f)
		{
			std::cerr << "Error: Vec4 divide by zero\n";
			return *this;
		}
		return Vec4<t>(x / f, y / f, z / f, w / f);
	}

	inline t &operator[](const int idx)
	{
		if (idx < 0 || idx > 3)
		{
			std::cerr << "Error: Vec4 index out of range\n";
			return x;
		}
		if (idx == 0)
			return x;
		else if (idx == 1)
			return y;
		else if (idx == 2)
			return z;
		else
			return w;
	}
	inline t operator*(const Vec4<t> &v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
	Vec4 &norm()
	{
		if (w != 0)
		{
			x /= w;
			y /= w;
			z /= w;
			w = 1.f;
		}
		return *this;
	}
	template <class t>
	Vec3<t> toVec3()
	{
		if (w == 0 || w == 1)
		{
			return Vec3<t>(x, y, z);
		}
		else
		{
			return Vec3<t>(x / w, y / w, z / w);
		}
	}
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec4<float> Vec4f;
typedef Vec4<int> Vec4i;

template <class t>
std::ostream &operator<<(std::ostream &s, Vec2<t> &v)
{
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t>
std::ostream &operator<<(std::ostream &s, Vec3<t> &v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

template <class t>
std::ostream &operator<<(std::ostream &s, Vec4<t> &v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
	return s;
}

template <typename T>
inline Vec2<T> operator*(const float f, const Vec2<T> &vec)
{
	return vec * f;
}

template <typename T>
inline Vec3<T> operator*(const float f, const Vec3<T> &vec)
{
	return vec * f;
}

template <typename T>
inline Vec4<T> operator*(const float f, const Vec4<T> &vec)
{
	return vec * f;
}

const int DEFAULT_ALLOC = 4;

namespace MyMtrix
{
	class Matrix
	{
	protected:
		std::vector<std::vector<float>> m;
		int rows, cols;

	public:
		Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC) : rows(r), cols(c), m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))){};
		inline int nrows() const { return rows; };
		inline int ncols() const { return cols; };

		Vec3f to_vec3(); //
		Vec4f to_vec4();

		static Matrix identity(int dimensions);
		std::vector<float> &operator[](const int i);
		Matrix operator*(const Matrix &a);
		Matrix operator/(const float a);
		Matrix transpose();
		Matrix inverse();

		Matrix get_minor(const int row, const int col) const;
		float det() const;
		float cofactor(const int row, const int col) const
		{
			return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
		}

		Matrix adjugate() const;
		Matrix invert_transpose() const;
		Matrix invert() const;

		friend std::ostream &operator<<(std::ostream &s, Matrix &m)
		{
			for (const auto &row : m.m)
			{
				for (const auto &ele : row)
				{
					s << ele << " ";
				}
				s << std::endl;
			}
			return s;
		};
	};

	Matrix toHomoCoordinate(Vec3f v, float w);
	MyMtrix::Matrix toMatrix(Vec4f v);
	MyMtrix::Matrix toMatrix(Vec3f v);

}


// MyMtrix::Matrix toMatrix(Vec3f v)
// {
// 	MyMtrix::Matrix m(3, 1);
// 	m[0][0] = v.x;
// 	m[1][0] = v.y;
// 	m[2][0] = v.z;
// 	return m;
// }

#endif //__GEOMETRY_H__