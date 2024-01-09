#pragma once

////////////////////////////
//List Macros

#define GET_TYPE_FOR_LIST_OF_T(T) struct BSQON_AST_##T##_List

#define BSQON_AST_LIST_DECLARE(LTYPE) \
struct GET_TYPE_FOR_LIST_OF_T(T) \
{ \
    T* value; \
    GET_TYPE_FOR_LIST_OF_T(T)* next; \
};

////////////////////////////
//Type Macros

#define GET_TYPE_NODE_FOR(T) struct BSQON_TYPE_AST_##T

#define BSQON_AST_TYPE_NODE_DECLARE_1(T, FTYPE1, FNAME1) \
struct GET_TYPE_NODE_FOR(T) { \
    struct BSQON_TYPE_AST_Node base; \
    FTYPE1 FNAME1; \
}; \
GET_TYPE_NODE_FOR(T)* BSQON_AST_as##T(const struct BSQON_TYPE_AST_Node* node); \
struct BSQON_TYPE_AST_Node* BSQON_AST_##T##Create(FTYPE1, FNAME1); \
void BSQON_AST_TYPE_print##T(struct BSQON_TYPE_AST_NominalNode* node);


#define BSQON_AST_TYPE_NODE_OPS_DECLARE(T) \

////////////////////////////
//Value Macros