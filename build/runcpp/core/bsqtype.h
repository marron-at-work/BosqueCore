#pragma once

#include "../common.h"
#include "lexer.h"
#include "serializer.h"

#define BSQ_PTR_MASK_LEAF nullptr

namespace ᐸRuntimeᐳ
{
    enum class RColor : uint16_t
    {
        Red,
        Black
    };
    
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_NONE = 0;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_BOOL = 1;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_INT = 2;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_NAT = 3;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CHKINT = 4;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CHKNAT = 5;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_FLOAT = 6;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_POSRB_TREE_LEAF_CSTRING = 7;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_POSRB_TREE_NODE_CSTRING = 8;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_POSRB_TREE_CSTRING = 9;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CSTRING_INLINE = 10;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CSTRING_TREE = 11;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CSTRING = 12;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_POSRB_TREE_LEAF_STRING = 13;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_POSRB_TREE_NODE_STRING = 14;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_POSRB_TREE_STRING = 15;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_STRING_INLINE = 16;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_STRING_TREE = 17;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_STRING = 18;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_BYTEBUFFERENTRY = 19;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_BYTEBUFFERBLOCK = 20;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_BYTEBUFFER = 21;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_BYTE = 22;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CCHAR = 23;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_UNICODECHAR = 24;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_UUIDV4 = 25;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_UUIDV7 = 26;

    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_CREGEX = 27;
    inline constexpr uint32_t WELL_KNOWN_TYPE_ID_REGEX = 28;

    enum class LayoutTag : uint16_t
    {
        Value,     //an inline value
        Ref        //a pointer to a heap allocated value
    };

    //Function pointer type for entrypoint constructors will run all needed validations on arguments (passed as pointer array of void* and write the constructed value to the second void* argument) -- this needs to handle long jump on error
    using ValidatingConstructorFp = void(*)(void**, void*); 

    //Function pointer type for JSON to BSQ conversion (takes the target typeinfo, a JSON object, and a pointer to the destination BSQ value) -- this needs to handle long jump on error
    using JSONParseToBSQFp = void(*)(const TypeInfo*, const json&, void*);

    //Function pointer type for parser to BSQ conversion (takes the target typeinfo, a parser object, and a pointer to the destination BSQ value) -- this needs to handle long jump on error
    using ParseToBSQFp = void(*)(const TypeInfo*, BAPILexer*, void*);

    //Function pointer type to convert a BSQ value to JSON (takes the source typeinfo, a pointer to the source BSQ value, and a JSON object to write to)
    using BSQToJSONFp = json(*)(const TypeInfo*, const void*);

    //Function pointer type to convert a BSQ value to BAPI (takes the source typeinfo, a pointer to the source BSQ value, and a out buffer (iobuffer or ByteBuffer writer) to write to)
    using BSQToBAPIFp = void(*)(const TypeInfo*, const void*, BSQSerializer*); 

    //Function pointer to write a value for display (diagnostics)
    using DisplayValueFp = void(*)(const TypeInfo*, const void*, std::ostream&, std::optional<std::string>); 

    class TypeOpDispatchInfo
    {
    public:
        ValidatingConstructorFp validatingConstructorFp;
        JSONParseToBSQFp jsonParseToBSQFp;
        ParseToBSQFp parseToBSQFp;
        BSQToJSONFp bsqToJSONFp;
        BSQToBAPIFp bsqToBAPIFp;
        DisplayValueFp displayFp;
    };

    class TypeLayoutInfo
    {
    public:
        uint32_t fieldid;
        uint32_t fieldbsqtypeid;
        uint32_t byteoffset;
        uint32_t slotoffset;

        const char* fieldkey;
        const char* fname;
    };

    using VInvokePtr = void(*)(void);
    class VInvokeTargetInfo
    {
    public:
        uint32_t invokeid;
        VInvokePtr invokeptr;

        const char* invokekey;
    };

    class TypeInfo
    {
    public:
        uint32_t bsqtypeid;
        uint32_t bytesize;
        uint32_t slotcount;
        LayoutTag tag;
        
        const char* ptrmask; // NULL is for leaf values or structs

        const uint32_t* supertypes;
        const uint32_t supertypescount;
        const TypeLayoutInfo* ftable;
        const uint32_t ftablecount;
        const VInvokeTargetInfo* vitable;
        const uint32_t vitablecount;
        const TypeOpDispatchInfo opdispatch;

        const char* typekey;

        bool quickrelease;

        //Way to get any typeinfo by its bsqtypeid -- map might be slower than desired (and not static initializable -- maybe evaluate later)
        // This map initialization needs to happen in emitter (otherwise linker error)
        static std::map<uint32_t, TypeInfo*> tinfomap;
    };

    consteval uint32_t byteSizeToSlotCount(size_t bytesize)
    {
        return bytesize / sizeof(uint64_t);
    }

    consteval uint32_t slotCountToByteSize(size_t slotcount)
    {
        return slotcount * sizeof(uint64_t);
    }
}
