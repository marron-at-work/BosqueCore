#include "encodings.h"

namespace ᐸRuntimeᐳ
{
    size_t multibyteCharCount(uint8_t c) 
    {
        if((c & 0x80) == 0) {
            return 1;
        }
        else if((c & 0xE0) == 0xC0) {
            return 2;
        }
        else if((c & 0xF0) == 0xE0) {
            return 3;
        }
        else if((c & 0xF8) == 0xF0) {
            return 4;
        }
        return 0;
    }

    size_t ucharToMultiByteEncoding(char32_t c, std::array<uint8_t, 4>& outbuff)
    {
        assert(c > 0x7F);
        
        if(c <= 0x7FF) {
            outbuff = { (uint8_t)(0xC0 | (c >> 6)), (uint8_t)(0x80 | (c & 0x3F)), 0, 0 };
            return 2;
        }
        else if(c <= 0xFFFF) {
            outbuff = { (uint8_t)(0xE0 | (c >> 12)), (uint8_t)(0x80 | ((c >> 6) & 0x3F)), (uint8_t)(0x80 | (c & 0x3F)), 0 };
            return 3;
        }
        else {
            outbuff = { (uint8_t)(0xF0 | (c >> 18)), (uint8_t)(0x80 | ((c >> 12) & 0x3F)), (uint8_t)(0x80 | ((c >> 6) & 0x3F)), (uint8_t)(0x80 | (c & 0x3F)) };
            return 4;
       }
    }

    char32_t multibyteToUChar(const std::array<uint8_t, 4>& inbuff, size_t bytecount)
    {
        assert(bytecount != 1);

        //TODO: we need to review this invalid encoding setup 
        //      Specifically we are not handling overlong UTF-8 encodings or invalid byte sequences rigorously.

        if(bytecount == 2) {
            return (char32_t)((inbuff[0] & 0x1F) << 6 | (inbuff[1] & 0x3F));
        }
        else if(bytecount == 3) {
            return (char32_t)((inbuff[0] & 0x0F) << 12 | ((inbuff[1] & 0x3F) << 6) | (inbuff[2] & 0x3F));
        }
        else {
            return (char32_t)((inbuff[0] & 0x07) << 18 | ((inbuff[1] & 0x3F) << 12) | ((inbuff[2] & 0x3F) << 6) | (inbuff[3] & 0x3F));
        }
    }

    /* Given a buffer that contains and encoded CChar (either named or numeric) get the char value (or return false if invalid) */
    bool processEncodedCChar(const std::array<uint8_t, 64>& inbuff, size_t bytecount, char& outchar)
    {
        uint8_t c = inbuff[0];
        
        if(c != '%') { 
            if(bytecount != 1) {
                return false;
            }

            outchar = c;
            return isLegalCChar(c) && !isMustEscapeCChar(c);
        }
        else {
            if(bytecount < 2 || inbuff[bytecount - 1] != ';') {
                return false;
            }

            auto ii = std::find_if(s_escape_names_char.cbegin(), s_escape_names_char.cend(), [&](const auto& p) { return std::strcmp(p.second, reinterpret_cast<const char*>(inbuff.data())) == 0; });
        if(ii != s_escape_names_char.cend()) {
            res = static_cast<char>(ii->first);
            return isLegalCChar(ii->first);
        }

        if(inbuff[1] == 'x') {
            uint8_t output = 0;
            auto [ptr, ec] = std::from_chars(reinterpret_cast<const char*>(inbuff.data()) + 2, reinterpret_cast<const char*>(inbuff.data()) + bytecount - 1, output, 16);

            res = static_cast<char>(output);
            return ec == std::errc() && (ptr == reinterpret_cast<const char*>(inbuff.data()) + bytecount - 1) && isLegalCChar(output);
        }

        return false;
        }
    }
}
