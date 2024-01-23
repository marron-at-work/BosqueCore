#include "common.h"

#include <codecvt>
#include <locale>

#ifdef BREX_DEBUG
    void processAssert(const char* file, int line, const char* msg)
    {
        fprintf(stderr, "Assertion failed: %s:%d -- %s\n", file, line, msg);
        abort();
    }
#endif

#define UTF8_ENCODING_BYTE_COUNT(B) utf8_encoding_sizes[((uint8_t)(B)) >> 4]

namespace BREX
{
        size_t utf8_encoding_sizes[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4};

        std::vector<std::pair<uint8_t, const char*>> s_escape_names_unicode = {
        {0, "%NUL;"},
        {1, "%SOH;"},
        {2, "%STX;"},
        {3, "%ETX;"},
        {4, "%EOT;"},
        {5, "%ENQ;"},
        {6, "%ACK;"},
        {7, "%a;"},
        {8, "%b;"},
        {9, "%t;"},
        {10, "%n;"},
        {11, "%v;"},
        {12, "%f;"},
        {13, "%r;"},
        {14, "%SO;"},
        {15, "%SI;"},
        {16, "%DLE;"},
        {17, "%DC1;"},
        {18, "%DC2;"},
        {19, "%DC3;"},
        {20, "%DC4;"},
        {21, "%NAK;"},
        {22, "%SYN;"},
        {23, "%ETB;"},
        {24, "%CAN;"},
        {25, "%EM;"},
        {26, "%SUB;"},
        {27, "%e;"},
        {28, "%FS;"},
        {29, "%GS;"},
        {30, "%RS;"},
        {31, "%US;"},
        {127, "%DEL;"},

        {32, "%space;"},
        {33, "%bang;"},
        {34, "%;"},
        {34, "%quote;"},
        {35, "%hash;"},
        {36, "%dollar;"},
        {37, "%%;"},
        {37, "%percent;"},
        {38, "%amp;"},
        {39, "%tick;"},
        {40, "%lparen;"},
        {41, "%rparen;"},
        {42, "%star;"},
        {43, "%plus;"},
        {44, "%comma;"},
        {45, "%dash;"},
        {46, "%dot;"},
        {47, "%slash;"},
        {58, "%colon;"},
        {59, "%semicolon;"},
        {60, "%langle;"},
        {61, "%equal;"},
        {62, "%rangle;"},
        {63, "%question;"},
        {64, "%at;"}, 
        {91, "%lbracket;"},
        {92, "%backslash;"},
        {93, "%rbracket;"},
        {94, "%caret;"},
        {95, "%underscore;"},
        {96, "%backtick;"},
        {123, "%lbrace;"},
        {124, "%pipe;"},
        {125, "%rbrace;"},
        {126, "%tilde;"}
    };

    std::vector<std::pair<uint8_t, const char*>> s_escape_names_ascii = {
        {32, "%space;"},
        {33, "%bang;"},
        {34, "%quote;"},
        {35, "%hash;"},
        {36, "%dollar;"},
        {37, "%%;"},
        {37, "%percent;"},
        {38, "%amp;"},
        {39, "%;"},
        {39, "%tick;"},
        {40, "%lparen;"},
        {41, "%rparen;"},
        {42, "%star;"},
        {43, "%plus;"},
        {44, "%comma;"},
        {45, "%dash;"},
        {46, "%dot;"},
        {47, "%slash;"},
        {58, "%colon;"},
        {59, "%semi;"},
        {60, "%langle;"},
        {61, "%equal;"},
        {62, "%rangle;"},
        {63, "%question;"},
        {64, "%at;"}, 
        {91, "%lbracket;"},
        {92, "%backslash;"},
        {93, "%rbracket;"},
        {94, "%caret;"},
        {95, "%underscore;"},
        {96, "%backtick;"},
        {123, "%lbrace;"},
        {124, "%pipe;"},
        {125, "%rbrace;"},
        {126, "%tilde;"}
    };

    size_t charCodeByteCount(const uint8_t* byteptr)
    {
        return UTF8_ENCODING_BYTE_COUNT(*byteptr);
    }

