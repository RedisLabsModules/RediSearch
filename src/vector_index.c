#include "vector_index.h"
#include "list_reader.h"

#define POC_INITIAL_CAPACITY 100000
#define POC_EF 200
#define POC_M 16

#define POC_ALGORITHM VecSimAlgo_HNSW
#define POC_METRIC VecSimMetric_L2
#define POC_TYPE VecSimVecType_INT32
#define POC_VECTOR_LEN 2


static RS_Vector *openVectorKeysDict(RedisSearchCtx *ctx, RedisModuleString *keyName,
                                             int write) {
  KeysDictValue *kdv = dictFetchValue(ctx->spec->keysDict, keyName);
  if (kdv) {
    return kdv->p;
  }
  if (!write) {
    return NULL;
  }
  kdv = rm_calloc(1, sizeof(*kdv));
  kdv->dtor = (void (*)(void *))VecSimIndex_Free;
  // TODO: get good values from Dvir
  VecSimAlgoParams params = { .algorithmType = POC_ALGORITHM,
                              .hnswParams.efConstuction = POC_EF,
                              .hnswParams.initialCapacity = POC_INITIAL_CAPACITY,
                              .hnswParams.M = POC_M };
  kdv->p = VecSimIndex_New(&params, POC_METRIC, POC_TYPE, POC_VECTOR_LEN);
  dictAdd(ctx->spec->keysDict, keyName, kdv);
  return kdv->p;
}

RS_Vector *OpenVectorIndex(RedisSearchCtx *ctx,
                            RedisModuleString *keyName) {
  return openVectorKeysDict(ctx, keyName, 1);
}

IndexIterator *NewVectorIterator(RedisSearchCtx *ctx, VectorFilter *vf) {
  VecSimQueryResult *result;
  // TODO: change Dict to hold strings
  RedisModuleString *key = RedisModule_CreateStringPrintf(ctx->redisCtx, "%s", vf->property);
  RS_Vector *vecsim = openVectorKeysDict(ctx, key, 0);
  RedisModule_FreeString(ctx->redisCtx, key);
  switch (vf->type) {
    case VECTOR_TOPK:
      vf->vector = VecSimIndex_TopKQuery(vecsim->data, vf->vector, 100);
      break;
    case VECTOR_RANGE:
      RS_LOG_ASSERT(0, "Range is not supported yet");
      break;
  }
  // TODO: len
  return NewListIterator(vf->vector, 100);
}

/* Create a vector filter from parsed strings and numbers */
// TODO: add property?
VectorFilter *NewVectorFilter(void *vector, char *type, double value) {
  VectorFilter *vf = rm_malloc(sizeof(*vf));
  vf->vector = vector;
  vf->value = value;
  if (!strncmp(type, "TOPK", strlen("TOPK"))) {
    vf->type = VECTOR_TOPK;
  } else if (!strncmp(type, "RANGE", strlen("RANGE"))) {
    vf->type = VECTOR_RANGE;
  } else {
    RS_LOG_ASSERT(0, "Unsupported vector query type");
  }
  return vf;
}

void VectorFilter_Free(VectorFilter *vf) {
  if (vf->property) rm_free((char *)vf->property);
  if (vf->vector) rm_free(vf->vector);
  rm_free(vf);
}