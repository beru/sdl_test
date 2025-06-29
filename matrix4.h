#pragma once

/*

四元正方行列

http://www12.plala.or.jp/ksp/formula/mathFormula/html/node79.html

*/

#include "vector4.h"

#include <cmath>
#include <utility>

template <typename T>
class alignas(16) Matrix4
{
public:
	union {
		struct {
			float
				n11, n12, n13, n14,
				n21, n22, n23, n24,
				n31, n32, n33, n34,
				n41, n42, n43, n44
			;
		};
		T t[4][4];
		T a[16];
	};

	T& operator () (int ri, int ci);
	T operator () (int ri, int ci) const;
	
	template <typename T2>
	Matrix4& operator *= (T2 v);
	
	template <typename T2>
	Matrix4& operator /= (T2 v);
	
	Matrix4& operator *= (const Matrix4& m);
	
	Matrix4& Transpose();

	
	static Matrix4 Identity();
	static Matrix4 Multiply(const Matrix4& a, const Matrix4& b);
	static Matrix4 MultiplyTranspose(const Matrix4& a, const Matrix4& b);
	static Matrix4 Inverse(const Matrix4& m);
	
	template <typename T2>
	static Matrix4 RotationX(T2 angle);
	
	template <typename T2>
	static Matrix4 RotationY(T2 angle);
	
	template <typename T2>
	static Matrix4 RotationZ(T2 angle);
	
	template <typename T2>
	static Matrix4 Scaling(T2 sx, T2 sy, T2 sz);

	template <typename T2>
	static Matrix4 Scaling(T2 sx, T2 sy, T2 sz, T2 cx, T2 cy, T2 cz);

	template <typename T2>
	static Matrix4 Translation(T2 x, T2 y, T2 z);
	
};

template <typename T>
T& Matrix4<T>::operator () (int ri, int ci)
{
	return t[ri][ci];
}

template <typename T>
T Matrix4<T>::operator () (int ri, int ci) const
{
	return t[ri][ci];
}

template <typename T>
template <typename T2>
Matrix4<T>& Matrix4<T>::operator *= (T2 v)
{
	for (size_t i=0; i<16; ++i) {
		a[i] *= v;
	}
	return *this;
}

template <typename T>
template <typename T2>
Matrix4<T>& Matrix4<T>::operator /= (T2 v)
{
	for (size_t i=0; i<16; ++i) {
		a[i] /= v;
	}
	return *this;
}

template <typename T>
Matrix4<T> operator * (const Matrix4<T>& a, const Matrix4<T>& b)
{
	Matrix4<T> r;
	r = Matrix4<T>::Multiply(a, b);
	return r;
}

template <typename T, typename T2>
Matrix4<T> operator * (const Matrix4<T>& a, T2 b)
{
	Matrix4<T> c = a;
	return c *= b;
}

template <typename T, typename T2>
Matrix4<T> operator * (T2 a, const Matrix4<T>& b)
{
	Matrix4<T> c = b;
	return b *= a;
}

template <typename T, typename T2>
Matrix4<T> operator / (const Matrix4<T>& a, T2 b)
{
	Matrix4<T> c = a;
	return c /= b;
}

template <typename T, typename T2>
Matrix4<T> operator / (T2 a, const Matrix4<T>& b)
{
	Matrix4<T> c = b;
	return b /= a;
}

template <typename T>
Matrix4<T>& Matrix4<T>::operator *= (const Matrix4<T>& m)
{
	Matrix4 tmp = *this;
	*this = tmp * m;
	return *this;
}

template <typename T>
Matrix4<T> Matrix4<T>::Identity()
{
	Matrix4<T> r;
	r(0, 0) = 1; r(0, 1) = 0; r(0, 2) = 0; r(0, 3) = 0;
	r(1, 0) = 0; r(1, 1) = 1; r(1, 2) = 0; r(1, 3) = 0;
	r(2, 0) = 0; r(2, 1) = 0; r(2, 2) = 1; r(2, 3) = 0;
	r(3, 0) = 0; r(3, 1) = 0; r(3, 2) = 0; r(3, 3) = 1;
	return r;
}

