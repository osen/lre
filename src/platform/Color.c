#include "Color.h"

struct Color ColorRgb(unsigned r, unsigned g, unsigned b)
{
  struct Color rtn = {0};

  rtn.r = r;
  rtn.g = g;
  rtn.b = b;
  rtn.a = 255;

  return rtn;
}

struct Color ColorRgba(unsigned r, unsigned g, unsigned b, unsigned a)
{
  struct Color rtn = {0};

  rtn.r = r;
  rtn.g = g;
  rtn.b = b;
  rtn.a = a;

  return rtn;
}