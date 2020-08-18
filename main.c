#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include "loc.h"

extern int debug_disp;

void error_at(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

char *file_read(char *filename, size_t *size) {
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

int main(int argc, char* argv[])
{
  char ch;
  debug_disp = 0;
  while ((ch = getopt(argc, argv, "dh")) != -1) {
    switch(ch) {
    case 'd': // detail or debug
      printf("d argc=%d optind=%d\n", argc, optind);
      debug_disp = 1;
      break;
    case '?':
    default:
      printf("Usage: %s [-d] [file]\n", argv[0]);
      printf("\t-d : detail print\n");
      printf("\toutput example:\n");
      printf("\tblank   comment code\n");
      printf("\t6       0       1\n");
      return 0;
    }
  }

  // オプションの次がファイル名
  char *filename = argv[optind];

  size_t size;
  char *buf = file_read(filename, &size);
  CommentToken *tok = tokenize(buf, size);
  free(buf);

  int line = 0;
  int blank = 0;
  int comment = 0;
  count(tok, &line, &blank, &comment);
  int code = line - blank - comment;
  if (debug_disp) {
    int pos = strlen(filename);
    printf("filenameblank,comment,code\n");
  }
  printf("%s,%d,%d,%d\n", filename, blank, comment, code);

  // 8bitで 空行 コメント行 コード行 を表現する(シェルの$?で取得するため)
  // 表現可能な行数は以下の通り
  // blank  : 2bit(0-3)
  // comment: 3bit(0-7)
  // Code   : 3bit(0-7)
  // 87654321
  // bbcccCCC
  int ret = (blank << 5) + (comment << 3) + code;
  if (debug_disp) {
    printf("blank:%d comment:%d code:%d ret:%d\n", blank, comment, code, ret);
  }
  return ret;
}