template <typename T>
Matrix4<T>& Matrix4<T>::Transpose()
{
	std::swap(t[0][1], t[1][0]);
	std::swap(t[0][2], t[2][0]);
	std::swap(t[0][3], t[3][0]);
	std::swap(t[1][2], t[2][1]);
	std::swap(t[1][3], t[3][1]);
	std::swap(t[2][3], t[3][2]);
	return *this;
}

template <typename T>
Matrix4<T> Matrix4<T>::Multiply(const Matrix4<T>& a, const Matrix4<T>& b)
{
	Matrix4<T> d;
	for (size_t r=0; r<4; ++r) {
		for (size_t c=0; c<4; ++c) {
			d(r, c) = a(r,0)*b(0,c) + a(r,1)*b(1,c) + a(r,2)*b(2,c) + a(r,3)*b(3,c);
		}
	}
	return d;
}

template <typename T>
Matrix4<T> Matrix4<T>::MultiplyTranspose(const Matrix4<T>& a, const Matrix4<T>& b)
{
	Matrix4<T> c = Matrix4::Multiply(a, b);
	c.Transpose();
	return c;
}

template <typename T>
Vector4<T> operator * (const Matrix4<T>& m, const Vector4<T>& v)
{
	return
		Vector4<T>(
			m.n11*v.x + m.n12*v.y + m.n13*v.z + m.n14,
			m.n21*v.x + m.n22*v.y + m.n23*v.z + m.n24,
			m.n31*v.x + m.n32*v.y + m.n33*v.z + m.n34,
			m.n41*v.x + m.n42*v.y + m.n43*v.z + m.n44
		);
}

// X軸中心回転
// 1    0    0    0
// 0    cos  -sin 0
// 0    sin  cos  0
// 0    0    0    1
template <typename T>
template <typename T2>
Matrix4<T> Matrix4<T>::RotationX(T2 angle)
{
	Matrix4<T> m = Matrix4<T>::Identity();
	T s = sin(angle);
	T c = cos(angle);
	m.n22 = c;
	m.n23 = -s;
	m.n32 = s;
	m.n33 = c;
	return m;
}

// Y軸中心回転
// cos  0  sin 0
// 0    1  0   0
// -sin 0  cos 0
// 0    0  0   1
template <typename T>
template <typename T2>
Matrix4<T> Matrix4<T>::RotationY(T2 angle)
{
	Matrix4<T> m = Matrix4<T>::Identity();
	T s = sin(angle);
	T c = cos(angle);
	m.n11 = c;
	m.n13 = s;
	m.n31 = -s;
	m.n33 = c;
	return m;
}

// Z軸中心回転
// cos -sin 0 0
// sin cos  0 0
// 0   0    1 0
// 0   0    0 1
template <typename T>
template <typename T2>
Matrix4<T> Matrix4<T>::RotationZ(T2 angle)
{
	Matrix4<T> m = Matrix4<T>::Identity();
	T s = sin(angle);
	T c = cos(angle);
	m.n11 = c;
	m.n12 = -s;
	m.n21 = s;
	m.n22 = c;
	return m;
}

// 拡大縮小行列
// a 0 0 0
// 0 b 0 0
// 0 0 c 0
// 0 0 0 1
template <typename T>
template <typename T2>
Matrix4<T> Matrix4<T>::Scaling(T2 sx, T2 sy, T2 sz)
{
	Matrix4<T> m = Matrix4<T>::Identity();
	m.n11 = sx;
	m.n22 = sy;
	m.n33 = sz;
	return m;
}

template <typename T>
template <typename T2>
Matrix4<T> Matrix4<T>::Scaling(T2 sx, T2 sy, T2 sz, T2 cx, T2 cy, T2 cz)
{
	Matrix4<T> m = Matrix4<T>::Identity();
	m.n11 = sx;
	m.n22 = sy;
	m.n33 = sz;
	m.n14 = cx - sx * cx;
	m.n24 = cy - sy * cy;
	m.n34 = cz - sz * cz;
	return m;
}


// 平行移動行列
// 1 0 0 x
// 0 1 0 y
// 0 0 1 z
// 0 0 0 1
template <typename T>
template <typename T2>
Matrix4<T> Matrix4<T>::Translation(T2 x, T2 y, T2 z)
{
	Matrix4<T> m = Matrix4<T>::Identity();
	m.n14 = x;
	m.n24 = y;
	m.n34 = z;
	return m;
}

