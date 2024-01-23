#define once

#include "common.h"

namespace BREX
{
    class RegexParser
    {
    public:
        const uint8_t* data;
        uint8_t* cpos;
        const uint8_t* epos;

        const bool isUnicode;
        bool negateAllowed;
        bool envAllowed;

        RegexParser(const uint8_t* data, size_t len, bool isUnicode, bool negateAllowed, bool envAllowed) : data(data), cpos(const_cast<uint8_t*>(data)), epos(data + len), isUnicode(isUnicode), negateAllowed(negateAllowed), envAllowed(envAllowed) {;}
        ~RegexParser() = default;

        inline bool isEOF() const
        {
            return this->cpos == this->epos;
        }
    };

}
