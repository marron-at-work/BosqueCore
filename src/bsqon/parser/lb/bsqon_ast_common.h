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

#define BSQON_NLIST_ENTRY(T) BSQON_NLIST_ENTRY_OF_##T
#define BSQON_NLIST(T) BSQON_NLIST_OF_##T

#define BSQON_AST_LIST_DECLARE(T) \
typedef struct BSQON_LIST(T) \
{ \
    T* value; \
    BSQON_LIST(T)* next; \
} BSQON_LIST(T); \
BSQON_LIST(T)* BSQON_LIST_##T##Reverse(BSQON_LIST(T)* ll); \
void BSQON_LIST_##T##Print(BSQON_LIST(T)* ll);

#define BSQON_LIST_Empty(T) NULL
#define BSQON_LIST_Singleton(T, V) memcpy(AST_ALLOC(sizeof(BSQON_LIST(T))), BSQON_LIST(T) { .value = V, .next = NULL }, sizeof(BSQON_LIST(T)))
#define BSQON_LIST_Push(T, L, V) memcpy(AST_ALLOC(sizeof(BSQON_LIST(T))), BSQON_LIST(T) { .value = V, .next = L }, sizeof(BSQON_LIST(T)))
#define BSQON_LIST_Reverse(T, L) BSQON_LIST_##T##Reverse(L)
#define BSQON_LIST_Print(T, L) BSQON_LIST_##T##Print(L)

#define BSQON_AST_NLIST_DECLARE(T) \
typedef struct BSQON_NLIST_ENTRY(T) \
{ \
    const char* name; \
    T value; \
} BSQON_NLIST_ENTRY(T); \
typedef struct BSQON_NLIST(T) \
{ \
    BSQON_NLIST_ENTRY(T) entry; \
    BSQON_NLIST(T)* next; \
} BSQON_NLIST(T); \
BSQON_NLIST(T)* BSQON_NLIST_##T##Reverse(BSQON_NLIST(T)* ll); \
void BSQON_NLIST_##T##Print(BSQON_NLIST(T)* ll);

#define BSQON_NLIST_ENTRY_Create(T, N, V) (BSQON_NLIST_ENTRY(T) { .name = N, .value = V })

#define BSQON_NLIST_Empty(T) NULL
#define BSQON_NLIST_Singleton(T, E) memcpy(AST_ALLOC(sizeof(BSQON_NLIST(T))), BSQON_NLIST(T) { .entry = E, .next = NULL }, sizeof(BSQON_NLIST(T)))
#define BSQON_NLIST_Push(T, L, E) memcpy(AST_ALLOC(sizeof(BSQON_NLIST(T))), BSQON_NLIST(T) { .entry = E, .next = L }, sizeof(BSQON_NLIST(T)))
#define BSQON_NLIST_Reverse(T, L) BSQON_NLIST_##T##Reverse(L)
#define BSQON_NLIST_Print(T, L) BSQON_NLIST_##T##Print(L)

////////////////////////////
//AST Declaration Macros

#define BSQON_AST_NODE(T) BSQON_AST_NODE_##T

#define BSQON_AST_NODE_DECLARE_0(T) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl);

#define BSQON_AST_NODE_DECLARE_1(T, FTYPE1, FNAME1) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1);

#define BSQON_AST_NODE_DECLARE_2(T, FTYPE1, FNAME1, FTYPE2, FNAME2) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
    FTYPE2 FNAME2; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2);

#define BSQON_AST_NODE_DECLARE_3(T, FTYPE1, FNAME1, FTYPE2, FNAME2, FTYPE3, FNAME3) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
    FTYPE2 FNAME2; \
    FTYPE3 FNAME3; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2, FTYPE3 FNAME3);

#define BSQON_AST_NODE_DECLARE_4(T, FTYPE1, FNAME1, FTYPE2, FNAME2, FTYPE3, FNAME3, FTYPE4, FNAME4) \
typedef struct BSQON_AST_NODE(T) { \
    BSQON_AST_Node base; \
    FTYPE1 FNAME1; \
    FTYPE2 FNAME2; \
    FTYPE3 FNAME3; \
    FTYPE4 FNAME4; \
} BSQON_AST_NODE(T); \
const BSQON_AST_NODE(T)* BSQON_AST##T##As(const BSQON_AST_Node* node); \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2, FTYPE3 FNAME3, FTYPE4 FNAME4);

////////////////////////////
//AST Definition Macros

#define BSQON_AST_NODE_AS(T, N) BSQON_AST##T##As(N)
#define BSQON_AST_NODE_CONS(T, TAG, SL, ...) BSQON_AST_##T##Create(TAG, SL, __VA_ARGS__)
#define BSQON_AST_NODE_PRINT(T, N) BSQON_AST_##T##Print(BSQON_AST_NODE_AS(T N))

