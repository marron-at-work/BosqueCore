#pragma once

#include "bytestring.h"
#include "bsqon_ast_common.h"

#ifdef __cplusplus
extern "C" 
{
#endif

typedef enum BSQON_AST_TAG
{
    BSQON_AST_TAG_Error = 1,

    BSQON_AST_TAG_NominalType,
    BSQON_AST_TAG_NominalScopedType,
    BSQON_AST_TAG_TupleType,
    BSQON_AST_TAG_RecordType,
    BSQON_AST_TAG_ConjunctionType,
    BSQON_AST_TAG_UnionType,

    BSQON_AST_TAG_NoneValue,
    BSQON_AST_TAG_NothingValue,
    BSQON_AST_TAG_TrueValue,
    BSQON_AST_TAG_FalseValue,
    BSQON_AST_TAG_NatValue,
    BSQON_AST_TAG_IntValue,
    BSQON_AST_TAG_BigNatValue,
    BSQON_AST_TAG_BigIntValue,
    BSQON_AST_TAG_RationalValue,
    BSQON_AST_TAG_FloatValue,
    BSQON_AST_TAG_DecimalValue,
    BSQON_AST_TAG_DecimalDegreeValue,
    BSQON_AST_TAG_LatLongValue,
    BSQON_AST_TAG_ComplexValue,
    BSQON_AST_TAG_NumberinoValue,
    BSQON_AST_TAG_ByteBufferValue,
    BSQON_AST_TAG_UUIDv4Value,
    BSQON_AST_TAG_UUIDv7Value,
    BSQON_AST_TAG_SHAHashcodeValue,
    BSQON_AST_TAG_StringValue,
    BSQON_AST_TAG_ASCIIStringValue,
    BSQON_AST_TAG_NakedPathValue,
    BSQON_AST_TAG_RegexValue,
    BSQON_AST_TAG_DateTimeValue,
    BSQON_AST_TAG_UTCDateTimeValue,
    BSQON_AST_TAG_PlainDateValue,
    BSQON_AST_TAG_PlainTimeValue,
    BSQON_AST_TAG_LogicalTimeValue,
    BSQON_AST_TAG_TickTimeValue,
    BSQON_AST_TAG_TimestampValue,

    BSQON_AST_TAG_IdentifierValue,
    BSQON_AST_TAG_UnspecIdentifierValue,
    
    BSQON_AST_TAG_StringOfValue,
    BSQON_AST_TAG_ASCIIStringOfValue,
    BSQON_AST_TAG_PathValue,
    BSQON_AST_TAG_TypedLiteralValue,

    BSQON_AST_TAG_MapEntryValue,
    BSQON_AST_TAG_BracketValue,
    BSQON_AST_TAG_BraceValue,
    BSQON_AST_TAG_TypedValue,

    BSQON_AST_TAG_SomethingConsValue,
    BSQON_AST_TAG_OkConsValue,
    BSQON_AST_TAG_ErrConsValue,

    BSQON_AST_TAG_LetInValue,

    BSQON_AST_TAG_ScopedNameValue
} BSQON_AST_TAG;

typedef struct BSQON_AST_Node
{
    enum BSQON_AST_TAG tag;
    struct AST_SourcePos pos;
} BSQON_AST_Node;

BSQON_AST_NODE_DECLARE_0(ErrorNode)

BSQON_AST_LIST_DECLARE(BSQON_AST_Node)
BSQON_AST_NLIST_DECLARE(BSQON_AST_Node)

////////
//Type nodes
typedef BSQON_LIST(BSQON_AST_Node) BSQON_AST_LIST_OF_TYPES;
#define BSQON_AST_LIST_OF_TYPES_Empty BSQON_LIST_Empty(BSQON_AST_Node)
#define BSQON_AST_LIST_OF_TYPES_Singleton(V) BSQON_LIST_Singleton(BSQON_AST_Node, V)
#define BSQON_AST_LIST_OF_TYPES_Push(V, L) BSQON_LIST_Push(BSQON_AST_Node, V, L)
#define BSQON_AST_LIST_OF_TYPES_Reverse(L) BSQON_LIST_Reverse(BSQON_AST_Node, L)

typedef BSQON_NLIST_ENTRY(BSQON_AST_Node) BSQON_AST_NLIST_OF_TYPES_ENTRY;
#define BSQON_AST_NLIST_OF_TYPES_ENTRY_Create(N, V) BSQON_NLIST_ENTRY_Create(BSQON_AST_Node, N, V)

typedef BSQON_NLIST(BSQON_AST_Node) BSQON_AST_NLIST_OF_TYPES;
#define BSQON_AST_NLIST_OF_TYPES_Empty BSQON_NLIST_Empty(BSQON_AST_Node)
#define BSQON_AST_NLIST_OF_TYPES_Singleton(V) BSQON_NLIST_Singleton(BSQON_AST_Node, V)
#define BSQON_AST_NLIST_OF_TYPES_Push(V, L) BSQON_NLIST_Push(BSQON_AST_Node, V, L)
#define BSQON_AST_NLIST_OF_TYPES_Reverse(L) BSQON_NLIST_Reverse(BSQON_AST_Node, L)

BSQON_AST_NODE_DECLARE_2(NominalType, const char*, name, BSQON_AST_LIST_OF_TYPES*, terms)
BSQON_AST_NODE_DECLARE_2(NominalScopedType, BSQON_AST_NODE(NominalType)*, root, const char*, ext)
BSQON_AST_NODE_DECLARE_1(TupleType, BSQON_AST_LIST_OF_TYPES*, types)
BSQON_AST_NODE_DECLARE_1(RecordType, BSQON_AST_NLIST_OF_TYPES*, entries)
BSQON_AST_NODE_DECLARE_2(ConjunctionType, BSQON_AST_Node*, left, BSQON_AST_Node*, right)
BSQON_AST_NODE_DECLARE_2(UnionType, BSQON_AST_Node*, left, BSQON_AST_Node*, right)

//Value Nodes
typedef BSQON_LIST(BSQON_AST_Node) BSQON_AST_LIST_OF_VALUES;
#define BSQON_AST_LIST_OF_VALUES_Empty BSQON_LIST_Empty(BSQON_AST_Node)
#define BSQON_AST_LIST_OF_VALUES_Singleton(V) BSQON_LIST_Singleton(BSQON_AST_Node, V)
#define BSQON_AST_LIST_OF_VALUES_Push(V, L) BSQON_LIST_Push(BSQON_AST_Node, V, L)
#define BSQON_AST_LIST_OF_VALUES_Reverse(L) BSQON_LIST_Reverse(BSQON_AST_Node, L)

typedef BSQON_NLIST_ENTRY(BSQON_AST_Node) BSQON_AST_NLIST_OF_VALUES_ENTRY;
#define BSQON_AST_NLIST_OF_VALUES_ENTRY_Create(N, V) BSQON_NLIST_ENTRY_Create(BSQON_AST_Node, N, V)

typedef BSQON_NLIST(BSQON_AST_Node) BSQON_AST_NLIST_OF_VALUES;
#define BSQON_AST_NLIST_OF_VALUES_Empty BSQON_NLIST_Empty(BSQON_AST_Node)
#define BSQON_AST_NLIST_OF_VALUES_Singleton(V) BSQON_NLIST_Singleton(BSQON_AST_Node, V)
#define BSQON_AST_NLIST_OF_VALUES_Push(V, L) BSQON_NLIST_Push(BSQON_AST_Node, V, L)
#define BSQON_AST_NLIST_OF_VALUES_Reverse(L) BSQON_NLIST_Reverse(BSQON_AST_Node, L)

BSQON_AST_NODE_DECLARE_0(SingletonValue)
BSQON_AST_NODE_DECLARE_1(LiteralStandardValue, const char*, data)
BSQON_AST_NODE_DECLARE_1(LiteralStringValue, ByteString*, data)

BSQON_AST_NODE_DECLARE_1(NameValue, const char*, data)
BSQON_AST_NODE_DECLARE_2(StringOfValue, ByteString*, data, BSQON_AST_Node*, type)
BSQON_AST_NODE_DECLARE_2(PathValue, BSQON_AST_Node*, data, BSQON_AST_Node*, type)
BSQON_AST_NODE_DECLARE_2(TypedLiteralValue, BSQON_AST_Node*, data, BSQON_AST_Node*, type)
BSQON_AST_NODE_DECLARE_2(MapEntryValue, BSQON_AST_Node*, key, BSQON_AST_Node*, value)
BSQON_AST_NODE_DECLARE_1(BracketValue, BSQON_AST_LIST_OF_VALUES*, values)
BSQON_AST_NODE_DECLARE_1(BraceValue, BSQON_AST_NLIST_OF_VALUES*, entries)
BSQON_AST_NODE_DECLARE_2(TypedValue, BSQON_AST_Node*, value, BSQON_AST_Node*, type)
BSQON_AST_NODE_DECLARE_2(SpecialConsValue, BSQON_AST_Node*, value, const char*, consname)

BSQON_AST_NODE_DECLARE_2(ScopedNameValue, BSQON_AST_Node*, root, const char*, identifier)
BSQON_AST_NODE_DECLARE_4(LetInValue, const char*, vname, BSQON_AST_Node*, vtype, BSQON_AST_Node*, value, BSQON_AST_Node*, exp)

BSQON_AST_TAG BSQON_AST_getTag(const BSQON_AST_Node* node);
const char* BSQON_AST_getTagName(const BSQON_AST_Node* node);

BSQON_AST_Node* BSQON_AST_parse_from_stdin();
BSQON_AST_Node* BSQON_AST_parse_from_file(const char* file);

size_t BSQON_AST_getErrorInfo(char** errorInfo);

#ifdef __cplusplus
}
#endif
