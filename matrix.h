#pragma once

#include "Point.h"

struct Matrix
{
	union {
		struct {
			float
				n11, n12, n13,
				n21, n22, n23,
				n31, n32, n33
			;
		};
		float t[3][3];
		float a[9];
	};
	
	float& operator () (int ri, int ci);
	const float& operator () (int ri, int ci) const;
	
	Matrix& operator *= (float v);
	
	Matrix& operator /= (float v);
	
	Matrix& operator *= (const Matrix& m);
	
	void Identity();
	Matrix& Transpose();
	
	static Matrix& Multiply(const Matrix& a, const Matrix& b, Matrix& c);
	static Matrix& MultiplyTranspose(const Matrix& a, const Matrix& b, Matrix& c);
	
	static Matrix Rotation(float angle);
	static Matrix Scaling(float x, float y);
	static Matrix Translation(float x, float y);

};

inline
Matrix operator * (const Matrix& a, const Matrix& b)
{
	Matrix c;
	Matrix::Multiply(a, b, c);
	return c;
}


inline
Point operator * (const Matrix& m, const Point& v)
{
	Point ret;
	ret.x = m.n11*v.x + m.n12*v.y + m.n13;
	ret.y = m.n21*v.x + m.n22*v.y + m.n23;
	return ret;
}

