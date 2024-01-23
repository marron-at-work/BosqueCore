#pragma once

#include <string>
#include <optional>

#include "json.hpp"
typedef nlohmann::json json;

#ifdef BREX_DEBUG
#define BREX_ABORT(msg) processAssert(__FILE__, __LINE__, msg)
#define BREX_ASSERT(condition, msg) if(!(condition)) { processAssert(__FILE__, __LINE__, msg); }
#else

#define BREX_ABORT(msg)
#define BREX_ASSERT(condition, msg)
#endif

#define UTF8_IS_SINGLEBYTE_ENCODING(byte) (((byte) & 0b10000000) == 0)
#define UTF8_IS_MULTIBYTE_ENCODING(byte) (((byte) & 0b10000000) != 0)

#define UTF8_CHARCODE_USES_SINGLEBYTE_ENCODING(cc) ((cc) <= 0x7F)
#define UTF8_CHARCODE_USES_MULTIBYTE_ENCODING(cc) ((cc) > 0x7F)

namespace BREX
{
    typedef std::u8string UnicodeString;
    typedef std::string ASCIIString;

    typedef uint32_t UnicodeCharCode;
    typedef char ASCIICharCode;

#ifdef BREX_DEBUG
    void processAssert(const char* file, int line, const char* msg) __attribute__ ((noreturn));
#endif

    //compute the number of bytes that the next codepoint in the utf8 string uses
    size_t charCodeByteCount(const uint8_t* byteptr);

    //convert to a UnicodeCharCode from bytes
    std::optional<UnicodeCharCode> toUnicodeCharCodeFromBytes(const uint8_t* byteptr, const uint8_t* endptr);

    //convert from a UnicodeCharCode to bytes
    void toBytesFromUnicodeCharCode(UnicodeCharCode cc, std::vector<char8_t>& intochars);

    class UnicodeIterator
    {
    public:
        const UnicodeString* sstr;
        UnicodeString::const_iterator curr;

        UnicodeIterator(const UnicodeString* sstr) : sstr(sstr), curr(sstr->cbegin()) {;}
        ~UnicodeIterator() {;}
        
        size_t charCodeByteCount() const;
        UnicodeCharCode toUnicodeCharCodeFromBytes() const;

        inline bool valid() const
        {
            return this->curr != this->sstr->cend();
        }

        inline void advance()
        {
            //if this is a multibyte char then advance by the number of bytes -- fast path on single byte
            if(UTF8_IS_SINGLEBYTE_ENCODING(*this->curr)) {
                this->curr++;
            }
            else {
                this->curr += this->charCodeByteCount();
            }
        }

        inline UnicodeCharCode get() const
        {
            //if this is a multibyte char then decode the number of bytes -- fast path on single byte
            if(UTF8_CHARCODE_USES_SINGLEBYTE_ENCODING(*this->curr)) {
                return *this->curr;
            }
            else {
                return this->toUnicodeCharCodeFromBytes();
            }
        }
    };

    class ASCIIIterator
    {
    public:
        const ASCIIString* sstr;
        ASCIIString::const_iterator curr;

        ASCIIIterator(const ASCIIString* sstr) : sstr(sstr), curr(sstr->cbegin()) {;}
        ~ASCIIIterator() {;}
        
        bool valid() const
        {
            return this->curr != this->sstr->cend();
        }

        void advance()
        {
            this->curr++;
        }

        ASCIICharCode get() const
        {
            return *this->curr;
        }
    };

    //Take a bytebuffer (of utf8 bytes) with escapes and convert to a UnicodeString
    std::optional<UnicodeString> unescapeString(const uint8_t* bytes, size_t length);

    //Convert a UnicodeString string to a bytebuffer (of utf8 bytes) with escapes
    std::vector<uint8_t> escapeString(const UnicodeString& sv);

    //Take an ascii string with escapes and convert to a true string
    std::optional<ASCIIString> unescapeASCIIString(const uint8_t* bytes, size_t length);

    //Convert an ascii string to an ascii string with escapes
    std::vector<uint8_t> escapeASCIIString(const ASCIIString& sv);

    //Take a utf8 regex literal with escapes and convert to a UnicodeString
    std::optional<UnicodeString> unescapeRegexLiteral(const const uint8_t* bytes, size_t length);

    //Convert a UnicodeString regex literal string into a utf8 string with escapes
    std::vector<uint8_t> escapeRegexLiteral(const UnicodeString& sv);

    //Take a utf8 regex CharRange value (possibly with escapes) and convert to a CharCode
    std::optional<UnicodeCharCode> unescapeRegexCharRangeValue(const const uint8_t* bytes, size_t length);

    //Convert a Unicode CharRange value to a utf8 string with escapes
    std::vector<uint8_t> escapeRegexCharRangeValue(UnicodeCharCode cc);

    //Convert an ascii regex string to a ascii regex string with escapes
    std::optional<ASCIIString> unescapeASCIIRegexLiteral(const const uint8_t* bytes, size_t length);

    //Convert an ascii regex string to a ascii regex string with escapes
    std::vector<uint8_t> escapeASCIIRegexLiteral(const ASCIIString& sv);

    //Take a ascii regex CharRange value (possibly with escapes) and convert to a CharCode
    std::optional<ASCIICharCode> unescapeASCIIRegexCharRangeValue(const const uint8_t* bytes, size_t length);

    //Convert an ascii CharRange value to a ascii string with escapes
    std::vector<uint8_t> escapeASCIIRegexCharRangeValue(ASCIICharCode cc);
}

