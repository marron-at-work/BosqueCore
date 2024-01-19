#pragma once

#include <string>
#include <optional>

#include "json.hpp"
typedef nlohmann::json json;

namespace BREX
{
    //
    //TODO: wstring is not that great for unicode -- at some point we need to switch to UTF8 etc.
    //
    typedef std::u32string UnicodeString;
    typedef std::string ASCIIString;

    typedef uint32_t UnicodeCharCode;
    typedef char ASCIICharCode;

    class UnicodeIterator
    {
    public:
        const UnicodeString* sstr;
        UnicodeString::const_iterator curr;

        UnicodeIterator(const UnicodeString* sstr) : sstr(sstr), curr(sstr->cbegin()) {;}
        ~UnicodeIterator() {;}
        
        bool valid() const
        {
            return this->curr != this->sstr->cend();
        }

        void advance()
        {
            this->curr++;
        }

        UnicodeCharCode get() const
        {
            return *this->curr;
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

    //Take a utf8 string with escapes and convert to a UnicodeString
    std::optional<UnicodeString> unescapeString(const uint8_t* bytes, size_t length);

    //Convert a UnicodeString string to a utf8 string with escapes
    std::vector<uint8_t> escapeString(const UnicodeString& sv);

    //Take an ascii string with escapes and convert to a true string
    std::optional<ASCIIString> unescapeASCIIString(const uint8_t* bytes, size_t length);

    //Convert an ascii string to an ascii string with escapes
    std::vector<uint8_t> escapeASCIIString(const ASCIIString& sv);

    //Take a utf8 regex literal with escapes and convert to a UnicodeString
    std::optional<UnicodeString> unescapeRegexLiteral(const const uint8_t* bytes, size_t length);

    //Convert a UnicodeString string to a utf8 string with escapes
    std::vector<uint8_t> escapeRegexLiteral(const UnicodeString& sv);

    //Take a utf8 regex CharRange value (possibly with escapes) and convert to a CharCode
    std::optional<UnicodeCharCode> unescapeRegexCharRangeValue(const const uint8_t* bytes, size_t length);

    //Convert a Unicode CharRange value to a utf8 string with escapes
    std::vector<uint8_t> escapeRegexLiteral(UnicodeCharCode cc);

    //Convert an ascii regex string to a ascii regex string with escapes
    std::optional<ASCIIString> unescapeASCIIRegexLiteral(const const uint8_t* bytes, size_t length);

    //Convert an ascii regex string to a ascii regex string with escapes
    std::vector<uint8_t> escapeASCIIRegexLiteral(const ASCIIString& sv);

    //Take a ascii regex CharRange value (possibly with escapes) and convert to a CharCode
    std::optional<ASCIICharCode> unescapeASCIIRegexCharRangeValue(const const uint8_t* bytes, size_t length);

    //Convert an ascii CharRange value to a ascii string with escapes
    std::vector<uint8_t> escapeASCIIRegexLiteral(ASCIICharCode cc);
}

