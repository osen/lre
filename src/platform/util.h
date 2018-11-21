#include "h.out.h"

#include <plank/plank.h>

#include <stdio.h>

int fgetline(struct HdFile *f, sstream *output);
unsigned char *fgetall(sstream *path, size_t *len);
void vector_sstream_clear(vector(sstream *) *v);
void sstream_split(sstream *s, char token, vector(sstream *) *out);
void sstream_splitwhitespace(sstream *ss, vector(sstream *) *out);
void sstream_cropbeforelast(sstream *ss, char c);