#define BSQON_AST_LIST_DEFINE(T) \
BSQON_LIST(T)* BSQON_LIST_##T##Reverse(BSQON_LIST(T)* ll) \
{ \
    BSQON_LIST(T)* lp = NULL; \
    while(ll != NULL) { \
        BSQON_LIST(T)* lc = ll; \
        ll = ll->next; \
\
        lc->next = lp; \
        lp = lc; \
    } \
\
    return lp; \
}\
void BSQON_LIST_##T##Print(BSQON_LIST(T)* list) \
{ \
    for(BSQON_LIST(T)* ll = list; ll != NULL; ll = ll->next) \
    { \
        BSQON_AST_print(ll->value); \
        if(ll->next != NULL) { \
            printf(", "); \
        } \
    } \
}

#define BSQON_AST_NLIST_DEFINE(T) \
BSQON_NLIST(T)* BSQON_NLIST_##T##Reverse(BSQON_NLIST(T)* ll) \
{ \
    BSQON_NLIST(T)* lp = NULL; \
    while(ll != NULL) { \
        BSQON_NLIST(T)* lc = ll; \
        ll = ll->next; \
\
        lc->next = lp; \
        lp = lc; \
    } \
\
    return lp; \
} \
void BSQON_NLIST_##T##Print(BSQON_NLIST(T)* list) \
{ \
    for(BSQON_NLIST(T)* ll = list; ll != NULL; ll = ll->next) \
    { \
        printf("%s: ", ll->entry.name); \
        BSQON_AST_print(ll->entry.value); \
        if(ll->next != NULL) { \
            printf(", "); \
        } \
    } \
}

#define BSQON_AST_NODE_DEFINE_0(T) \
const BSQON_AST_NODE(T)* BSQON_AST_##T##As(const BSQON_AST_Node* node) { return (BSQON_AST_NODE(T)*)node; } \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl) \
{ \
    BSQON_AST_NODE(T)* node = (BSQON_AST_NODE(T)*)AST_ALLOC(sizeof(BSQON_AST_NODE(T))); \
    node->base.tag = tag; \
    node->base.pos = sl; \
    return (BSQON_AST_Node*)node; \
}

#define BSQON_AST_NODE_DEFINE_1(T, FTYPE1, FNAME1) \
const BSQON_AST_NODE(T)* BSQON_AST_##T##As(const BSQON_AST_Node* node) { return (BSQON_AST_NODE(T)*)node; } \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1) \
{ \
    BSQON_AST_NODE(T)* node = (BSQON_AST_NODE(T)*)AST_ALLOC(sizeof(BSQON_AST_NODE(T))); \
    node->base.tag = tag; \
    node->base.pos = sl; \
    node->FNAME1 = FNAME1; \
    return (BSQON_AST_Node*)node; \
}

#define BSQON_AST_NODE_DEFINE_2(T, FTYPE1, FNAME1, FTYPE2, FNAME2) \
const BSQON_AST_NODE(T)* BSQON_AST_##T##As(const BSQON_AST_Node* node) { return (BSQON_AST_NODE(T)*)node; } \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2) \
{ \
    BSQON_AST_NODE(T)* node = (BSQON_AST_NODE(T)*)AST_ALLOC(sizeof(BSQON_AST_NODE(T))); \
    node->base.tag = tag; \
    node->base.pos = sl; \
    node->FNAME1 = FNAME1; \
    node->FNAME2 = FNAME2; \
    return (BSQON_AST_Node*)node; \
}

#define BSQON_AST_NODE_DEFINE_3(T, FTYPE1, FNAME1, FTYPE2, FNAME2, FTYPE3, FNAME3) \
const BSQON_AST_NODE(T)* BSQON_AST_##T##As(const BSQON_AST_Node* node) { return (BSQON_AST_NODE(T)*)node; } \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2, FTYPE3 FNAME3) \
{ \
    BSQON_AST_NODE(T)* node = (BSQON_AST_NODE(T)*)AST_ALLOC(sizeof(BSQON_AST_NODE(T))); \
    node->base.tag = tag; \
    node->base.pos = sl; \
    node->FNAME1 = FNAME1; \
    node->FNAME2 = FNAME2; \
    node->FNAME3 = FNAME3; \
    return (BSQON_AST_Node*)node; \
}

#define BSQON_AST_NODE_DEFINE_4(T, FTYPE1, FNAME1, FTYPE2, FNAME2, FTYPE3, FNAME3, FTYPE4, FNAME4) \
const BSQON_AST_NODE(T)* BSQON_AST_##T##As(const BSQON_AST_Node* node) { return (BSQON_AST_NODE(T)*)node; } \
BSQON_AST_Node* BSQON_AST_##T##Create(BSQON_AST_TAG tag, AST_SourcePos sl, FTYPE1 FNAME1, FTYPE2 FNAME2, FTYPE3 FNAME3, FTYPE4 FNAME4) \
{ \
    BSQON_AST_NODE(T)* node = (BSQON_AST_NODE(T)*)AST_ALLOC(sizeof(BSQON_AST_NODE(T))); \
    node->base.tag = tag; \
    node->base.pos = sl; \
    node->FNAME1 = FNAME1; \
    node->FNAME2 = FNAME2; \
    node->FNAME3 = FNAME3; \
    node->FNAME4 = FNAME4; \
    return (BSQON_AST_Node*)node; \
}

