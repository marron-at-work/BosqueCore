#pragma once

#include "../../common.h"

namespace ᐸRuntimeᐳ 
{
    constexpr std::array<std::pair<char32_t, const char*>, 68> s_escape_names_unicode = {
        std::make_pair<char32_t, const char*>(0, "%NUL;"),
        std::make_pair<char32_t, const char*>(1, "%SOH;"),
        std::make_pair<char32_t, const char*>(2, "%STX;"),
        std::make_pair<char32_t, const char*>(3, "%ETX;"),
        std::make_pair<char32_t, const char*>(4, "%EOT;"),
        std::make_pair<char32_t, const char*>(5, "%ENQ;"),
        std::make_pair<char32_t, const char*>(6, "%ACK;"),
        std::make_pair<char32_t, const char*>(7, "%a;"),
        std::make_pair<char32_t, const char*>(8, "%b;"),
        std::make_pair<char32_t, const char*>(9, "%t;"),
        std::make_pair<char32_t, const char*>(10, "%n;"),
        std::make_pair<char32_t, const char*>(11, "%v;"),
        std::make_pair<char32_t, const char*>(12, "%f;"),
        std::make_pair<char32_t, const char*>(13, "%r;"),
        std::make_pair<char32_t, const char*>(14, "%SO;"),
        std::make_pair<char32_t, const char*>(15, "%SI;"),
        std::make_pair<char32_t, const char*>(16, "%DLE;"),
        std::make_pair<char32_t, const char*>(17, "%DC1;"),
        std::make_pair<char32_t, const char*>(18, "%DC2;"),
        std::make_pair<char32_t, const char*>(19, "%DC3;"),
        std::make_pair<char32_t, const char*>(20, "%DC4;"),
        std::make_pair<char32_t, const char*>(21, "%NAK;"),
        std::make_pair<char32_t, const char*>(22, "%SYN;"),
        std::make_pair<char32_t, const char*>(23, "%ETB;"),
        std::make_pair<char32_t, const char*>(24, "%CAN;"),
        std::make_pair<char32_t, const char*>(25, "%EM;"),
        std::make_pair<char32_t, const char*>(26, "%SUB;"),
        std::make_pair<char32_t, const char*>(27, "%e;"),
        std::make_pair<char32_t, const char*>(28, "%FS;"),
        std::make_pair<char32_t, const char*>(29, "%GS;"),
        std::make_pair<char32_t, const char*>(30, "%RS;"),
        std::make_pair<char32_t, const char*>(31, "%US;"),
        std::make_pair<char32_t, const char*>(127, "%DEL;"),

        std::make_pair<char32_t, const char*>(32, "%space;"),
        std::make_pair<char32_t, const char*>(33, "%bang;"),
        std::make_pair<char32_t, const char*>(34, "%;"),
        std::make_pair<char32_t, const char*>(34, "%quote;"),
        std::make_pair<char32_t, const char*>(35, "%hash;"),
        std::make_pair<char32_t, const char*>(36, "%dollar;"),
        std::make_pair<char32_t, const char*>(37, "%%;"),
        std::make_pair<char32_t, const char*>(37, "%percent;"),
        std::make_pair<char32_t, const char*>(38, "%amp;"),
        std::make_pair<char32_t, const char*>(39, "%tick;"),
        std::make_pair<char32_t, const char*>(40, "%lparen;"),
        std::make_pair<char32_t, const char*>(41, "%rparen;"),
        std::make_pair<char32_t, const char*>(42, "%star;"),
        std::make_pair<char32_t, const char*>(43, "%plus;"),
        std::make_pair<char32_t, const char*>(44, "%comma;"),
        std::make_pair<char32_t, const char*>(45, "%dash;"),
        std::make_pair<char32_t, const char*>(46, "%dot;"),
        std::make_pair<char32_t, const char*>(47, "%slash;"),
        std::make_pair<char32_t, const char*>(58, "%colon;"),
        std::make_pair<char32_t, const char*>(59, "%semicolon;"),
        std::make_pair<char32_t, const char*>(60, "%langle;"),
        std::make_pair<char32_t, const char*>(61, "%equal;"),
        std::make_pair<char32_t, const char*>(62, "%rangle;"),
        std::make_pair<char32_t, const char*>(63, "%question;"),
        std::make_pair<char32_t, const char*>(64, "%at;"), 
        std::make_pair<char32_t, const char*>(91, "%lbracket;"),
        std::make_pair<char32_t, const char*>(92, "%backslash;"),
        std::make_pair<char32_t, const char*>(93, "%rbracket;"),
        std::make_pair<char32_t, const char*>(94, "%caret;"),
        std::make_pair<char32_t, const char*>(95, "%underscore;"),
        std::make_pair<char32_t, const char*>(96, "%backtick;"),
        std::make_pair<char32_t, const char*>(123, "%lbrace;"),
        std::make_pair<char32_t, const char*>(124, "%pipe;"),
        std::make_pair<char32_t, const char*>(125, "%rbrace;"),
        std::make_pair<char32_t, const char*>(126, "%tilde;")
    };

