#include "bytebuff.h"
#include "../runtime/utils/lexer.h"
#include "../runtime/utils/builder.h"

namespace ᐸRuntimeᐳ
{
    thread_local GCAllocator<ByteBufferEntry> ByteBufferEntry_allocator(&g_typeinfo_ByteBufferEntry);
    thread_local GCAllocator<ByteBufferBlock> ByteBufferBlock_allocator(&g_typeinfo_ByteBufferBlock);

    const TypeInfo* XByteBuffer::s_entrytypeinfo = &g_typeinfo_ByteBufferEntry;
    thread_local GCAllocator<ByteBufferEntry>* XByteBuffer::s_entryallocator = &ByteBufferEntry_allocator;
    const TypeInfo* XByteBuffer::s_blocktypeinfo = &g_typeinfo_ByteBufferBlock;
    thread_local GCAllocator<ByteBufferBlock>* XByteBuffer::s_blockallocator = &ByteBufferBlock_allocator;

    void jsonParseToBSQ_ByteBuffer(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        xxxx;
    }

    void parseToBSQ_ByteBuffer(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        xxxx;
    }

    json bsqToJSON_ByteBuffer(const TypeInfo* tinfo, const void* valptr)
    {
        xxxx;
    }

    void bsqToBAPI_ByteBuffer(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        xxxx;
    }

    void displayValue_ByteBuffer(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        xxxx;
    }
}
