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

// コメント行
int main(int argc, char* argv[])
{
  /*  printf("hello %d\n", argc);
      printf("%s\n", argv[0]);*/

  char *filename = argv[1];
  FILE* fp = fopen(filename, "r");
  if (fp == NULL)
    error_at("fopen error");

  // 行頭が空白のコメント行
  if (fseek(fp, 0, SEEK_END) == -1) //行頭が空白でないコメント(カウントしない)
    error_at("seek end error."); /* 行頭が空白でないコメント(カウントしない) */
  size_t size = ftell(fp);
  if (size == -1)
    error_at("ftell error.");
  if (fseek(fp, 0, SEEK_SET) == -1)
    error_at("seek set error.");

  printf("size:%zd\n", size); /* 行頭が空白でないコメント(カウントしない)
                               範囲コメント内の改行の次行はコメント行にカウントする*/
  char *buf = calloc(1, size);
  fread(buf, size, 1, fp);

  int line = 0;
  for(size_t i=0; i<size; i++){
    printf("%c", buf[i]);
    if (buf[i] == '\n') line++;
  }

  free(buf);
  printf("line:%d\n", line);

  return 0;
}
