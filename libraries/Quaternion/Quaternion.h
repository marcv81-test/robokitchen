#ifndef QUATERNION_H
#define QUATERNION_H

#include <Arduino.h>
#include "config.h"

class Quaternion;
class Vector;
class AxisAngle;

// ================================ Quaternion ================================ //

class Quaternion
{
  protected:
    float w, x, y, z;

  public:

    // Constructors
    Quaternion();
    Quaternion(float w, float x, float y, float z);
    Quaternion(const Vector& v);
    Quaternion(float w, const Vector& v);

    // Accessors
    float getW() const { return w; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    // Basic operations
    Quaternion& operator += (const Quaternion& q);
    Quaternion& operator -= (const Quaternion& q);
    Quaternion& operator *= (float f);
    Quaternion& operator /= (float f);
    const Quaternion operator + (const Quaternion& q) const;
    const Quaternion operator - (const Quaternion& q) const;
    const Quaternion operator * (float f) const;
    const Quaternion operator / (float f) const;

    // Quaternion operations
    float normSquare() const;
    float norm() const;
    Quaternion& normalise();
    const Quaternion conjugate() const;
    const Quaternion operator * (const Quaternion& q) const;

    // Conversions
    static const Quaternion fromAxisAngle(const AxisAngle& a);
};

// ================================ Vector ================================ //

class Vector
{
  protected:
    float x, y, z;

  public:

    // Constructors
    Vector();
    Vector(float x, float y, float z);
    Vector(const Quaternion& q);

    // Accessors
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    // Basic operations
    Vector& operator += (const Vector& v);
    Vector& operator -= (const Vector& v);
    Vector& operator *= (float f);
    Vector& operator /= (float f);
    const Vector operator + (const Vector& v) const;
    const Vector operator - (const Vector& v) const;
    const Vector operator * (float f) const;
    const Vector operator / (float f) const;

    // Vector operations
    float normSquare() const;
    float norm() const;
    Vector& normalise();
    float dot(const Vector& v) const;
    const Vector cross(const Vector& v) const;
    const Vector rotate(const Quaternion& q) const;
};

// ================================ AxisAngle ================================ //

class AxisAngle : public Vector
{
  public:

    // Constructors
    AxisAngle();
    AxisAngle(float x, float y, float z);
    AxisAngle(const Vector& v);

    // Conversions
    static const AxisAngle fromTwoVectors(const Vector& u, const Vector& v);
};

#endif // QUATERNION_H
