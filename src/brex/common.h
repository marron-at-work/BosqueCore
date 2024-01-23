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
    typedef char8_t UnicodeStringChar;

    typedef std::string ASCIIString;
    typedef char ASCIIStringChar;

    typedef uint32_t RegexChar;

#ifdef BREX_DEBUG
    void processAssert(const char* file, int line, const char* msg) __attribute__ ((noreturn));
#endif

    class UnicodeRegexIterator
    {
    public:
        const UnicodeString* sstr;
        UnicodeString::const_iterator curr;

        UnicodeRegexIterator(const UnicodeString* sstr) : sstr(sstr), curr(sstr->cbegin()) {;}
        ~UnicodeRegexIterator() = default;
        
        size_t charCodeByteCount() const;
        RegexChar toRegexCharCodeFromBytes() const;

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

        inline RegexChar get() const
        {
            //if this is a multibyte char then decode the number of bytes -- fast path on single byte
            if(UTF8_CHARCODE_USES_SINGLEBYTE_ENCODING(*this->curr)) {
                return *this->curr;
            }
            else {
                return this->toRegexCharCodeFromBytes();
            }
        }
    };

    class ASCIIRegexIterator
    {
    public:
        const ASCIIString* sstr;
        ASCIIString::const_iterator curr;

        ASCIIRegexIterator(const ASCIIString* sstr) : sstr(sstr), curr(sstr->cbegin()) {;}
        ~ASCIIRegexIterator() = default;
        
        bool valid() const
        {
            return this->curr != this->sstr->cend();
        }

        void advance()
        {
            this->curr++;
        }

        RegexChar get() const
        {
            return *this->curr;
        }
    };

    size_t charCodeByteCount(const uint8_t* buff);
    RegexChar toRegexCharCodeFromBytes(const uint8_t* buff, size_t length);

    std::optional<RegexChar> decodeHexEscapeAsRegex(const uint8_t* s, const uint8_t* e);
    std::optional<UnicodeString> decodeHexEscapeAsUnicode(const uint8_t* s, const uint8_t* e);
    std::optional<ASCIIString> decodeHexEscapeAsASCII(const uint8_t* s, const uint8_t* e);
    std::vector<uint8_t> extractRegexCharToBytes(RegexChar rc); //utf8 encoded but no escaping

    //Take a bytebuffer (of utf8 bytes) with escapes and convert to/from a UnicodeString
    std::optional<UnicodeString> unescapeString(const uint8_t* bytes, size_t length);
    std::vector<uint8_t> escapeString(const UnicodeString& sv);

    //Take a bytebuffer (of ascii bytes) with escapes and convert to/from an ASCIIString
    std::optional<ASCIIString> unescapeASCIIString(const uint8_t* bytes, size_t length);
    std::vector<uint8_t> escapeASCIIString(const ASCIIString& sv);

    //Take a bytebuffer regex literal (of utf8 bytes or ascii bytes) with escapes and convert to/from a vector of RegexChars
    std::optional<std::vector<RegexChar>> unescapeRegexLiteral(const const uint8_t* bytes, size_t length);
    std::optional<std::vector<RegexChar>> unescapeASCIIRegexLiteral(const const uint8_t* bytes, size_t length);

    std::vector<uint8_t> escapeSingleRegexChar(RegexChar c);
    std::vector<uint8_t> escapeRegexLiteralCharBuffer(const std::vector<RegexChar>& sv);
}

