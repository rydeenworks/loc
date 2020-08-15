#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

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
  bool is_new_line = false; // 連続した改行を判定するためのフラグ
  for (size_t i = 0; i < size; i++) {
    // 空白のみの行は空行とする(行頭の空白は常に無視する)
    // 改行は'\n'
    // コメントのみの行をコメント行として計測する
    // 1行コメント "//"
    // 範囲コメント "/* ... */"
    if(isspace(buf[i])) {
      // ファイル全行数の計測
      if (buf[i] == '\n') {
        line++;
        if (is_new_line)
          empty_line++;
        is_new_line = true;
      }
      // 空白を全て無視して処理しない('\n'除く)
      continue;
    }
    is_new_line = false;
  }
  free(buf);

  printf("line:%d empty:%d\n", line, empty_line);
  return line;
}
