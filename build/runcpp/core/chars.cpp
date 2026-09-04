#include "chars.h"

#include "../runtime/utils/encodings.h"
#include "../runtime/utils/lexer.h"
#include "../runtime/utils/builder.h"

namespace ᐸRuntimeᐳ 
{
    size_t writeByteValue(XByte val, std::array<char, 64>& numbuf)
    {
        return std::snprintf(numbuf.data(), numbuf.size(), "0x%x", val.value);
    }

    size_t writeCCharValue(XCChar val, std::array<char, 64>& numbuf)
    {
        if(!isMustEscapeCChar((char)val.value)) {
            return std::snprintf(numbuf.data(), numbuf.size(), "c'%c'", (char)val.value);            
        }
        else {
            auto ii = std::find_if(s_escape_names_char_simple.begin(), s_escape_names_char_simple.end(), [val](const std::pair<uint8_t, const char*>& p) { 
                return p.first == (uint8_t)val.value; 
            });
            
            if(ii != s_escape_names_char_simple.end()) {
                return std::snprintf(numbuf.data(), numbuf.size(), "c'%s'", ii->second);
            }
            else {
                return std::snprintf(numbuf.data(), numbuf.size(), "c'%%x%x;'", (uint8_t)val.value);
            }
        }
    }

    size_t writeUnicodeCharValue(XUnicodeChar val, std::array<char, 64>& numbuf)
    {
        if(!isMustEscapeUnicodeChar((char32_t)val.value)) {
            xxxx;            
        }
        else {
            xxxx;
        }
    }

    ///////////////////////////////
    //Byte
    ///////////////////////////////

    void jsonParseToBSQ_Byte(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        bsq_validate(j.is_number_unsigned(), "JSON -> BSQ", 0, nullptr, "Expected an unsigned number for Byte");
        
        uint64_t vv = j.get<uint64_t>();
        bsq_validate(vv <= std::numeric_limits<uint8_t>::max(), "JSON -> BSQ", 0, nullptr, "Value out of range for Byte");
        *(XByte*)resptr = XByte{vv};
    }

    void parseToBSQ_Byte(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        bsq_validate(lexer->getCurrentTokenType() == BAPITokenType::LiteralByte, "Parse -> BSQ", 0, nullptr, "Expected a literal byte token");

        std::array<uint8_t, 64> outchars;
        size_t size = lexer->extractSmallToken(outchars);

        uint64_t output = 0;
        auto [ptr, ec] = std::from_chars(reinterpret_cast<const char*>(outchars.data()) + 2, reinterpret_cast<const char*>(outchars.data()) + size, output, 16);
        bsq_validate(ec == std::errc() && ptr == reinterpret_cast<const char*>(outchars.data()) + size, "Parse -> BSQ", 0, nullptr, "Failed to parse Byte token");

        *(XByte*)resptr = XByte{output};
        lexer->consume();
    }

    json bsqToJSON_Byte(const TypeInfo* tinfo, const void* valptr)
    {
        XByte v = *(XByte*)valptr;
        return json(v.value);
    }

    void bsqToBAPI_Byte(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        XByte v = *(XByte*)valptr;
        std::array<char, 64> numbuf;
        size_t written = writeByteValue(v, numbuf);

        builder->appendConstString(numbuf.data(), written);
    }

    void displayValue_Byte(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        XByte v = *(XByte*)valptr;
        std::array<char, 64> numbuf;
        size_t written = writeByteValue(v, numbuf);

        os << getDisplayIndent(indent) << std::string(numbuf.data(), written);
    }

    ///////////////////////////////
    //CChar
    ///////////////////////////////

    void jsonParseToBSQ_CChar(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        bsq_validate(j.is_number_unsigned(), "JSON -> BSQ", 0, nullptr, "Expected an unsigned number for CChar");
        
        int64_t vv = j.get<int64_t>();
        bsq_validate(vv <= std::numeric_limits<uint8_t>::max() && isLegalCChar((uint8_t)vv), "JSON -> BSQ", 0, nullptr, "Value out of range for CChar");
        *(XCChar*)resptr = XCChar{vv};
    
    }

