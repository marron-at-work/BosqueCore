#include "common.h"

#include <codecvt>
#include <locale>
#include <sstream>

namespace BREX
{
        std::vector<std::pair<uint8_t, UnicodeString>> s_escape_names_unicode = {
        {0, U"%NUL;"},
        {1, U"%SOH;"},
        {2, U"%STX;"},
        {3, U"%ETX;"},
        {4, U"%EOT;"},
        {5, U"%ENQ;"},
        {6, U"%ACK;"},
        {7, U"%a;"},
        {8, U"%b;"},
        {9, U"%t;"},
        {10, U"%n;"},
        {11, U"%v;"},
        {12, U"%f;"},
        {13, U"%r;"},
        {14, U"%SO;"},
        {15, U"%SI;"},
        {16, U"%DLE;"},
        {17, U"%DC1;"},
        {18, U"%DC2;"},
        {19, U"%DC3;"},
        {20, U"%DC4;"},
        {21, U"%NAK;"},
        {22, U"%SYN;"},
        {23, U"%ETB;"},
        {24, U"%CAN;"},
        {25, U"%EM;"},
        {26, U"%SUB;"},
        {27, U"%e;"},
        {28, U"%FS;"},
        {29, U"%GS;"},
        {30, U"%RS;"},
        {31, U"%US;"},
        {127, U"%DEL;"},

        {32, U"%space;"},
        {33, U"%bang;"},
        {34, U"%;"},
        {34, U"%quote;"},
        {35, U"%hash;"},
        {36, U"%dollar;"},
        {37, U"%%;"},
        {37, U"%percent;"},
        {38, U"%amp;"},
        {39, U"%tick;"},
        {40, U"%lparen;"},
        {41, U"%rparen;"},
        {42, U"%star;"},
        {43, U"%plus;"},
        {44, U"%comma;"},
        {45, U"%dash;"},
        {46, U"%dot;"},
        {47, U"%slash;"},
        {58, U"%colon;"},
        {59, U"%semicolon;"},
        {60, U"%langle;"},
        {61, U"%equal;"},
        {62, U"%rangle;"},
        {63, U"%question;"},
        {64, U"%at;"}, 
        {91, U"%lbracket;"},
        {92, U"%backslash;"},
        {93, U"%rbracket;"},
        {94, U"%caret;"},
        {95, U"%underscore;"},
        {96, U"%backtick;"},
        {123, U"%lbrace;"},
        {124, U"%pipe;"},
        {125, U"%rbrace;"},
        {126, U"%tilde;"}
    };

    std::vector<std::pair<uint8_t, ASCIIString>> s_escape_names_ascii = {
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

    UnicodeString resolveEscapeUnicodeFromCode(uint8_t c)
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

    ASCIIString resolveEscapeASCIIFromCode(uint8_t c)
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
        //assume string has "..." so we need to remove them

        std::stringstream acc;
        auto cconv = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{};

        for(size_t i = 1; i < length - 1; ++i) {
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

                    acc << cconv.to_bytes(esc.value());
                }
                else {
                    auto esc = resolveEscapeUnicodeFromName(bytes + i, sc + 1);
                    if(!esc.has_value()) {
                        return std::nullopt;
                    }

                    acc << (char)esc.value();
                }

                i += std::distance(bytes + i, sc) - 1;
            }
            else {
                acc << (char)c;
            }
        }

        return std::make_optional(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(acc.str()));
    }

    std::vector<uint8_t> escapeString(const UnicodeString& sv)
    {
        std::stringstream ss;
        auto cconv = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{};

        ss << '"';
        for(auto ii = sv.cbegin(); ii != sv.cend(); ++ii) {
            char32_t c = *ii;

            if(c == U'%' || c == U'"' || (c <= 127 && !std::isprint(c))) {
                ss << cconv.to_bytes(resolveEscapeUnicodeFromCode(c));
            }
            else {
                ss << cconv.to_bytes(c);
            }
        }
        ss << '"';

        std::string utf8 = ss.str();
        std::vector<uint8_t> res(utf8.size());

        std::transform(utf8.cbegin(), utf8.cend(), res.begin(), [](char c) { return (uint8_t)c; });

        return res;
    }

    std::optional<std::string> unescapeASCIIString(const uint8_t* bytes, size_t length)
    {
        //assume string has '...' (or `...`, /.../) so we need to remove them

        std::stringstream acc;

        for(size_t i = 1; i < length - 1; ++i) {
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

                    acc << (char)esc.value();
                }
                else {
                    auto esc = resolveEscapeASCIIFromName(bytes + i, sc);
                    if(!esc.has_value()) {
                        return std::nullopt;
                    }

                    acc << (char)esc.value();
                }

                i += std::distance(bytes + i, sc) - 1;
            }
            else {
                acc << (char)c;
            }
        }

        return std::make_optional(acc.str());
    }

    std::vector<uint8_t> escapeASCIIString(const std::string& sv)
    {
        std::stringstream ss;

        for(auto ii = sv.cbegin(); ii != sv.cend(); ++ii) {
            char c = *ii;

            if(c == '%' || c == '\'' || !std::isprint(c)) {
                ss << resolveEscapeASCIIFromCode(c);
            }
            else {
                ss << c;
            }
        }

        std::string ascii = ss.str();
        std::vector<uint8_t> res(ascii.size());

        std::transform(ascii.cbegin(), ascii.cend(), res.begin(), [](char c) { return (uint8_t)c; });

        return res;
    }

    std::optional<UnicodeString> unescapeRegexLiteral(const const uint8_t* bytes, size_t length)
    {
        return unescapeString(bytes, length);
    }

    std::vector<uint8_t> escapeRegexLiteral(const UnicodeString& sv)
    {
        return escapeString(sv);
    }

    std::optional<UnicodeCharCode> unescapeRegexCharRangeValue(const const uint8_t* bytes, size_t length)
    {
        xxxx;
    }

    std::vector<uint8_t> escapeRegexLiteral(UnicodeCharCode cc)
    {
        xxxx;
    }

    std::optional<ASCIIString> unescapeASCIIRegexLiteral(const const uint8_t* bytes, size_t length)
    {
        return unescapeASCIIString(bytes, length);
    }

    std::vector<uint8_t> escapeASCIIRegexLiteral(const ASCIIString& sv)
    {
        return escapeASCIIString(sv);
    }

    std::optional<ASCIICharCode> unescapeASCIIRegexCharRangeValue(const const uint8_t* bytes, size_t length)
    {
        xxxx;
    }

    std::vector<uint8_t> escapeASCIIRegexLiteral(ASCIICharCode cc)
    {

    }
}
