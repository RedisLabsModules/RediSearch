#include "forward_index.h"
#include "stopwords.h"
#include "tokenize.h"
#include "toksep.h"
#include "rmalloc.h"
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

typedef struct {
  RSTokenizer base;
  char **pos;
  Stemmer *stemmer;
} simpleTokenizer;

static void simpleTokenizer_Start(RSTokenizer *base, char *text, size_t len, uint32_t options) {
  simpleTokenizer *self = (simpleTokenizer *)base;
  TokenizerCtx *ctx = &base->ctx;
  ctx->text = text;
  ctx->options = options;
  ctx->len = len;
  self->pos = &ctx->text;
}

// Shortest word which can/should actually be stemmed
#define MIN_STEM_CANDIDATE_LEN 4

// Normalization buffer
#define MAX_NORMALIZE_SIZE 128

/**
 * Normalizes text.
 * - s contains the raw token
 * - dst is the destination buffer which contains the normalized text
 * - len on input contains the length of the raw token. on output contains the
 * on output contains the length of the normalized token
 */
static char *DefaultNormalize(char *s, char *dst, size_t *len) {
  size_t origLen = *len;
  char *realDest = s;
  size_t dstLen = 0;

#define SWITCH_DEST()        \
  if (realDest != dst) {     \
    realDest = dst;          \
    memcpy(realDest, s, ii); \
  }
  // set to 1 if the previous character was a backslash escape
  int escaped = 0;
  for (size_t ii = 0; ii < origLen; ++ii) {
    if (isupper(s[ii])) {
      SWITCH_DEST();
      realDest[dstLen++] = tolower(s[ii]);
    } else if ((isblank(s[ii]) && !escaped) || iscntrl(s[ii])) {
      SWITCH_DEST();
    } else if (s[ii] == '\\' && !escaped) {
      SWITCH_DEST();
      escaped = 1;
      continue;
    } else {
      dst[dstLen++] = s[ii];
    }
    escaped = 0;
  }

  *len = dstLen;
  return dst;
}

// tokenize the text in the context
uint32_t simpleTokenizer_Next(RSTokenizer *base, Token *t) {
  TokenizerCtx *ctx = &base->ctx;
  simpleTokenizer *self = (simpleTokenizer *)base;
  while (*self->pos != NULL) {
    // get the next token
    size_t origLen;
    char *tok = toksep(self->pos, &origLen);

    // normalize the token
    size_t normLen = origLen;

    char normalized_s[MAX_NORMALIZE_SIZE];
    char *normBuf;
    if (ctx->options & TOKENIZE_NOMODIFY) {
      normBuf = normalized_s;
      if (normLen > MAX_NORMALIZE_SIZE) {
        normLen = MAX_NORMALIZE_SIZE;
      }
    } else {
      normBuf = tok;
    }

    char *normalized = DefaultNormalize(tok, normBuf, &normLen);
    // ignore tokens that turn into nothing
    if (normalized == NULL || normLen == 0) {
      continue;
    }

    // skip stopwords
    if (StopWordList_Contains(ctx->stopwords, normalized, normLen)) {
      continue;
    }

    *t = (Token){.tok = normalized,
                 .tokLen = normLen,
                 .raw = tok,
                 .rawLen = origLen,
                 .pos = ++ctx->lastOffset,
                 .stem = NULL,
                 .flags = Token_CopyStem};

    // if we support stemming - try to stem the word
    if (!(ctx->options & TOKENIZE_NOSTEM) && self->stemmer && normLen >= MIN_STEM_CANDIDATE_LEN) {
      size_t sl;
      const char *stem = self->stemmer->Stem(self->stemmer->ctx, tok, normLen, &sl);
      if (stem) {
        t->stem = stem;
        t->stemLen = sl;
      }
    }
    return ctx->lastOffset;
  }

  return 0;
}

void simpleTokenizer_Free(RSTokenizer *self) {
  free(self);
}

static void doReset(RSTokenizer *tokbase, Stemmer *stemmer, StopWordList *stopwords,
                    uint32_t opts) {
  simpleTokenizer *t = (simpleTokenizer *)tokbase;
  t->stemmer = stemmer;
  t->base.ctx.stopwords = stopwords;
  t->base.ctx.options = opts;
  t->base.ctx.lastOffset = 0;
}

RSTokenizer *NewSimpleTokenizer(Stemmer *stemmer, StopWordList *stopwords, uint32_t opts) {
  simpleTokenizer *t = calloc(1, sizeof(*t));
  t->base.Free = simpleTokenizer_Free;
  t->base.Next = simpleTokenizer_Next;
  t->base.Start = simpleTokenizer_Start;
  t->base.Reset = doReset;
  t->base.Reset(&t->base, stemmer, stopwords, opts);
  return &t->base;
}

static mempool_t *tokpoolLatin_g = NULL;
static mempool_t *tokpoolCn_g = NULL;

static void *newLatinTokenizerAlloc() {
  return NewSimpleTokenizer(NULL, NULL, 0);
}
static void *newCnTokenizerAlloc() {
  return NewChineseTokenizer(NULL, NULL, 0);
}
static void tokenizerFree(void *p) {
  RSTokenizer *t = p;
  t->Free(t);
}

RSTokenizer *GetTokenizer(const char *language, Stemmer *stemmer, StopWordList *stopwords) {
  if (language && strcasecmp(language, "chinese") == 0) {
    return GetChineseTokenizer(stemmer, stopwords);
  } else {
    return GetSimpleTokenizer(stemmer, stopwords);
  }
}

RSTokenizer *GetChineseTokenizer(Stemmer *stemmer, StopWordList *stopwords) {
  if (!tokpoolCn_g) {
    tokpoolCn_g = mempool_new(16, newCnTokenizerAlloc, tokenizerFree);
  }

  RSTokenizer *t = mempool_get(tokpoolCn_g);
  t->Reset(t, stemmer, stopwords, 0);
  return t;
}

RSTokenizer *GetSimpleTokenizer(Stemmer *stemmer, StopWordList *stopwords) {
  if (!tokpoolLatin_g) {
    tokpoolLatin_g = mempool_new(16, newLatinTokenizerAlloc, tokenizerFree);
  }
  RSTokenizer *t = mempool_get(tokpoolLatin_g);
  t->Reset(t, stemmer, stopwords, 0);
  return t;
}

void Tokenizer_Release(RSTokenizer *t) {
  // In the future it would be nice to have an actual ID field or w/e, but for
  // now we can just compare callback pointers
  if (t->Next == simpleTokenizer_Next) {
    mempool_release(tokpoolLatin_g, t);
  } else {
    mempool_release(tokpoolCn_g, t);
  }
}