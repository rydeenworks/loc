#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void error_at(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 32bitで 空行 コメント行 コード行 を表現する
// 表現可能な行数は以下の通り
// blank  : 9bit(0-515)
// comment: 9bit(0-515)
// code   :14bit(0-16383)
// 21098765432109876543210987654321
// [  blank][comment][        code]

// 8bitで 空行 コメント行 コード行 を表現する(シェルの$?で取得するため)
// 表現可能な行数は以下の通り
// blank  : 2bit(0-3)
// comment: 3bit(0-7)
// Code   : 3bit(0-7)
// 87654321
// bbcccCCC


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

  char *buf = calloc(1, size);
  fread(buf, size, 1, fp);

  int line = 0;
  int blank_line = 0;
  int comment_line = 0;
  bool is_file_top = true; // ファイル先頭の場合true 改行または空白以外の出現以降はfalse
  bool is_new_line = false; // 改行直後はtrue 改行後に空白以外の出現以降はfalse(連続した改行を判定するためのフラグ)
  bool is_comment_scope = false; // 範囲コメントのスコープ内の場合はtrue
  char *c = buf;
  for (size_t i = 0; i < size; i++, c++) {
    // 空白のみの行は空行とする
    // 行頭の空白は常に無視する
    // 改行は'\n'
    // 1行コメント "//" (行頭が１行コメントの場合のみカウント)
    // 範囲コメント "/* ... */" (1行目は行頭が範囲コメントの場合のみカウント)
    // 範囲コメント内の１行コメントは無視する
    // １行コメント後の範囲コメントは無視する

    // 空白を全て無視して処理しない('\n'除く)
    if(isspace(*c)) {
      // ファイル全行数の計測
      if (*c == '\n') {
        line++;
        // 範囲コメント内の改行はコメント行として加える
        if (is_comment_scope)
          comment_line++;
        // 改行後にすぐ改行した場合は空行とする
        if (is_new_line)
          blank_line++;
        is_new_line = true;
      }
    } else {
      if (strncmp("//", c, 2) == 0) {
        if( is_file_top || is_new_line)
          comment_line++;
      } else if (strncmp("/*", c, 2) == 0) {
        if( is_file_top || is_new_line)
          comment_line++;
        is_comment_scope = true;
      } else if (strncmp("*/", c, 2) == 0 ) {
        is_comment_scope = false;
      }
      is_new_line = false;
    }
  }
  free(buf);

  int code = line - blank_line - comment_line;
  int ret = (blank_line << 5) + (comment_line << 3) + code;
  printf("blank:%d comment:%d code:%d ret:%d\n", blank_line, comment_line, code, ret);
  return ret;
}