    std::optional<UnicodeCharCode> toUnicodeCharCodeFromBytes(const uint8_t* byteptr, const uint8_t* endptr)
    {
        auto bytecount = UTF8_ENCODING_BYTE_COUNT(*byteptr);
        if(byteptr + bytecount > endptr) {
            return std::nullopt;
        }

        auto curr = byteptr + 1;
        UnicodeCharCode cval = *byteptr;
        while(--bytecount > 0)
        {
            cval = (cval << 8) | (((char32_t)(*curr++)) & 0xff);
        }

        return std::make_optional<UnicodeCharCode>(cval);
    }

    void toBytesFromUnicodeCharCode(UnicodeCharCode cc, std::vector<char8_t>& intochars)
    {
        //TODO: this is a bit inefficient -- we should be able to do this tithout codecvt

        auto cconv = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(cc);
        std::copy(cconv.cbegin(), cconv.cend(), std::back_inserter(intochars));
    }

    size_t UnicodeIterator::charCodeByteCount() const
    {
        return UTF8_ENCODING_BYTE_COUNT(*this->curr);
    }

    UnicodeCharCode UnicodeIterator::toUnicodeCharCodeFromBytes() const
    {
        auto bytecount = UTF8_ENCODING_BYTE_COUNT(*this->curr);

        auto curr = this->curr + 1;
        UnicodeCharCode cval = *this->curr;
        while(--bytecount > 0)
        {
            cval = (cval << 8) | (((char32_t)(*curr++)) & 0xff);
        }

        return cval;
    } 

    std::optional<UnicodeCharCode> decodeHexEscape(const uint8_t* s, const uint8_t* e)
    {
        size_t ccount = std::distance(s, e);

        //1-6 digits and a %;
        if(ccount <= 2 || 8 < ccount) {
            return std::nullopt;
        }

        UnicodeCharCode cval;
        auto sct = sscanf((char*)(s + 1), "%x;", &cval);
        if(sct != 1) {
            return std::nullopt;
        }
        else {
            return std::make_optional<UnicodeCharCode>(cval);
        }
    }

    const char* resolveEscapeUnicodeFromCode(char8_t c)
    {
        auto ii = std::find_if(s_escape_names_unicode.cbegin(), s_escape_names_unicode.cend(), [c](const std::pair<uint8_t, UnicodeString>& p) { 
            return p.first == c; 
        });
        return ii->second;
    }

    std::optional<uint8_t> resolveEscapeUnicodeFromName(const uint8_t* s, const uint8_t* e)
    {
        auto ii = std::find_if(s_escape_names_unicode.cbegin(), s_escape_names_unicode.cend(), [s, e](const std::pair<uint8_t, UnicodeString>& p) { 
            return std::equal(p.second.cbegin(), p.second.cend(), s, e);
        });
        if(ii == s_escape_names_unicode.cend()) {
            return std::nullopt;
        }
        else {
            return std::make_optional(ii->first);
        }
    }

    const char* resolveEscapeASCIIFromCode(char c)
    {
        auto ii = std::find_if(s_escape_names_ascii.cbegin(), s_escape_names_ascii.cend(), [c](const std::pair<uint8_t, std::string>& p) { 
            return p.first == c; 
        });
        return ii->second;
    }

    std::optional<uint8_t> resolveEscapeASCIIFromName(const uint8_t* s, const uint8_t* e)
    {
        auto ii = std::find_if(s_escape_names_ascii.cbegin(), s_escape_names_ascii.cend(), [s, e](const std::pair<uint8_t, std::string>& p) { 
            return std::equal(p.second.cbegin(), p.second.cend(), s, e); 
        });
        if(ii == s_escape_names_ascii.cend()) {
            return std::nullopt;
        }
        else {
            return std::make_optional(ii->first);
        }
    }

