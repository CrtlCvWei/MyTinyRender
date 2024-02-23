#include <iostream>
#include <vector>
#include <cassert>

#include "geometry.hpp"

MyMtrix::Matrix MyMtrix::Matrix::identity(int dimensions)
{
	Matrix E(dimensions, dimensions);
	for (int i = 0; i < dimensions; i++)
		E[i][i] = 1.0f;
	return E;
}

std::vector<float> &MyMtrix::Matrix::operator[](const int i)
{
	assert(i >= 0 && i < rows);
	return m[i];
}

MyMtrix::Matrix MyMtrix::Matrix::operator*(const MyMtrix::Matrix &a)
{
	assert(cols == a.rows);
	MyMtrix::Matrix result(rows, a.cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < a.cols; j++)
		{
			result[i][j] = 0.0f;
			for (int k = 0; k < cols; k++)
			{
				result[i][j] += m[i][k] * a.m[k][j];
			}
		}
	}
	return result;
}

MyMtrix::Matrix MyMtrix::Matrix::operator/(const float a)
{
	assert(a != 0);
	MyMtrix::Matrix result(rows, cols);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			result[i][j] = m[i][j] / a;
	return result;
}

MyMtrix::Matrix MyMtrix::Matrix::transpose()
{
	MyMtrix::Matrix result(cols, rows);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			result[i][j] = m[j][i];
		}
	return result;
}

MyMtrix::Matrix MyMtrix::Matrix::inverse()
{
	assert(rows == cols);
	MyMtrix::Matrix result(rows, cols * 2);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			result[i][j] = m[i][j];
	for (int i = 0; i < rows; i++)
		result[i][i + cols] = 1;
	for (int i = 0; i < rows - 1; i++)
	{
		for (int j = result.cols - 1; j >= 0; j--)
			result[i][j] /= result[i][i];
		for (int k = i + 1; k < rows; k++)
		{
			float coeff = result[k][i];
			for (int j = 0; j < result.cols; j++)
			{
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}

	for (int j = result.cols - 1; j >= rows - 1; j--)
		result[rows - 1][j] /= result[rows - 1][rows - 1];

	for (int i = rows - 1; i > 0; i--)
	{
		for (int k = i - 1; k >= 0; k--)
		{
			float coeff = result[k][i];
			for (int j = 0; j < result.cols; j++)
			{
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}

	Matrix truncate(rows, cols);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			truncate[i][j] = result[i][j + cols];
	return truncate;
}

std::ostream &operator<<(std::ostream &s, MyMtrix::Matrix &m)
{
	for (int i = 0; i < m.nrows(); i++)
	{
		for (int j = 0; j < m.ncols(); j++)
		{
			s << m[i][j];
			if (j < m.ncols() - 1)
				s << "\t";
		}
		s << "\n";
	}
	return s;
}

Vec4f MyMtrix::Matrix::to_vec4()
{
	if (this->rows != 4 || this->cols != 1)
	{
		std::cerr << "Error: the matrix is not a 4x1 matrix\n";
		return Vec4f();
	}
	return Vec4f{m[0][0], m[1][0], m[2][0], m[3][0]};
}

MyMtrix::Matrix MyMtrix::toHomoCoordinate(Vec3f v , float w = 1.f)
{
	MyMtrix::Matrix temp(4, 1);
	temp[0][0] = v.x;
	temp[1][0] = v.y;
	temp[2][0] = v.z;
	temp[3][0] = w;
	return temp;
}


MyMtrix::Matrix MyMtrix::toMatrix(Vec4f v)
{
	MyMtrix::Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = v.w;
	return m;
}

MyMtrix::Matrix MyMtrix::toMatrix(Vec3f v)
{
	MyMtrix::Matrix m(3, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	return m;
}

// 计算行列式
template <int n>
struct dt
{
	static double det(const MyMtrix::Matrix &src)
	{
		double ret = 0;
		for (int i = n; i--; ret += src[0][i] * src.cofactor(0, i))
			;
		return ret;
	}
};

float MyMtrix::Matrix::det() const
{
	assert(rows == cols);
	if (rows == 1)
		return m[0][0];
	float ret = 0;
	for (int i = cols; i--; ret += this->m[0][i] * this->cofactor(0, i))
		;
	return ret;
}

MyMtrix::Matrix MyMtrix::Matrix::get_minor(const int row, const int col) const
{
	MyMtrix::Matrix ret(rows - 1, cols - 1);
	for (int i = rows - 1; i--;)
		for (int j = cols - 1; j--; ret[i][j] = m[i < row ? i : i + 1][j < col ? j : j + 1]);
	return ret;
}
MyMtrix::Matrix MyMtrix::Matrix::adjugate() const
{
	MyMtrix::Matrix ret(rows, cols);
	for (int i = rows; i--;)
		for (int j = cols; j--; ret[i][j] = cofactor(i, j))
			;
	return ret;
}

MyMtrix::Matrix MyMtrix::Matrix::invert_transpose() const
{
	MyMtrix::Matrix ret = adjugate();

	auto dot_product = [](const auto &lhs, const auto &rhs)
	{
		double ret = 0;
		for (int i = lhs.size(); i--; ret += lhs[i] * rhs[i]);
		return ret;
	};
	return ret / dot_product(ret[0], m[0]);
}