// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __Applet1__
#define __Applet1__

#pragma interface

#include <java/lang/Object.h>
#include <gcj/array.h>

extern "Java"
{
  class Applet1;
  class Render;
  class Vertex;
  class MyTexture;
}

class Applet1 : public ::java::lang::Object
{
public:
  virtual void init ();
  virtual void run ();
  virtual void stop () { }
  virtual void MainLoop ();
public: // actually package-private
  virtual void DrawObjects ();
public:
  static void main (JArray< ::java::lang::String *> *);
  Applet1 ();
public: // actually package-private
  ::MyTexture * __attribute__((aligned(__alignof__( ::java::lang::Object )))) texture;
  static const jint SCREENW = 360L;
  static const jint SCREENH = 160L;
  static const jint FRAMERATE = 10L;
  static const jint DETAILLEVEL = 12L;
  jfloat rx;
  jfloat ry;
  jfloat rz;
  JArray< ::Vertex *> *vertices;
  jintArray indices;
  ::Render *render;
  jfloat ag;
public:

  static ::java::lang::Class class$;
};

#endif /* __Applet1__ */
