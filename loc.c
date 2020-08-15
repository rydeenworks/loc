#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// コメント解析トークン型
typedef enum {
              TK_NEW_LINE,      // 改行
              TK_LINE_COMMENT,  // １行コメント
              TK_START_COMMENT, // 範囲コメント開始
              TK_END_COMMENT,   // 範囲コメント終了
              TK_OTHRE,         // 上記以外
}TokenKind;

typedef struct CommentToken CommentToken;

struct CommentToken{
  TokenKind kind;
  CommentToken *next;
  CommentToken *prev;
};

CommentToken *new_token(TokenKind kind, CommentToken* cur) {
  CommentToken *tok = calloc(1, sizeof(CommentToken));
  tok->kind = kind;
  cur->next = tok;
  tok->prev = cur;
  return tok;
}


CommentToken *tokenize(char *buf, size_t size) {
  CommentToken head;
  head.kind = TK_NEW_LINE; //ファイル先頭は改行直接として処理する
  head.next = NULL;
  head.prev = NULL;
  CommentToken *cur = &head;

  char *c = buf;
  for (size_t i = 0; i < size; i++, c++) {
    switch(cur->kind) {
    case TK_NEW_LINE:
    case TK_END_COMMENT:
    case TK_OTHRE:
      // 空白を全て無視して処理しない('\n'除く)
      if(isspace(*c)) {
        // 改行の数でファイル全行数を計測する
        if (*c == '\n') {
          cur = new_token(TK_NEW_LINE, cur);
        } else {
          //nop
        }
      } else {
        if (strncmp("//", c, 2) == 0) {
          cur = new_token(TK_LINE_COMMENT, cur);
          c++; i++;
        } else if (strncmp("/*", c, 2) == 0) {
          cur = new_token(TK_START_COMMENT, cur);
          c++; i++;
        } else if (strncmp("*/", c, 2) == 0) {
          cur = new_token(TK_END_COMMENT, cur);
          c++; i++;
        } else {
          if (cur->kind != TK_OTHRE)
            cur = new_token(TK_OTHRE, cur);
        }
      }
      break;
    case TK_LINE_COMMENT:
    case TK_START_COMMENT:
      if (*c == '\n') {
        cur = new_token(TK_NEW_LINE, cur);
      } else if (strncmp("*/", c, 2) == 0 ) {
        cur = new_token(TK_END_COMMENT, cur);
        c++; i++;
      }
      break;
    }
  }

  head.next->prev = NULL;
  return head.next;
}

void count(CommentToken *tok, int *l, int *b, int *c) {
  // 空白のみの行は空行とする
  // 改行は'\n'
  // 1行コメント "//" (行頭の場合のみカウント)
  // 範囲コメント "/* ... */" (行頭の場合のみカウント)
  int lines = 1;
  int blank = 0;
  int comment = 0;
  printf("%d ", lines);
  bool is_comment_scope = false;
  for(CommentToken *cur=tok; cur->next != NULL; cur = cur->next ) {
    switch(cur->kind) {
    case TK_NEW_LINE:      // 改行直後
      // ファイル先頭で改行した場合は空行とする
      if (!cur->prev) {
        blank++;
        printf("b ");
      }
      printf("\n%d ", ++lines);
      // 範囲コメント内の改行はコメント行として加える
      if (is_comment_scope) {
        comment++;
        printf("c ");
      } else {
        // 改行後にすぐ改行した場合は空行とする
        if (!cur->next || cur->next->kind == TK_NEW_LINE) {
          blank++;
          printf("b ");
        }
      }
      break;
    case TK_LINE_COMMENT:  // １行コメント以降
      // 範囲コメント内の１行コメントは無視する
      if (!is_comment_scope && (!cur->prev || cur->prev->kind == TK_NEW_LINE)) {
        printf("c ");
        printf("//\t");
        comment++;
      }
      break;
    case TK_START_COMMENT: // 範囲コメント開始
      if (!is_comment_scope && (!cur->prev || cur->prev->kind == TK_NEW_LINE)) {
        printf("c ");
        printf("/*\t");
        comment++;
      }
      is_comment_scope = true;
      break;
    case TK_END_COMMENT: // 範囲コメント終了
      printf("*/\t");
      is_comment_scope = false;
      break;
    case TK_OTHRE:         // 上記以外
      printf("...\t");
      break;
    }
  }
  printf("\n");

  *l = lines;
  *b = blank;
  *c = comment;
}


void error_at(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

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

  CommentToken *tok = tokenize(buf, size);
  count(tok, &line, &blank_line, &comment_line);

  free(buf);

  int code = line - blank_line - comment_line;
  int ret = (blank_line << 5) + (comment_line << 3) + code;
  printf("blank:%d comment:%d code:%d ret:%d\n", blank_line, comment_line, code, ret);
  return ret;
}