    void parseToBSQ_CChar(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        if(lexer->getCurrentTokenType() != BAPITokenType::LiteralCChar) {
            bsq_validate(lexer->allowSloppyStrings && lexer->getCurrentTokenType() == BAPITokenType::LiteralUnicodeChar, "Parse -> BSQ", 0, nullptr, "Expected a CChar token");
        }

        std::array<uint8_t, 64> outchars;
        size_t size = lexer->extractSmallToken(outchars);

        char output = 0;
        if(size == 4) {
            output = outchars[2]; //just a simple char c'x'
            bsq_validate(isLegalCChar(output), "Parse -> BSQ", 0, nullptr, "Invalid CChar literal");
        }
        else {
            bool charok = processEncodedCChar(outchars, size, output); //skip c' and '
            bsq_validate(charok, "Parse -> BSQ", 0, nullptr, "Invalid CChar literal");
        }

        *(XCChar*)resptr = XCChar{output};
        lexer->consume();
    }

    json bsqToJSON_CChar(const TypeInfo* tinfo, const void* valptr)
    {
        XCChar v = *(XCChar*)valptr;
        return json(v.value);
    }
    
    void bsqToBAPI_CChar(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        XCChar v = *(XCChar*)valptr;
        
        std::array<char, 64> numbuf;
        size_t written = writeCCharValue(v, numbuf);

        builder->appendConstString(numbuf.data(), written);
    
    }
    void displayValue_CChar(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        std::array<char, 64> numbuf;
        size_t written = writeCCharValue(*(XCChar*)valptr, numbuf);

        os << getDisplayIndent(indent) << std::string(numbuf.data(), written);
    }
    
    ///////////////////////////////
    //UnicodeChar
    ///////////////////////////////

    void jsonParseToBSQ_UnicodeChar(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        bsq_validate(j.is_number_unsigned(), "JSON -> BSQ", 0, nullptr, "Expected an unsigned number for UnicodeChar");
        
        uint64_t vv = j.get<uint64_t>();
        bsq_validate(vv <= std::numeric_limits<uint32_t>::max() && isLegalUnicodeChar((char32_t)vv), "JSON -> BSQ", 0, nullptr, "Value out of range for UnicodeChar");
        *(XUnicodeChar*)resptr = XUnicodeChar{vv};
    }

    void parseToBSQ_UnicodeChar(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        if(lexer->getCurrentTokenType() != BAPITokenType::LiteralUnicodeChar) {
            bsq_validate(lexer->allowSloppyStrings && lexer->getCurrentTokenType() == BAPITokenType::LiteralCChar, "Parse -> BSQ", 0, nullptr, "Expected a CChar token");
        }

        std::array<uint8_t, 64> outchars;
        size_t size = lexer->extractSmallToken(outchars);

        char32_t output = 0;
        if(isMultibyteEncoding(outchars[2])) {
            xxxx;
        }
        else {
            if(size == 4) {
                output = outchars[2]; //just a simple char c"x" and x is not multi-byte encoded
                bsq_validate(isLegalUnicodeChar(output), "Parse -> BSQ", 0, nullptr, "Invalid UnicodeChar literal");
            }
            else {
                bool charok = processCCharFromEncoding(outchars.data() + 2, outchars.data() + size - 3, &output); //skip c' and '
                bsq_validate(charok, "Parse -> BSQ", 0, nullptr, "Invalid CChar literal");
            }
        }
            
        *(XUnicodeChar*)resptr = XUnicodeChar{output};
        lexer->consume();
    }

    json bsqToJSON_UnicodeChar(const TypeInfo* tinfo, const void* valptr)
    {
        XUnicodeChar v = *(XUnicodeChar*)valptr;
        return json(v.value);
    }

    void bsqToBAPI_UnicodeChar(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        XUnicodeChar v = *(XUnicodeChar*)valptr;
        xxxx;
    }

    void displayValue_UnicodeChar(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        xxxx;
    }
}
