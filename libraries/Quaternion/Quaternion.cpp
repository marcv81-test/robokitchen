#ifndef UNIT_TEST
  #include <Arduino.h>
#else
  #include "ArduinoMock.h"
#endif

#include "Quaternion.h"

#include <math.h>

// ================================ Quaternion ================================ //

Quaternion::Quaternion() :
  w(1.0), x(0.0), y(0.0), z(0.0) { }

Quaternion::Quaternion(float w, float x, float y, float z) :
  w(w), x(x), y(y), z(z) { }

Quaternion::Quaternion(const Vector& v) :
  w(0.0), x(v.getX()), y(v.getY()), z(v.getZ()) { }

Quaternion::Quaternion(float w, const Vector& v) :
  w(w), x(v.getX()), y(v.getY()), z(v.getZ()) { }

Quaternion& Quaternion::operator += (const Quaternion& q)
{
  w += q.w; x += q.x; y += q.y; z += q.z;
  return *this;
}

Quaternion& Quaternion::operator -= (const Quaternion& q)
{
  w -= q.w; x -= q.x; y -= q.y; z -= q.z;
  return *this;
}

Quaternion& Quaternion::operator *= (float f)
{
  w *= f; x *= f; y *= f; z *= f;
  return *this;
}

Quaternion& Quaternion::operator /= (float f)
{
  w /= f; x /= f; y /= f; z /= f;
  return *this;
}

const Quaternion Quaternion::operator + (const Quaternion& q) const
{
  return Quaternion(*this) += q;
}

const Quaternion Quaternion::operator - (const Quaternion& q) const
{
  return Quaternion(*this) -= q;
}

const Quaternion Quaternion::operator * (float f) const
{
  return Quaternion(*this) *= f;
}

const Quaternion Quaternion::operator / (float f) const
{
  return Quaternion(*this) /= f;
}

float Quaternion::normSquare() const
{
  return sq(w) + sq(x) + sq(y) + sq(z);
}

float Quaternion::norm() const
{
  return sqrt(normSquare());
}

Quaternion& Quaternion::normalise()
{
  return *this /= this->norm();
}

const Quaternion Quaternion::conjugate() const
{
  return Quaternion(w, -x, -y, -z);
}

const Quaternion Quaternion::operator *(const Quaternion& q) const
{
  return Quaternion(
    (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z),
    (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y),
    (w * q.y) - (x * q.z) + (y * q.w) + (z * q.x),
    (w * q.z) + (x * q.y) - (y * q.x) + (z * q.w)
  );
}

const Quaternion Quaternion::fromAxisAngle(const AxisAngle& a)
{
  float angle = a.norm();
  float cosHalfAngle = cos(angle * 0.5);
  float sinHalfAngle = sqrt(1.0 - sq(cosHalfAngle));
  float ratio = sinHalfAngle / angle;
  if(ratio != ratio) ratio = 0.0; // Avoid ratio singularity
  return Quaternion(
    cosHalfAngle,
    a.getX() * ratio,
    a.getY() * ratio,
    a.getZ() * ratio
  );
}

float Quaternion::getRoll() const
{
  return atan2(2*(w*x + y*z), 1 - 2*(x*x + y*y));
}

float Quaternion::getPitch() const
{
  return asin(2*(w*y - x*z));
}

float Quaternion::getYaw() const
{
  return atan2(2*(w*z + x*y), 1 - 2*(y*y + z*z));
}

// ================================ Vector ================================ //

Vector::Vector() :
  x(0.0), y(0.0), z(0.0) { }

Vector::Vector(float x, float y, float z) :
  x(x), y(y), z(z) { }

Vector::Vector(const Quaternion& q) :
  x(q.getX()), y(q.getY()), z(q.getZ()) { }

Vector& Vector::operator += (const Vector& v)
{
  x += v.x; y += v.y; z += v.z;
  return *this;
}

Vector& Vector::operator -= (const Vector& v)
{
  x -= v.x; y -= v.y; z -= v.z;
  return *this;
}

Vector& Vector::operator *= (float f)
{
  x *= f; y *= f; z *= f;
  return *this;
}

Vector& Vector::operator /= (float f)
{
  x /= f; y /= f; z /= f;
  return *this;
}

const Vector Vector::operator + (const Vector& v) const
{
  return Vector(*this) += v;
}

const Vector Vector::operator - (const Vector& v) const
{
  return Vector(*this) -= v;
}

const Vector Vector::operator * (float f) const
{
  return Vector(*this) *= f;
}

const Vector Vector::operator / (float f) const
{
  return Vector(*this) /= f;
}

float Vector::normSquare() const
{
  return sq(x) + sq(y) + sq(z);
}

float Vector::norm() const
{
  return sqrt(normSquare());
}

Vector& Vector::normalise()
{
  return *this /= this->norm();
}

float Vector::dot(const Vector& v) const
{
  return (x * v.x) + (y * v.y) + (z * v.z);
}

const Vector Vector::cross(const Vector& v) const
{
  return Vector(
    (y * v.z) - (z * v.y),
    (z * v.x) - (x * v.z),
    (x * v.y) - (y * v.x)
  );
}

const Vector Vector::rotate(const Quaternion& q) const
{
  Vector v = Vector(q);
  Vector t = v.cross(*this) * 2.0;
  return *this + (t * q.getW()) + v.cross(t);
}

const Vector Vector::project(const Vector& v) const
{
  return v * (v.dot(*this) / v.normSquare());
}

const Vector Vector::reject(const Vector& v) const
{
  return *this - (*this).project(v);
}

// ================================ AxisAngle ================================ //

AxisAngle::AxisAngle() :
  Vector() { }

AxisAngle::AxisAngle(float x, float y, float z) :
  Vector(x, y, z) { }

AxisAngle::AxisAngle(const Vector& v) :
  Vector(v) { }

const AxisAngle AxisAngle::fromTwoVectors(const Vector& u, const Vector& v)
{
  AxisAngle a = u.cross(v);
  float norm = sqrt(u.normSquare() * v.normSquare());
  if(norm != 0.0) a /= norm; // Avoid singularity if u or v is null
  float sinAngle = a.norm();
  float ratio = asin(sinAngle) / sinAngle;
  if(ratio != ratio) ratio = 0.0; // Avoid ratio singularity
  return a * ratio;
}
