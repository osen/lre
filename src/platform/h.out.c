#include <stdlib.h>
#include <string.h>
#include <stdio.h>

size_t horde_entries_count = 1;

char *horde_path_entries[] = {
  "__dummy",
};
size_t horde_size_entries[] = {
  1,
};
unsigned char horde_data[] = {
0x00,
};

struct HdFile { size_t begin; size_t end; unsigned char *data; size_t size; size_t current; FILE *fp; }; struct HdFile *HdFileOpen(char *path) { size_t i = 0; struct HdFile *rtn = NULL; size_t start = 0; FILE *fp = fopen(path, "rb"); if(fp) { rtn = calloc(1, sizeof(*rtn)); rtn->fp = fp; return rtn; } for(i = 0; i < horde_entries_count; i++) { if(strcmp(path, horde_path_entries[i]) == 0) { rtn = calloc(1, sizeof(*rtn)); rtn->begin = start; rtn->data = horde_data; rtn->size = horde_size_entries[i]; rtn->current = rtn->begin; rtn->end = rtn->begin + rtn->size; return rtn; } start += horde_size_entries[i]; } return NULL; } char *HdFileGets(struct HdFile *ctx, char *buffer, size_t len) { unsigned char *c = NULL; size_t i = 0; char *rtn = NULL; if(ctx->fp) { return fgets(buffer, len, ctx->fp); } while(1) { char t = '\0'; if(i >= len) return rtn; if(ctx->current >= ctx->end) return rtn; c = &ctx->data[ctx->current]; ctx->current++; t = *c; buffer[i] = t; rtn = buffer; if(t == '\n') { return rtn; } i++; } return rtn; } unsigned char *HdFileRead(struct HdFile *ctx, size_t *len) { unsigned char *rtn = NULL; size_t curr = 0; size_t i = 0; if(ctx->fp) { long fsize = 0; fseek(ctx->fp, 0, SEEK_END); fsize = ftell(ctx->fp); fseek(ctx->fp, 0, SEEK_SET); rtn = malloc(fsize + 1); fread(rtn, fsize, 1, ctx->fp); *len = fsize; return rtn; } rtn = malloc(ctx->size); *len = ctx->size; curr = ctx->begin; while(1) { if(curr >= ctx->end) break; rtn[i] = ctx->data[curr]; i++; curr++; } return rtn; } void HdFileClose(struct HdFile *ctx) { if(ctx->fp) { fclose(ctx->fp); } free(ctx); }

