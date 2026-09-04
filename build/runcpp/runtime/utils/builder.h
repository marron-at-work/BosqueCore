#pragma once

#include "../../common.h"

namespace ᐸRuntimeᐳ 
{
    std::string getDisplayIndent(std::optional<std::string> indent)
    {
        return indent.has_value() ? indent.value() : "";
    }

    /* This is an abstract class for streaming bytes/char/char32 values into a some BSQ object (or byte stream) that can buffer and build efficiently from the input data */   
    class BSQStreamingBuilder
    {
    public:
        BSQStreamingBuilder() {}

        virtual void appendByte(uint8_t byte) = 0;

        virtual void appendChar(char c) = 0;
        virtual void appendChar(char32_t cchar) = 0;

        virtual void appendConstString(const char* str, size_t len) = 0;
        virtual void appendConstString(const char* str) = 0;
    };
}
