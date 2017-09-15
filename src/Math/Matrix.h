#pragma once

namespace Framework {
	namespace Math {

class Vector3;
class Vector4;
class Quaternion;

class Matrix {
private:
    float M[16];
public:
    static const Matrix Zero;
    static const Matrix Identity;
    
    Matrix();
    Matrix(const float _11, const float _12, const float _13, const float _14,
           const float _21, const float _22, const float _23, const float _24,
           const float _31, const float _32, const float _33, const float _34,
           const float _41, const float _42, const float _43, const float _44);
    explicit Matrix(const Vector3 &t);
    Matrix(const Vector3 &t, const Quaternion &r);
    Matrix(const Vector3 &t, const Vector3 &s);
    Matrix(const Quaternion &r, const Vector3 &s);
    Matrix(const Vector3 &t, const Quaternion &r, const Vector3 &s);
    explicit Matrix(const float *m);

    float& operator ()(unsigned int row, unsigned int col);
    const float& operator ()(unsigned int row, unsigned int col) const;
    operator const float*() const;
    
    float GetDeterminant() const;
    float GetFastDeterminant() const;

    Matrix GetTransposed() const;
    Matrix GetInverse() const;
    Matrix GetFastInverse() const;

    void Transpose();
    void Invert();
    void FastInvert();

    Matrix operator *(float s) const;
    Matrix& operator *=(float s);
    Matrix operator /(float s) const;
    Matrix& operator /=(float s);

    Matrix operator *(const Matrix &m) const;
    Matrix& operator *=(const Matrix &m);
    Vector3 operator *(const Vector3 &v) const;
    Vector4 operator *(const Vector4 &v) const;

    const Vector4& GetColumn(int index) const;
    Vector4& GetColumn(int index);

	Vector3 MultiplyPoint(const Vector3 &v) const;
	
	Matrix FastMultiply(const Matrix &m) const;
    Vector3 FastMultiplyPoint(const Vector3 &v) const;
    Vector3 FastMultiplyVector(const Vector3 &v) const;
};
    
	} // namespace Math
} // namespace Framework
