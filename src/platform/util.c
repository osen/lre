#include "util.h"
#include "h.out.h"

#include <string.h>

#define LINE_BUF 64

unsigned char *fgetall(sstream *path, size_t *len)
{
  struct HdFile *fp = NULL;
  unsigned char *rtn = NULL;

  fp = HdFileOpen(sstream_cstr(path));

  if(!fp) return NULL;

  rtn = HdFileRead(fp, len);

  return rtn;
}


int fgetline(struct HdFile *f, sstream *output)
{
  char *res = NULL;
  int hasContent = 0;

  sstream_clear(output);

  while(1)
  {
    char buf[LINE_BUF] = {0};
    int endl = 0;
    size_t i = 0;

    res = HdFileGets(f, buf, LINE_BUF - 1);
    if(!res) break;

    hasContent = 1;

    for(i = 0; i < strlen(res); i++)
    {
      if(res[i] == '\r')
      {
        res[i] = '\0';
      }
      else if(res[i] == '\n')
      {
        endl = 1;
        res[i] = '\0';
      }
    }

    sstream_push_cstr(output, res);
    if(endl) break;
  }

  //if(sstream_length(output) < 1)
  if(!hasContent)
  {
    return 0;
  }

  return 1;
}

void sstream_split(struct sstream *ss, char token,
  vector(struct sstream*) *out)
{
  size_t max = 0;
  size_t i = 0;
  struct sstream *curr = NULL;

  //vector_sstream_clear(out);

  if(max >= vector_size(out))
  {
    curr = sstream_new();
  }
  else
  {
    curr = vector_at(out, max);
    sstream_clear(curr);
  }

  for(i = 0; i < sstream_length(ss); i++)
  {
    if(sstream_at(ss, i) == token)
    {
      if(max >= vector_size(out))
      {
        vector_push_back(out, curr);
      }
      max++;
      if(max >= vector_size(out))
      {
        curr = sstream_new();
      }
      else
      {
        curr = vector_at(out, max);
        sstream_clear(curr);
      }
    }
    else
    {
      sstream_push_char(curr, sstream_at(ss, i));
    }
  }

  if(sstream_length(curr) > 0)
  {
    if(max >= vector_size(out))
    {
      vector_push_back(out, curr);
    }
    max++;
  }
  else
  {
    if(max >= vector_size(out))
    {
      sstream_delete(curr);
    }
  }

  while(vector_size(out) > max)
  {
    sstream_delete(vector_at(out, vector_size(out) - 1));
    vector_erase(out, vector_size(out) - 1);
  }
}

void vector_sstream_clear(vector(sstream *) *v)
{
  size_t i = 0;

  for(i = 0; i < vector_size(v); i++)
  {
    sstream_delete(vector_at(v, i));
  }

  vector_clear(v);
}

void sstream_splitwhitespace(sstream *ss, vector(sstream *) *out)
{
  size_t max = 0;
  size_t i = 0;
  struct sstream *curr = NULL;

  //vector_sstream_clear(out);

  if(max >= vector_size(out))
  {
    curr = sstream_new();
  }
  else
  {
    curr = vector_at(out, max);
    sstream_clear(curr);
  }

  for(i = 0; i < sstream_length(ss); i++)
  {
    if(sstream_at(ss, i) == ' ' ||
      sstream_at(ss, i) == '\t')
    {
      if(sstream_length(curr) > 0)
      {
        if(max >= vector_size(out))
        {
          vector_push_back(out, curr);
        }
        max++;
        if(max >= vector_size(out))
        {
          curr = sstream_new();
        }
        else
        {
          curr = vector_at(out, max);
          sstream_clear(curr);
        }
      }
    }
    else
    {
      sstream_push_char(curr, sstream_at(ss, i));
    }
  }

  if(sstream_length(curr) > 0)
  {
    if(max >= vector_size(out))
    {
      vector_push_back(out, curr);
    }
    max++;
  }
  else
  {
    if(max >= vector_size(out))
    {
      sstream_delete(curr);
    }
  }

  while(vector_size(out) > max)
  {
    sstream_delete(vector_at(out, vector_size(out) - 1));
    vector_erase(out, vector_size(out) - 1);
  }
}

void sstream_cropbeforelast(sstream *ss, char c)
{
  sstream *tmp = NULL;
  size_t i = 0;
  size_t lastToken = 0;
  int found = 0;

  for(i = 0; i < sstream_length(ss); i++)
  {
    if(sstream_at(ss, i) == c)
    {
      lastToken = i;
      found = 1;
    }
  }

  if(!found)
  {
    return;
  }

  tmp = sstream_new();

  for(i = 0; i < lastToken; i++)
  {
    sstream_push_char(tmp, sstream_at(ss, i));
  }

  sstream_clear(ss);
  sstream_push_cstr(ss, sstream_cstr(tmp));
  sstream_delete(tmp);
}

