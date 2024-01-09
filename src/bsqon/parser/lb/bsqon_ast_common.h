#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#pragma once

////////////////////////////
//ALLOCATION Macros

#define AST_ALLOC_ALIGN_8(size) (((size) + 7) & ~7)
#define AST_ALLOC(size) malloc(AST_ALLOC_ALIGN_8(size))

#define AST_STRDUP(str) strdup(str)

typedef struct AST_SourcePos
{
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} AST_SourcePos;

AST_SourcePos createSourcePos(uint32_t first_line, uint32_t first_column, uint32_t last_line, uint32_t last_column);

////////////////////////////
//List Macros

#define BSQON_LIST(T) BSQON_LIST_OF_##T
#define BSQON_NLIST(T) BSQON_NLIST_OF_##T

#define BSQON_AST_LIST_DECLARE(T) \
typedef struct BSQON_LIST(T) \
{ \
    T* value; \
    BSQON_LIST(T)* next; \
} BSQON_LIST(T);

#define BSQON_LIST_Empty(T) NULL
#define BSQON_LIST_Singleton(T, V) memcpy(AST_ALLOC(sizeof(BSQON_LIST(T))), BSQON_LIST(T) { .value = V, .next = NULL }, sizeof(BSQON_LIST(T)))
#define BSQON_LIST_Push(T, L, V) memcpy(AST_ALLOC(sizeof(BSQON_LIST(T))), BSQON_LIST(T) { .value = V, .next = L }, sizeof(BSQON_LIST(T)))
#define BSQON_LIST_Reverse(T, L) BAD

#define BSQON_AST_NLIST_DECLARE(T) \
typedef struct BSQON_NLIST(T) \
{ \
    const char* name; \
    T* value; \
    BSQON_NLIST(T)* next; \
} BSQON_NLIST(T);

#define BSQON_NLIST_Empty(T) NULL
#define BSQON_NLIST_Singleton(T, N, V) memcpy(AST_ALLOC(sizeof(BSQON_NLIST(T))), BSQON_NLIST(T) { .name = N, .value = V, .next = NULL }, sizeof(BSQON_NLIST(T)))
#define BSQON_NLIST_Push(T, L, N, V) memcpy(AST_ALLOC(sizeof(BSQON_NLIST(T))), BSQON_NLIST(T) { .name = N, .value = V, .next = L }, sizeof(BSQON_NLIST(T)))
#define BSQON_NLIST_Reverse(T, L) BAD

////////////////////////////
//AST Macros

#define BSQON_AST_NODE(T) BSQON_AST_NODE_##T

#define BSQON_AST_NODE_DECLARE_0(T) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl); \
void BSQON_AST_##T##Print(const BSQON_AST_NODE(T)* node);

#define BSQON_AST_NODE_DECLARE_1(T, FTYPE1, FNAME1) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1); \
void BSQON_AST_##T##Print(const BSQON_AST_NODE(T)* node);

#define BSQON_AST_NODE_DECLARE_2(T, FTYPE1, FNAME1, FTYPE2, FNAME2) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
    FTYPE2 FNAME2; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2); \
void BSQON_AST_##T##Print(const BSQON_AST_NODE(T)* node);

#define BSQON_AST_NODE_DECLARE_3(T, FTYPE1, FNAME1, FTYPE2, FNAME2, FTYPE3, FNAME3) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
    FTYPE2 FNAME2; \
    FTYPE3 FNAME3; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2, FTYPE3 FNAME3); \
void BSQON_AST_##T##Print(const BSQON_AST_NODE(T)* node);