    std::optional<UnicodeString> unescapeString(const uint8_t* bytes, size_t length)
    {
        std::vector<char8_t> acc;
        for(size_t i = 0; i < length; ++i) {
            uint8_t c = bytes[i];

            if(c <= 127 && !std::isprint(c) && !std::iswspace(c)) {
                return std::nullopt;
            }

            if(c == '%') {
                auto sc = std::find(bytes + i, bytes + length, ';');
                if(sc == bytes + length) {
                    return std::nullopt;
                }

                if(std::isdigit(bytes[i + 1])) {
                    //it should be a hex number
                    auto esc = decodeHexEscape(bytes + i, sc + 1);
                    if(!esc.has_value()) {
                        return std::nullopt;
                    }

                    toBytesFromUnicodeCharCode(esc.value(), acc);
                }
                else {
                    auto esc = resolveEscapeUnicodeFromName(bytes + i, sc + 1);
                    if(!esc.has_value()) {
                        return std::nullopt;
                    }

                    acc.push_back(esc.value());
                }

                i += std::distance(bytes + i, sc) - 1;
            }
            else {
                acc.push_back(c);
            }
        }

        return std::make_optional<UnicodeString>(acc.cbegin(), acc.cend());
    }

    std::optional<std::vector<UnicodeCharCode>> unescapeStringCodes(const uint8_t* bytes, size_t length)
    {
        auto x = unescapeString(bytes, length);
        if(!x.has_value()) {
            return std::nullopt;
        }

        auto uiter = UnicodeIterator(&x.value());
        std::vector<UnicodeCharCode> acc;
        while (uiter.valid())
        {
            acc.push_back(uiter.get());
            uiter.advance();
        }

        return std::make_optional(acc);
    }

    std::vector<uint8_t> escapeString(const UnicodeString& sv)
    {
        std::vector<uint8_t> acc = {};
        for(auto ii = sv.cbegin(); ii != sv.cend(); ++ii) {
            char8_t c = *ii;

            if(c == U'%' || c == U'"' || (c <= 127 && !std::isprint(c))) {
                auto escc = resolveEscapeUnicodeFromCode(c);
                while(*escc != '\0') {
                    acc.push_back(*escc++);
                }
            }
            else {
                acc.push_back(c);
            }
        }

        return std::move(acc);
    }

    std::optional<ASCIIString> unescapeASCIIString(const uint8_t* bytes, size_t length)
    {
        std::vector<char> acc;
        for(size_t i = 0; i < length; ++i) {
            uint8_t c = bytes[i];

            if(!std::isprint(c) && !std::iswspace(c)) {
                return std::nullopt;
            }

            if(c == '%') {
                auto sc = std::find(bytes + i, bytes + length, ';');
                if(sc == bytes + length) {
                    return std::nullopt;
                }

                if(std::isdigit(bytes[i + 1])) {
                    auto esc = decodeHexEscape(bytes + i, sc);
                    if(!esc.has_value() || esc.value() > 127) {
                        return std::nullopt;
                    }

                    acc.push_back(esc.value());
                }
                else {
                    auto esc = resolveEscapeASCIIFromName(bytes + i, sc);
                    if(!esc.has_value()) {
                        return std::nullopt;
                    }

                    acc.push_back(esc.value());
                }

                i += std::distance(bytes + i, sc) - 1;
            }
            else {
                acc.push_back(c);
            }
        }

        return std::make_optional(ASCIIString(acc.cbegin(), acc.cend()));
    }

    std::optional<std::vector<ASCIICharCode>> unescapeASCIIStringCodes(const uint8_t* bytes, size_t length)
    {
        auto x = unescapeASCIIString(bytes, length);
        if(!x.has_value()) {
            return std::nullopt;
        }

        auto uiter = ASCIIIterator(&x.value());
        std::vector<ASCIICharCode> acc;
        while (uiter.valid())
        {
            acc.push_back(uiter.get());
            uiter.advance();
        }

        return std::make_optional(acc);
    }

    std::vector<uint8_t> escapeASCIIString(const std::string& sv)
    {
        std::vector<uint8_t> acc = {};
        for(auto ii = sv.cbegin(); ii != sv.cend(); ++ii) {
            char c = *ii;

            if(c == '%' || c == '\'' || !std::isprint(c)) {
                auto escc = resolveEscapeASCIIFromCode(c);
                while(*escc != '\0') {
                    acc.push_back(*escc++);
                }
            }
            else {
                acc.push_back(c);
            }
        }

        return std::move(acc);
    }