    constexpr std::array<std::pair<char32_t, const char*>, 4> s_escape_names_unicode_simple = {
        std::make_pair<char32_t, const char*>(9, "%t;"),
        std::make_pair<char32_t, const char*>(10, "%n;"),
        std::make_pair<char32_t, const char*>(34, "%;"),
        std::make_pair<char32_t, const char*>(37, "%%;")
    };

    inline bool isSimpleEscapeUnicodeChar(char32_t c)
    {
        return (c == 9 || c == 10 || c == 34 || c == 37);
    }

    inline bool isMustEscapeUnicodeChar(char32_t c)
    {
        return !(32 <= c && c <= 126) || isSimpleEscapeUnicodeChar(c);
    }

    constexpr std::array<std::pair<uint8_t, const char*>, 37> s_escape_names_char = {
        std::make_pair<uint8_t, const char*>(9, "%t;"),
        std::make_pair<uint8_t, const char*>(10, "%n;"),

        std::make_pair<uint8_t, const char*>(32, "%space;"),
        std::make_pair<uint8_t, const char*>(33, "%bang;"),
        std::make_pair<uint8_t, const char*>(34, "%quote;"),
        std::make_pair<uint8_t, const char*>(35, "%hash;"),
        std::make_pair<uint8_t, const char*>(36, "%dollar;"),
        std::make_pair<uint8_t, const char*>(37, "%%;"),
        std::make_pair<uint8_t, const char*>(37, "%percent;"),
        std::make_pair<uint8_t, const char*>(38, "%amp;"),
        std::make_pair<uint8_t, const char*>(39, "%;"),
        std::make_pair<uint8_t, const char*>(39, "%tick;"),
        std::make_pair<uint8_t, const char*>(40, "%lparen;"),
        std::make_pair<uint8_t, const char*>(41, "%rparen;"),
        std::make_pair<uint8_t, const char*>(42, "%star;"),
        std::make_pair<uint8_t, const char*>(43, "%plus;"),
        std::make_pair<uint8_t, const char*>(44, "%comma;"),
        std::make_pair<uint8_t, const char*>(45, "%dash;"),
        std::make_pair<uint8_t, const char*>(46, "%dot;"),
        std::make_pair<uint8_t, const char*>(47, "%slash;"),
        std::make_pair<uint8_t, const char*>(58, "%colon;"),
        std::make_pair<uint8_t, const char*>(59, "%semi;"),
        std::make_pair<uint8_t, const char*>(60, "%langle;"),
        std::make_pair<uint8_t, const char*>(61, "%equal;"),
        std::make_pair<uint8_t, const char*>(62, "%rangle;"),
        std::make_pair<uint8_t, const char*>(63, "%question;"),
        std::make_pair<uint8_t, const char*>(64, "%at;"), 
        std::make_pair<uint8_t, const char*>(91, "%lbracket;"),
        std::make_pair<uint8_t, const char*>(92, "%backslash;"),
        std::make_pair<uint8_t, const char*>(93, "%rbracket;"),
        std::make_pair<uint8_t, const char*>(94, "%caret;"),
        std::make_pair<uint8_t, const char*>(95, "%underscore;"),
        std::make_pair<uint8_t, const char*>(96, "%backtick;"),
        std::make_pair<uint8_t, const char*>(123, "%lbrace;"),
        std::make_pair<uint8_t, const char*>(124, "%pipe;"),
        std::make_pair<uint8_t, const char*>(125, "%rbrace;"),
        std::make_pair<uint8_t, const char*>(126, "%tilde;")
    };

    constexpr std::array<std::pair<uint8_t, const char*>, 4> s_escape_names_char_simple = {
        std::make_pair<uint8_t, const char*>(9, "%t;"),
        std::make_pair<uint8_t, const char*>(10, "%n;"),
        std::make_pair<uint8_t, const char*>(37, "%%;"),
        std::make_pair<uint8_t, const char*>(39, "%;")
    };

    inline bool isMustEscapeCChar(char c)
    {
        return (c == '%' || c == '\'' || c == '\t' || c == '\n');
    }

    inline bool isLegalCChar(uint8_t c)
    {
        return c <= 126 && (std::isprint(c) || (c == '\t') || (c == '\n'));
    }

    inline bool isLegalUnicodeChar(char32_t c)
    {
        return (c <= 0x10FFFF) && !((0xD800 <= c) && (c <= 0xDFFF));
    }

    inline bool isMultibyteEncoding(uint8_t c)
    {
        return (c & 0x80) != 0;
    }

    inline bool isTrimableWhitespace(char32_t c) {
        //TODO: we don't really handle unicode whitespace -- need to investigate the expected behavior and update this function accordingly
        return std::isspace(c);
    }

    size_t multibyteCharCount(uint8_t c);
    size_t ucharToMultiByteEncoding(char32_t c, std::array<uint8_t, 4>& outbuff);
    char32_t multibyteToUChar(const std::array<uint8_t, 4>& inbuff, size_t bytecount);

    /* Given a buffer that contains and encoded CChar (either named or numeric) get the char value (or return false if invalid) */
    bool processEncodedCChar(const std::array<uint8_t, 64>& inbuff, size_t bytecount, char& outchar);
}
