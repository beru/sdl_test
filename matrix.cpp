#include "matrix.h"

#include <cmath>
#include <algorithm>

float& Matrix::operator () (int ri, int ci)
{
	return t[ri][ci];
}

const float& Matrix::operator () (int ri, int ci) const
{
	return t[ri][ci];
}


Matrix& Matrix::operator *= (float v)
{
	for (size_t i=0; i<9; ++i) {
		a[i] *= v;
	}
	return *this;
}

Matrix& Matrix::operator /= (float v)
{
	return *this *= (1.0f / v);
}

Matrix& Matrix::operator *= (const Matrix& m)
{
	Matrix tmp = *this;
	*this = tmp * m;
	return *this;
}

void Matrix::Identity()
{
	t[0][0] = 1; t[0][1] = 0; t[0][2] = 0;
	t[1][0] = 0; t[1][1] = 1; t[1][2] = 0;
	t[2][0] = 0; t[2][1] = 0; t[2][2] = 1;
}

Matrix& Matrix::Transpose()
{
	std::swap(t[0][1], t[1][0]);
	std::swap(t[0][2], t[2][0]);
	std::swap(t[1][2], t[2][1]);
	return *this;
}

// static
Matrix& Matrix::Multiply(const Matrix& ma, const Matrix& mb, Matrix& mc)
{
	for (size_t r=0; r<3; ++r) {
		for (size_t c=0; c<3; ++c) {
			mc(r, c) = ma(r,0)*mb(0,c) + ma(r,1)*mb(1,c) + ma(r,2)*mb(2,c);
		}
	}
	return mc;
}

// static
Matrix& Matrix::MultiplyTranspose(const Matrix& a, const Matrix& b, Matrix& c)
{
	Matrix::Multiply(a, b, c);
	c.Transpose();
	return c;
}

#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

// static
Matrix Matrix::Rotation(float angle)
{
	Matrix m;
	m.Identity();
	float rad = DEG2RAD(angle);
	float s = sin(rad);
	float c = cos(rad);
	m.n11 = c;
	m.n12 = -s;
	m.n21 = s;
	m.n22 = c;
	return m;
}

// static
Matrix Matrix::Scaling(float x, float y)
{
	Matrix m;
	m.Identity();
	m.n11 = x;
	m.n22 = y;
	return m;
}

// static
Matrix Matrix::Translation(float x, float y)
{
	Matrix m;
	m.Identity();
	m.n13 = x;
	m.n23 = y;
	return m;
}

