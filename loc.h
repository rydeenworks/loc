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

CommentToken *tokenize(char *buf, size_t size);
void count(CommentToken *tok, int *l, int *b, int *c);
