#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "loc.h"

void error_at(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

char *read(char *filename, size_t *size) {
  FILE* fp = fopen(filename, "r");
  if (fp == NULL)
    error_at("fopen error");

  if (fseek(fp, 0, SEEK_END) == -1)
    error_at("seek end error.");
  *size = ftell(fp);
  if (*size == -1)
    error_at("ftell error.");
  if (fseek(fp, 0, SEEK_SET) == -1)
    error_at("seek set error.");

  char *buf = calloc(1, *size);
  fread(buf, *size, 1, fp);
  return buf;
}

void result_print(int blank, int comment, int code) {
  printf("blank\tcomment\tcode\n");
  printf("%d\t%d\t%d\n", blank, comment, code);
}

int main(int argc, char* argv[])
{
  char *filename = argv[1];

  size_t size;
  char *buf = read(filename, &size);
  CommentToken *tok = tokenize(buf, size);
  free(buf);

  int line = 0;
  int blank_line = 0;
  int comment_line = 0;
  count(tok, &line, &blank_line, &comment_line);

  int code = line - blank_line - comment_line;
  result_print(blank_line, comment_line, code);

  // 8bitで 空行 コメント行 コード行 を表現する(シェルの$?で取得するため)
  // 表現可能な行数は以下の通り
  // blank  : 2bit(0-3)
  // comment: 3bit(0-7)
  // Code   : 3bit(0-7)
  // 87654321
  // bbcccCCC
  int ret = (blank_line << 5) + (comment_line << 3) + code;
  printf("blank:%d comment:%d code:%d ret:%d\n", blank_line, comment_line, code, ret);
  return ret;
}
