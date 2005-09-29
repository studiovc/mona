// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __Matrix__
#define __Matrix__

#pragma interface

#include <java/lang/Object.h>

extern "Java"
{
  class Vector;
  class Matrix;
}

class Matrix : public ::java::lang::Object
{
public: // actually package-private
  virtual void Initialize ();
  virtual void Initialize (::Matrix *);
  virtual void Mult (::Matrix *);
  virtual void Scale (jfloat, jfloat, jfloat);
  virtual void RotateX (jfloat);
  virtual void RotateY (jfloat);
  virtual void RotateZ (jfloat);
  virtual void Translate (jfloat, jfloat, jfloat);
  virtual ::Vector *Transform (::Vector *);
  virtual ::Vector *Rotation (::Vector *);
  virtual void Invert (::Matrix *);
  virtual void View (::Vector *, ::Vector *, ::Vector *);
  virtual void Projection (jfloat, jfloat);
  virtual void Projection (jfloat, jfloat, jfloat, jfloat);
  virtual void Clip (jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);
  virtual void ViewPort (jfloat, jfloat, jfloat, jfloat);
  virtual void Blend (::Matrix *, ::Matrix *, jfloat);
  virtual void Shadow (::Vector *);
public:
  Matrix ();
public: // actually package-private
  jfloat __attribute__((aligned(__alignof__( ::java::lang::Object ))))  m00;
  jfloat m01;
  jfloat m02;
  jfloat m03;
  jfloat m10;
  jfloat m11;
  jfloat m12;
  jfloat m13;
  jfloat m20;
  jfloat m21;
  jfloat m22;
  jfloat m23;
  jfloat m30;
  jfloat m31;
  jfloat m32;
  jfloat m33;
public:

  static ::java::lang::Class class$;
};

#endif /* __Matrix__ */
