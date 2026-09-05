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
        bsq_validate(j.is_array(), "JSON -> BSQ", 0, nullptr, "Expected JSON array for ByteBuffer");

        size_t jlen = j.size();
        ByteBufferStreamingBuilder builder{};
        for(size_t i = 0; i < jlen; ++i)
        {
            const json& elem = j[i];
            bsq_validate(elem.is_number_unsigned(), "JSON -> BSQ", 0, nullptr, "Expected JSON number for ByteBuffer element");

            builder.appendByte(static_cast<uint8_t>(elem.get<uint64_t>()));
        }

        *((XByteBuffer*)resptr) = builder.finalize();
    }

    void parseToBSQ_ByteBuffer(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        bsq_validate(lexer->getCurrentTokenType() == BAPITokenType::LiteralByteBuffer, "BAPI -> BSQ", 0, nullptr, "Expected LiteralByteBuffer token");
     
        size_t tlen = lexer->getCurrentTokenDataSize();
        if(tlen == 4) {
            *((XByteBuffer*)resptr) = XByteBuffer{};
        }
        else {
            //eat 0x and (the [ gets handled in the loop)
            size_t cpos = 2; 
            BAPIIteratorAdaptor* ii = lexer->getCurrentTokenIterator();
            ii->advance();
            ii->advance();

            ByteBufferStreamingBuilder builder{};
            while(cpos < tlen - 1) { //ignore the closing ']' of the LiteralByteBuffer
                uint8_t bb = ii->get();
                bsq_validate(bb == ',' || bb == '[', "BAPI -> BSQ", 0, nullptr, "Expected ',' separator or '[' start in LiteralByteBuffer");
                ++cpos;
                ii->advance();
                    
                while(std::isspace(ii->get())) {
                    ++cpos;
                    ii->advance();
                }

                //read hex value
                char outbuff[16] = {0};
                size_t ecount = 0;
                while(std::isxdigit(ii->get())) {
                    outbuff[ecount] = ii->get();
                    ++ecount;
                    ++cpos;
                    ii->advance();
                }

                uint8_t output = 0;
                auto [ptr, ec] = std::from_chars(outbuff, outbuff + ecount, output, 16);

                bsq_validate(ec == std::errc() && (ptr == outbuff + ecount), "BAPI -> BSQ", 0, nullptr, "Failed to parse hex value for LiteralByteBuffer element");
                builder.appendByte(output);

                while(std::isspace(ii->get())) {
                    ++cpos;
                    ii->advance();
                }
            }

            *((XByteBuffer*)resptr) = builder.finalize();
        }

        lexer->consume();
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
