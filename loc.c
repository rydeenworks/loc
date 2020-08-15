#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void error_at(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, char* argv[])
{
  char *filename = argv[1];
  FILE* fp = fopen(filename, "r");
  if (fp == NULL)
    error_at("fopen error");

  if (fseek(fp, 0, SEEK_END) == -1)
    error_at("seek end error.");
  size_t size = ftell(fp);
  if (size == -1)
    error_at("ftell error.");
  if (fseek(fp, 0, SEEK_SET) == -1)
    error_at("seek set error.");

  printf("size:%zd\n", size);
  char *buf = calloc(1, size);
  fread(buf, size, 1, fp);

  int line = 0;
  int empty_line = 0;
  for(size_t i=0; i<size; i++){
    if (buf[i] == '\n') {
      line++;
      if (i < size - 2 && buf[i+1] == '\n') {
        empty_line++;
      }
    }
  }
  free(buf);

  printf("line:%d empty:%d\n", line, empty_line);
  return line;
}
