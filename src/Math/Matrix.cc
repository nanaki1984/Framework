#include "Core/Debug.h"
#include "Core/Memory/Memory.h"
#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"

namespace Framework {
	namespace Math {

const Matrix Matrix::Zero = Matrix(0.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 0.0f);
const Matrix Matrix::Identity = Matrix(1.0f, 0.0f, 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f,
                                       0.0f, 0.0f, 0.0f, 1.0f);

Matrix::Matrix()
{ }

Matrix::Matrix(const float _11, const float _12, const float _13, const float _14,
               const float _21, const float _22, const float _23, const float _24,
               const float _31, const float _32, const float _33, const float _34,
               const float _41, const float _42, const float _43, const float _44)
{
    M[ 0] = _11; M[ 1] = _12; M[ 2] = _13; M[ 3] = _14;
    M[ 4] = _21; M[ 5] = _22; M[ 6] = _23; M[ 7] = _24;
    M[ 8] = _31; M[ 9] = _32; M[10] = _33; M[11] = _34;
    M[12] = _41; M[13] = _42; M[14] = _43; M[15] = _44;
}

Matrix::Matrix(const Vector3 &t)
{
    Memory::Copy(M, Identity.M, 16);

    M[12] = t.x;
    M[13] = t.y;
    M[14] = t.z;
}

Matrix::Matrix(const Vector3 &t, const Quaternion &r)
{
    float xy2 = 2.0f * r.i * r.j, xz2 = 2.0f * r.i * r.k, xw2 = 2.0f * r.i * r.w,
          yz2 = 2.0f * r.j * r.k, yw2 = 2.0f * r.j * r.w,
          zw2 = 2.0f * r.k * r.w,
          xx = r.i * r.i, yy = r.j * r.j, zz = r.k * r.k, ww = r.w * r.w;

    M[ 0] = (xx - yy - zz + ww);
    M[ 1] = (xy2 + zw2);
    M[ 2] = (xz2 - yw2);
    M[ 3] = 0.0f;
    M[ 4] = (xy2 - zw2);
    M[ 5] = (-xx + yy - zz + ww);
    M[ 6] = (yz2 + xw2);
    M[ 7] = 0.0f;
    M[ 8] = (xz2 + yw2);
    M[ 9] = (yz2 - xw2);
    M[10] = (-xx - yy + zz + ww);
    M[11] = 0.0f;
    M[12] = t.x;
    M[13] = t.y;
    M[14] = t.z;
    M[15] = 1.0f;
}

Matrix::Matrix(const Vector3 &t, const Vector3 &s)
{
    M[ 0] = s.x;
    M[ 1] = 0.0f;
    M[ 2] = 0.0f;
    M[ 3] = 0.0f;
    M[ 4] = 0.0f;
    M[ 5] = s.y;
    M[ 6] = 0.0f;
    M[ 7] = 0.0f;
    M[ 8] = 0.0f;
    M[ 9] = 0.0f;
    M[10] = s.z;
    M[11] = 0.0f;
    M[12] = t.x;
    M[13] = t.y;
    M[14] = t.z;
    M[15] = 1.0f;
}

Matrix::Matrix(const Quaternion &r, const Vector3 &s)
{
    float xy2 = 2.0f * r.i * r.j, xz2 = 2.0f * r.i * r.k, xw2 = 2.0f * r.i * r.w,
          yz2 = 2.0f * r.j * r.k, yw2 = 2.0f * r.j * r.w,
          zw2 = 2.0f * r.k * r.w,
          xx = r.i * r.i, yy = r.j * r.j, zz = r.k * r.k, ww = r.w * r.w;
    
    M[ 0] = (xx - yy - zz + ww) * s.x;
    M[ 1] = (xy2 + zw2) * s.x;
    M[ 2] = (xz2 - yw2) * s.x;
    M[ 3] = 0.0f;
    M[ 4] = (xy2 - zw2) * s.y;
    M[ 5] = (-xx + yy - zz + ww) * s.y;
    M[ 6] = (yz2 + xw2) * s.y;
    M[ 7] = 0.0f;
    M[ 8] = (xz2 + yw2) * s.z;
    M[ 9] = (yz2 - xw2) * s.z;
    M[10] = (-xx - yy + zz + ww) * s.z;
    M[11] = 0.0f;
    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = 0.0f;
    M[15] = 1.0f;
}

Matrix::Matrix(const Vector3 &t, const Quaternion &r, const Vector3 &s)
{
    float xy2 = 2.0f * r.i * r.j, xz2 = 2.0f * r.i * r.k, xw2 = 2.0f * r.i * r.w,
          yz2 = 2.0f * r.j * r.k, yw2 = 2.0f * r.j * r.w,
          zw2 = 2.0f * r.k * r.w,
          xx = r.i * r.i, yy = r.j * r.j, zz = r.k * r.k, ww = r.w * r.w;

    M[ 0] = (xx - yy - zz + ww) * s.x;
    M[ 1] = (xy2 + zw2) * s.x;
    M[ 2] = (xz2 - yw2) * s.x;
    M[ 3] = 0.0f;
    M[ 4] = (xy2 - zw2) * s.y;
    M[ 5] = (-xx + yy - zz + ww) * s.y;
    M[ 6] = (yz2 + xw2) * s.y;
    M[ 7] = 0.0f;
    M[ 8] = (xz2 + yw2) * s.z;
    M[ 9] = (yz2 - xw2) * s.z;
    M[10] = (-xx - yy + zz + ww) * s.z;
    M[11] = 0.0f;
    M[12] = t.x;
    M[13] = t.y;
    M[14] = t.z;
    M[15] = 1.0f;
}

Matrix::Matrix(const float *m)
{
    Memory::Copy(M, m, 16);
}

float&
Matrix::operator ()(unsigned int row, unsigned int col)
{
    return M[(row << 2) + col];
}

const float&
Matrix::operator ()(unsigned int row, unsigned int col) const
{
    return M[(row << 2) + col];
}

Matrix::operator const float*() const
{
    return M;
}

float
Matrix::GetDeterminant() const
{
    return ((M[ 0] * M[ 5] - M[ 4] * M[ 1]) * (M[10] * M[15] - M[14] * M[11]) - (M[ 0] * M[ 9] - M[ 8] * M[ 1]) * (M[ 6] * M[15] - M[14] * M[ 7])+
            (M[ 0] * M[13] - M[12] * M[ 1]) * (M[ 6] * M[11] - M[10] * M[ 7]) + (M[ 4] * M[ 9] - M[ 8] * M[ 5]) * (M[ 2] * M[15] - M[14] * M[ 3])-
            (M[ 4] * M[13] - M[12] * M[ 5]) * (M[ 2] * M[11] - M[10] * M[ 3]) + (M[ 8] * M[13] - M[12] * M[ 9]) * (M[ 2] * M[ 7] - M[ 6] * M[ 3]));
}

float
Matrix::GetFastDeterminant() const
{
    return ((M[ 0] * M[ 5] - M[ 4] * M[ 1]) * M[10] -
            (M[ 0] * M[ 9] - M[ 8] * M[ 1]) * M[ 6] +
            (M[ 4] * M[ 9] - M[ 8] * M[ 5]) * M[ 2]);
}

Matrix
Matrix::GetTransposed() const
{
    return Matrix(M[ 0], M[ 4], M[ 8], M[12],
                  M[ 1], M[ 5], M[ 9], M[13],
                  M[ 2], M[ 6], M[10], M[14],
                  M[ 3], M[ 7], M[11], M[15]);
}

Matrix
Matrix::GetInverse() const
{
    float d = this->GetDeterminant();
    assert(fabs(d) > Math::Epsilon);
    d = 1.0f / d;

    Matrix m;

    m(0, 0) = d * (M[ 5] * (M[10] * M[15] - M[14] * M[11]) + M[ 9] * (M[14] * M[ 7] - M[ 6] * M[15]) + M[13] * (M[ 6] * M[11] - M[10] * M[ 7]));
    m(1, 0) = d * (M[ 6] * (M[ 8] * M[15] - M[12] * M[11]) + M[10] * (M[12] * M[ 7] - M[ 4] * M[15]) + M[14] * (M[ 4] * M[11] - M[ 8] * M[ 7]));
    m(2, 0) = d * (M[ 7] * (M[ 8] * M[13] - M[12] * M[ 9]) + M[11] * (M[12] * M[ 5] - M[ 4] * M[13]) + M[15] * (M[ 4] * M[ 9] - M[ 8] * M[ 5]));
    m(3, 0) = d * (M[ 4] * (M[13] * M[10] - M[ 9] * M[14]) + M[ 8] * (M[ 5] * M[14] - M[13] * M[ 6]) + M[12] * (M[ 9] * M[ 6] - M[ 5] * M[10]));
    m(0, 1) = d * (M[ 9] * (M[ 2] * M[15] - M[14] * M[ 3]) + M[13] * (M[10] * M[ 3] - M[ 2] * M[11]) + M[ 1] * (M[14] * M[11] - M[10] * M[15]));
    m(1, 1) = d * (M[10] * (M[ 0] * M[15] - M[12] * M[ 3]) + M[14] * (M[ 8] * M[ 3] - M[ 0] * M[11]) + M[ 2] * (M[12] * M[11] - M[ 8] * M[15]));
    m(2, 1) = d * (M[11] * (M[ 0] * M[13] - M[12] * M[ 1]) + M[15] * (M[ 8] * M[ 1] - M[ 0] * M[ 9]) + M[ 3] * (M[12] * M[ 9] - M[ 8] * M[13]));
    m(3, 1) = d * (M[ 8] * (M[13] * M[ 2] - M[ 1] * M[14]) + M[12] * (M[ 1] * M[10] - M[ 9] * M[ 2]) + M[ 0] * (M[ 9] * M[14] - M[13] * M[10]));
    m(0, 2) = d * (M[13] * (M[ 2] * M[ 7] - M[ 6] * M[ 3]) + M[ 1] * (M[ 6] * M[15] - M[14] * M[ 7]) + M[ 5] * (M[14] * M[ 3] - M[ 2] * M[15]));
    m(1, 2) = d * (M[14] * (M[ 0] * M[ 7] - M[ 4] * M[ 3]) + M[ 2] * (M[ 4] * M[15] - M[12] * M[ 7]) + M[ 6] * (M[12] * M[ 3] - M[ 0] * M[15]));
    m(2, 2) = d * (M[15] * (M[ 0] * M[ 5] - M[ 4] * M[ 1]) + M[ 3] * (M[ 4] * M[13] - M[12] * M[ 5]) + M[ 7] * (M[12] * M[ 1] - M[ 0] * M[13]));
    m(3, 2) = d * (M[12] * (M[ 5] * M[ 2] - M[ 1] * M[ 6]) + M[ 0] * (M[13] * M[ 6] - M[ 5] * M[14]) + M[ 4] * (M[ 1] * M[14] - M[13] * M[ 2]));
    m(0, 3) = d * (M[ 1] * (M[10] * M[ 7] - M[ 6] * M[11]) + M[ 5] * (M[ 2] * M[11] - M[10] * M[ 3]) + M[ 9] * (M[ 6] * M[ 3] - M[ 2] * M[ 7]));
    m(1, 3) = d * (M[ 2] * (M[ 8] * M[ 7] - M[ 4] * M[11]) + M[ 6] * (M[ 0] * M[11] - M[ 8] * M[ 3]) + M[10] * (M[ 4] * M[ 3] - M[ 0] * M[ 7]));
    m(2, 3) = d * (M[ 3] * (M[ 8] * M[ 5] - M[ 4] * M[ 9]) + M[ 7] * (M[ 0] * M[ 9] - M[ 8] * M[ 1]) + M[11] * (M[ 4] * M[ 1] - M[ 0] * M[ 5]));
    m(3, 3) = d * (M[ 0] * (M[ 5] * M[10] - M[ 9] * M[ 6]) + M[ 4] * (M[ 9] * M[ 2] - M[ 1] * M[10]) + M[ 8] * (M[ 1] * M[ 6] - M[ 5] * M[ 2]));
    
    return m;
}

Matrix
Matrix::GetFastInverse() const
{
    float d = this->GetFastDeterminant();
    assert(fabs(d) > Math::Epsilon);
    d = 1.0f / d;

    Matrix m;

    m(0, 0) = d * (M[ 5] * M[10] - M[ 6] * M[ 9]);
    m(0, 1) = d * (M[ 9] * M[ 2] - M[10] * M[ 1]);
    m(0, 2) = d * (M[ 1] * M[ 6] - M[ 2] * M[ 5]);
    m(0, 3) = 0.0f;
    m(1, 0) = d * (M[ 6] * M[ 8] - M[ 4] * M[10]);
    m(1, 1) = d * (M[10] * M[ 0] - M[ 8] * M[ 2]);
    m(1, 2) = d * (M[ 2] * M[ 4] - M[ 0] * M[ 6]);
    m(1, 3) = 0.0f;
    m(2, 0) = d * (M[ 4] * M[ 9] - M[ 5] * M[ 8]);
    m(2, 1) = d * (M[ 8] * M[ 1] - M[ 9] * M[ 0]);
    m(2, 2) = d * (M[ 0] * M[ 5] - M[ 1] * M[ 4]);
    m(2, 3) = 0.0f;
    m(3, 0) = d * (M[ 4] * (M[13] * M[10] - M[ 9] * M[14]) + M[ 8] * (M[ 5] * M[14] - M[13] * M[ 6]) + M[12] * (M[ 9] * M[ 6] - M[ 5] * M[10]));
    m(3, 1) = d * (M[ 8] * (M[13] * M[ 2] - M[ 1] * M[14]) + M[12] * (M[ 1] * M[10] - M[ 9] * M[ 2]) + M[ 0] * (M[ 9] * M[14] - M[13] * M[10]));
    m(3, 2) = d * (M[12] * (M[ 5] * M[ 2] - M[ 1] * M[ 6]) + M[ 0] * (M[13] * M[ 6] - M[ 5] * M[14]) + M[ 4] * (M[ 1] * M[14] - M[13] * M[ 2]));
    m(3, 3) = 1.0f;

    return m;
}

void
Matrix::Transpose()
{
    std::swap(M[ 1], M[ 4]);
    std::swap(M[ 2], M[ 8]);
    std::swap(M[ 3], M[12]);
    std::swap(M[ 6], M[ 9]);
    std::swap(M[ 7], M[13]);
    std::swap(M[11], M[14]);
}

void
Matrix::Invert()
{
    (*this) = this->GetInverse();
}

void
Matrix::FastInvert()
{
    (*this) = this->GetFastInverse();
}

Matrix
Matrix::operator *(float s) const
{
    Matrix m;
    for (int i = 0; i < 16; ++i)
        m.M[i] = M[i] * s;
    return m;
}

Matrix&
Matrix::operator *=(float s)
{
    for (int i = 0; i < 16; ++i)
        M[i] *= s;
    return (*this);
}

Matrix
Matrix::operator /(float s) const
{
    return this->operator*(1.0f / s);
}

Matrix&
Matrix::operator /=(float s)
{
    return this->operator*=(1.0f / s);
}

Matrix
Matrix::operator *(const Matrix &m) const
{
    Matrix mat;
    mat.M[ 0] = M[ 0] * m.M[ 0] + M[ 1] * m.M[ 4] + M[ 2] * m.M[ 8] + M[ 3] * m.M[12];
    mat.M[ 1] = M[ 0] * m.M[ 1] + M[ 1] * m.M[ 5] + M[ 2] * m.M[ 9] + M[ 3] * m.M[13];
    mat.M[ 2] = M[ 0] * m.M[ 2] + M[ 1] * m.M[ 6] + M[ 2] * m.M[10] + M[ 3] * m.M[14];
    mat.M[ 3] = M[ 0] * m.M[ 3] + M[ 1] * m.M[ 7] + M[ 2] * m.M[11] + M[ 3] * m.M[15];
    mat.M[ 4] = M[ 4] * m.M[ 0] + M[ 5] * m.M[ 4] + M[ 6] * m.M[ 8] + M[ 7] * m.M[12];
    mat.M[ 5] = M[ 4] * m.M[ 1] + M[ 5] * m.M[ 5] + M[ 6] * m.M[ 9] + M[ 7] * m.M[13];
    mat.M[ 6] = M[ 4] * m.M[ 2] + M[ 5] * m.M[ 6] + M[ 6] * m.M[10] + M[ 7] * m.M[14];
    mat.M[ 7] = M[ 4] * m.M[ 3] + M[ 5] * m.M[ 7] + M[ 6] * m.M[11] + M[ 7] * m.M[15];
    mat.M[ 8] = M[ 8] * m.M[ 0] + M[ 9] * m.M[ 4] + M[10] * m.M[ 8] + M[11] * m.M[12];
    mat.M[ 9] = M[ 8] * m.M[ 1] + M[ 9] * m.M[ 5] + M[10] * m.M[ 9] + M[11] * m.M[13];
    mat.M[10] = M[ 8] * m.M[ 2] + M[ 9] * m.M[ 6] + M[10] * m.M[10] + M[11] * m.M[14];
    mat.M[11] = M[ 8] * m.M[ 3] + M[ 9] * m.M[ 7] + M[10] * m.M[11] + M[11] * m.M[15];
    mat.M[12] = M[12] * m.M[ 0] + M[13] * m.M[ 4] + M[14] * m.M[ 8] + M[15] * m.M[12];
    mat.M[13] = M[12] * m.M[ 1] + M[13] * m.M[ 5] + M[14] * m.M[ 9] + M[15] * m.M[13];
    mat.M[14] = M[12] * m.M[ 2] + M[13] * m.M[ 6] + M[14] * m.M[10] + M[15] * m.M[14];
    mat.M[15] = M[12] * m.M[ 3] + M[13] * m.M[ 7] + M[14] * m.M[11] + M[15] * m.M[15];
    return mat;
}

Matrix&
Matrix::operator *=(const Matrix &m)
{
    float new_mat[16];
    new_mat[ 0] = M[ 0] * m.M[ 0] + M[ 1] * m.M[ 4] + M[ 2] * m.M[ 8] + M[ 3] * m.M[12];
    new_mat[ 1] = M[ 0] * m.M[ 1] + M[ 1] * m.M[ 5] + M[ 2] * m.M[ 9] + M[ 3] * m.M[13];
    new_mat[ 2] = M[ 0] * m.M[ 2] + M[ 1] * m.M[ 6] + M[ 2] * m.M[10] + M[ 3] * m.M[14];
    new_mat[ 3] = M[ 0] * m.M[ 3] + M[ 1] * m.M[ 7] + M[ 2] * m.M[11] + M[ 3] * m.M[15];
    new_mat[ 4] = M[ 4] * m.M[ 0] + M[ 5] * m.M[ 4] + M[ 6] * m.M[ 8] + M[ 7] * m.M[12];
    new_mat[ 5] = M[ 4] * m.M[ 1] + M[ 5] * m.M[ 5] + M[ 6] * m.M[ 9] + M[ 7] * m.M[13];
    new_mat[ 6] = M[ 4] * m.M[ 2] + M[ 5] * m.M[ 6] + M[ 6] * m.M[10] + M[ 7] * m.M[14];
    new_mat[ 7] = M[ 4] * m.M[ 3] + M[ 5] * m.M[ 7] + M[ 6] * m.M[11] + M[ 7] * m.M[15];
    new_mat[ 8] = M[ 8] * m.M[ 0] + M[ 9] * m.M[ 4] + M[10] * m.M[ 8] + M[11] * m.M[12];
    new_mat[ 9] = M[ 8] * m.M[ 1] + M[ 9] * m.M[ 5] + M[10] * m.M[ 9] + M[11] * m.M[13];
    new_mat[10] = M[ 8] * m.M[ 2] + M[ 9] * m.M[ 6] + M[10] * m.M[10] + M[11] * m.M[14];
    new_mat[11] = M[ 8] * m.M[ 3] + M[ 9] * m.M[ 7] + M[10] * m.M[11] + M[11] * m.M[15];
    new_mat[12] = M[12] * m.M[ 0] + M[13] * m.M[ 4] + M[14] * m.M[ 8] + M[15] * m.M[12];
    new_mat[13] = M[12] * m.M[ 1] + M[13] * m.M[ 5] + M[14] * m.M[ 9] + M[15] * m.M[13];
    new_mat[14] = M[12] * m.M[ 2] + M[13] * m.M[ 6] + M[14] * m.M[10] + M[15] * m.M[14];
    new_mat[15] = M[12] * m.M[ 3] + M[13] * m.M[ 7] + M[14] * m.M[11] + M[15] * m.M[15];
    Memory::Copy(M, new_mat, 16);
    return (*this);
}

Vector3
Matrix::operator *(const Vector3 &v) const
{
    float oow = 1.0f / (v.x * M[ 3] + v.y * M[ 7] + v.z * M[11] + M[15]);
    return Vector3((v.x * M[ 0] + v.y * M[ 4] + v.z * M[ 8] + M[12]) * oow,
                   (v.x * M[ 1] + v.y * M[ 5] + v.z * M[ 9] + M[13]) * oow,
                   (v.x * M[ 2] + v.y * M[ 6] + v.z * M[10] + M[14]) * oow);
}

Vector4
Matrix::operator *(const Vector4 &v) const
{
    return Vector4(v.x * M[ 0] + v.y * M[ 4] + v.z * M[ 8] + v.w * M[12],
                   v.x * M[ 1] + v.y * M[ 5] + v.z * M[ 9] + v.w * M[13],
                   v.x * M[ 2] + v.y * M[ 6] + v.z * M[10] + v.w * M[14],
                   v.x * M[ 3] + v.y * M[ 7] + v.z * M[11] + v.w * M[15]);
}

const Vector4&
Matrix::GetColumn(int index) const
{
    assert(index >= 0 && index < 4);
    return *reinterpret_cast<const Vector4*>(M + (index << 2));
}

Vector4&
Matrix::GetColumn(int index)
{
    assert(index >= 0 && index < 4);
    return *reinterpret_cast<Vector4*>(M + (index << 2));
}

Vector3
Matrix::MultiplyPoint(const Vector3 &v) const
{
	float oow = v.x * M[3] + v.y * M[7] + v.z * M[11] + M[15];

	if (oow > Epsilon)
		oow = 1.0f / oow;
	else
		oow = 1.0f;

	return Vector3((v.x * M[0] + v.y * M[4] + v.z * M[ 8] + M[12]) * oow,
				   (v.x * M[1] + v.y * M[5] + v.z * M[ 9] + M[13]) * oow,
				   (v.x * M[2] + v.y * M[6] + v.z * M[10] + M[14]) * oow);
}

Matrix
Matrix::FastMultiply(const Matrix &m) const
{
    Matrix mat;
    mat.M[ 0] = M[ 0] * m.M[ 0] + M[ 1] * m.M[ 4] + M[ 2] * m.M[ 8];
    mat.M[ 1] = M[ 0] * m.M[ 1] + M[ 1] * m.M[ 5] + M[ 2] * m.M[ 9];
    mat.M[ 2] = M[ 0] * m.M[ 2] + M[ 1] * m.M[ 6] + M[ 2] * m.M[10];
    mat.M[ 3] = 0.0f;
    mat.M[ 4] = M[ 4] * m.M[ 0] + M[ 5] * m.M[ 4] + M[ 6] * m.M[ 8];
    mat.M[ 5] = M[ 4] * m.M[ 1] + M[ 5] * m.M[ 5] + M[ 6] * m.M[ 9];
    mat.M[ 6] = M[ 4] * m.M[ 2] + M[ 5] * m.M[ 6] + M[ 6] * m.M[10];
    mat.M[ 7] = 0.0f;
    mat.M[ 8] = M[ 8] * m.M[ 0] + M[ 9] * m.M[ 4] + M[10] * m.M[ 8];
    mat.M[ 9] = M[ 8] * m.M[ 1] + M[ 9] * m.M[ 5] + M[10] * m.M[ 9];
    mat.M[10] = M[ 8] * m.M[ 2] + M[ 9] * m.M[ 6] + M[10] * m.M[10];
    mat.M[11] = 0.0f;
    mat.M[12] = M[12] * m.M[ 0] + M[13] * m.M[ 4] + M[14] * m.M[ 8] + m.M[12];
    mat.M[13] = M[12] * m.M[ 1] + M[13] * m.M[ 5] + M[14] * m.M[ 9] + m.M[13];
    mat.M[14] = M[12] * m.M[ 2] + M[13] * m.M[ 6] + M[14] * m.M[10] + m.M[14];
    mat.M[15] = 1.0f;
    return mat;
}

Vector3
Matrix::FastMultiplyPoint(const Vector3 &v) const
{
    return Vector3(v.x * M[ 0] + v.y * M[ 4] + v.z * M[ 8] + M[12],
                   v.x * M[ 1] + v.y * M[ 5] + v.z * M[ 9] + M[13],
                   v.x * M[ 2] + v.y * M[ 6] + v.z * M[10] + M[14]);
}

Vector3
Matrix::FastMultiplyVector(const Vector3 &v) const
{
    return Vector3(v.x * M[ 0] + v.y * M[ 4] + v.z * M[ 8],
                   v.x * M[ 1] + v.y * M[ 5] + v.z * M[ 9],
                   v.x * M[ 2] + v.y * M[ 6] + v.z * M[10]);
}

	} // namespace Math
} // namespace Framework
