#include <plank/plank.h>

struct Actor;

struct ScriptManager
{
  size_t counter;
  vector(sstream *) *lines;
  vector(sstream *) *command;
  float wait;
  vector(struct Actor *) *actors;
  vector(struct Actor *) *doors;
};

