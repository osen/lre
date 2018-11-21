#include "payload.h"
#include "dirent_wrapper.h"

#include <plank/plank.h>

#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 1024

struct Entry
{
  sstream *path;
  vector(unsigned char) *data;
  size_t size;
};

struct Entry *EntryCreate(sstream *path)
{
  struct Entry *rtn = NULL;
  FILE *fp = NULL;
  unsigned char buffer[BUFF_SIZE] = {0};

  rtn = palloc(struct Entry);
  rtn->path = sstream_copy(path);
  rtn->data = vector_new(unsigned char);

  if(strcmp(sstream_cstr(path), "__dummy") != 0)
  {
    fp = fopen(sstream_cstr(path), "rb");

    if(!fp)
    {
      printf("Failed to open: %s\n", sstream_cstr(path));
      abort();
    }

    while(1)
    {
      size_t br = 0;
      size_t bi = 0;

      br = fread(buffer, sizeof(unsigned char), BUFF_SIZE, fp);

      for(bi = 0; bi < br; bi++)
      {
        vector_push_back(rtn->data, buffer[bi]);
      }

      rtn->size += br;

      if(br != BUFF_SIZE)
      {
        //printf("File [%s] size: %i\n", sstream_cstr(path), (int)rtn->size);
        break;
      }
    }

    fclose(fp);
  }
  else
  {
    vector_push_back(rtn->data, 0);
    rtn->size += 1;
  }

  printf("Entry: %s [%i]\n", sstream_cstr(rtn->path), (int)rtn->size);

  return rtn;
}

void EntryDestroy(struct Entry *ctx)
{
  sstream_delete(ctx->path);
  vector_delete(ctx->data);
  pfree(ctx);
}

vector(struct Entry *) *entries;

int is_dir(char *path)
{
  DIR *d = NULL;

  d = opendir(path);

  if(!d) return 0;

  closedir(d);

  return 1;
}

void scan_dir(char *path)
{
  DIR *dp = NULL;
  struct dirent *ep = NULL;     

  dp = opendir(path);

  if(!dp)
  {
    printf("Failed to open: %s\n", path);
    abort();
  }

  while(1)
  {
    sstream *fullPath = NULL;
    int isDir = 0;

    ep = readdir(dp);

    if(!ep) break;
    if(strcmp(ep->d_name, ".") == 0) continue;
    if(strcmp(ep->d_name, "..") == 0) continue;

/*
    if(ep->d_type != DT_DIR &&
      ep->d_type != DT_REG)
    {
      continue;
    }
*/

    fullPath = sstream_new();
    sstream_push_cstr(fullPath, path);
    sstream_push_char(fullPath, '/');
    sstream_push_cstr(fullPath, ep->d_name);

    isDir = is_dir(sstream_cstr(fullPath));

    if(isDir == 1)
    {
      //printf("Directory: %s\n", sstream_cstr(fullPath));
      scan_dir(sstream_cstr(fullPath));
    }
    else
    {
      struct Entry *entry = NULL;

      entry = EntryCreate(fullPath);
      vector_push_back(entries, entry);
    }

    sstream_delete(fullPath);
  }

  closedir(dp);
}

const char *testProg = MULTILINE(
  int main()
  {
    struct HdFile *fp = NULL;

    fp = HdFileOpen("assets/rooms/entrance_hall_1.dat");

    if(!fp)
    {
      printf("Failed to open file\n");
      abort();
    }

    while(1)
    {
      char buff[100] = {0};

      if(!HdFileGets(fp, buff, 99))
      {
        break;
      }

      printf("Line: %s\n", buff);
    }

    HdFileClose(fp);

    return 0;
  }
);

void generate_h()
{
  FILE *fp = NULL;

  fp = fopen("src/platform/h.out.h", "w");

  if(!fp)
  {
    printf("Failed to open: %s\n", "h.out.h");
    abort();
  }

  fprintf(fp, "#include <stdlib.h>\n\n");
  fprintf(fp, "struct HdFile;\n");
  fprintf(fp, "struct HdFile *HdFileOpen(char *path);\n");
  fprintf(fp, "char *HdFileGets(struct HdFile *ctx, char *buffer, size_t len);\n");
  fprintf(fp, "unsigned char *HdFileRead(struct HdFile *ctx, size_t *len);\n");
  fprintf(fp, "void HdFileClose(struct HdFile *ctx);\n");

  fclose(fp);
}

void generate_c()
{
  FILE *fp = NULL;
  size_t i = 0;

  fp = fopen("src/platform/h.out.c", "w");

  if(!fp)
  {
    printf("Failed to open: %s\n", "h.out.c");
    abort();
  }

  fprintf(fp, "#include <stdlib.h>\n");
  fprintf(fp, "#include <string.h>\n");
  fprintf(fp, "#include <stdio.h>\n\n");

  fprintf(fp, "size_t horde_entries_count = %i;\n\n", (int)vector_size(entries));

  // Path

  fprintf(fp, "char *horde_path_entries[] = {\n");

  for(i = 0; i < vector_size(entries); i++)
  {
    struct Entry *entry = vector_at(entries, i);

    fprintf(fp, "  \"%s\",\n", sstream_cstr(entry->path));
  }

  fprintf(fp, "};\n");

  // Size

  fprintf(fp, "size_t horde_size_entries[] = {\n");

  for(i = 0; i < vector_size(entries); i++)
  {
    struct Entry *entry = vector_at(entries, i);

    fprintf(fp, "  %i,\n", (int)entry->size);
  }

  fprintf(fp, "};\n");

  // Data

  fprintf(fp, "unsigned char horde_data[] = {\n");

  for(i = 0; i < vector_size(entries); i++)
  {
    size_t di = 0;
    struct Entry *entry = vector_at(entries, i);

    for(di = 0; di < vector_size(entry->data); di++)
    {
      fprintf(fp, "0x%02x,", vector_at(entry->data, di));

      if(di % 10 == 0)
      {
        fprintf(fp, "\n");
      }
    }

  }

  fprintf(fp, "};\n\n");

  fprintf(fp, "%s\n\n", payload);

  //fprintf(fp, "%s\n", testProg);

  fclose(fp);
}

int main(int argc, char *argv[])
{
  size_t ei = 0;

  printf("Horde\n");

  if(argc < 2)
  {

  }

  entries = vector_new(struct Entry *);

  if(argc > 1)
  {
    scan_dir(argv[1]);
  }
  else
  {
    struct Entry *entry = NULL;
    sstream *dp = NULL;

    dp = sstream_new();
    sstream_push_cstr(dp, "__dummy");
    entry = EntryCreate(dp);
    vector_push_back(entries, entry);
    sstream_delete(dp);
  }

  generate_c();
  generate_h();

  for(ei = 0; ei < vector_size(entries); ei++)
  {
    EntryDestroy(vector_at(entries, ei));
  }

  vector_delete(entries);

  return 0;
}
