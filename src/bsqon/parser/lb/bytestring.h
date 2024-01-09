#pragma once

#include "bsqon_ast_common.h"

#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct ByteString
{
    uint8_t* bytes;
    uint64_t len;
} ByteString;

ByteString* bstrAlloc(ByteString dst);

#ifdef __cplusplus
}
#endif