    std::optional<UnicodeString> unescapeRegexLiteral(const const uint8_t* bytes, size_t length)
    {
        return unescapeString(bytes, length);
    }

    std::vector<uint8_t> escapeRegexLiteral(const UnicodeString& sv)
    {
        std::vector<uint8_t> acc = {};
        for(auto ii = sv.cbegin(); ii != sv.cend(); ++ii) {
            char8_t c = *ii;

            if(c == U'%' || c == U'"' || c == U'/' || c == U'\\' || (c <= 127 && !std::isprint(c))) {
                auto escc = resolveEscapeUnicodeFromCode(c);
                while(*escc != '\0') {
                    acc.push_back(*escc++);
                }
            }
            else {
                acc.push_back(c);
            }
        }

        return std::move(acc);
    }

    std::optional<UnicodeCharCode> unescapeRegexCharRangeValue(const const uint8_t* bytes, size_t length)
    {
        if(*bytes != '%') {
            auto bcount = charCodeByteCount(bytes);

            if(bcount == 1) {
                return std::make_optional<UnicodeCharCode>(*bytes);
            }
            else {
                return toUnicodeCharCodeFromBytes(bytes, bytes + bcount);
            }
        }
        else {
            auto sc = std::find(bytes, bytes + length, ';');
            if(sc == bytes + length) {
                return std::nullopt;
            }
        
            if(std::isdigit(bytes[1])) {
                return decodeHexEscape(bytes, sc);
            }
            else {
                return resolveEscapeUnicodeFromName(bytes, sc);
            }
        }
    }

    std::vector<uint8_t> escapeRegexCharRangeValue(UnicodeCharCode cc)
    {
        std::vector<uint8_t> acc;
        if(cc == U'%' || cc == U'"' || cc == U'/' || cc == U'\\' || (cc <= 127 && !std::isprint(cc))) {
            auto escc = resolveEscapeUnicodeFromCode(cc);
            while(*escc != '\0') {
                acc.push_back(*escc++);
            }
        }
        else {
            acc.push_back(cc);
        }

        return std::move(acc);
    }

    std::optional<ASCIIString> unescapeASCIIRegexLiteral(const const uint8_t* bytes, size_t length)
    {
        return unescapeASCIIString(bytes, length);
    }

    std::vector<uint8_t> escapeASCIIRegexLiteral(const ASCIIString& sv)
    {
        std::vector<uint8_t> acc = {};
        for(auto ii = sv.cbegin(); ii != sv.cend(); ++ii) {
            char c = *ii;

            if(c == '%' || c == '\'' || c == U'/' || c == U'\\' || !std::isprint(c)) {
                auto escc = resolveEscapeASCIIFromCode(c);
                while(*escc != '\0') {
                    acc.push_back(*escc++);
                }
            }
            else {
                acc.push_back(c);
            }
        }

        return std::move(acc);
    }

    std::optional<ASCIICharCode> unescapeASCIIRegexCharRangeValue(const const uint8_t* bytes, size_t length)
    {
        if(*bytes != '%') {
            return std::make_optional<ASCIICharCode>(*bytes);
        }
        else {
            auto sc = std::find(bytes, bytes + length, ';');
            if(sc == bytes + length) {
                return std::nullopt;
            }
        
            if(std::isdigit(bytes[1])) {
                return decodeHexEscape(bytes, sc);
            }
            else {
                return resolveEscapeASCIIFromName(bytes, sc);
            }
        }
    }

    std::vector<uint8_t> escapeASCIIRegexCharRangeValue(ASCIICharCode cc)
    {
        std::vector<uint8_t> acc;
        if(cc == U'%' || cc == U'"' || cc == U'/' || cc == U'\\' || (cc <= 127 && !std::isprint(cc))) {
            auto escc = resolveEscapeASCIIFromCode(cc);
            while(*escc != '\0') {
                acc.push_back(*escc++);
            }
        }
        else {
            acc.push_back(cc);
        }

       return std::move(acc);
    }
}
