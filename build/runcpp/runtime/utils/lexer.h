#pragma once

#include "../../common.h"

#include "builder.h"

namespace ᐸRuntimeᐳ 
{
    char* skipPlusSignOpt(char* ptr)
    {
        if(*ptr == '+') {
            return ptr + 1;
        }
        else {
            return ptr;
        }
    }

    enum class BAPITokenType : uint64_t
    {
        Invalid = 0,
        ErrorToken,
        EOFToken,
        LiteralNone,
        LiteralTrue,
        LiteralFalse,
        LiteralNat,
        LiteralInt,
        LiteralChkNat,
        LiteralChkInt,
        LiteralFloat,
        LiteralByte,
        LiteralCChar,
        LiteralUnicodeChar,
        LiteralCString,
        LiteralString,
        LiteralByteBuffer,
        LiteralSymbol,
        LiteralKeyword,
        Identifier
    };

    class BAPIIteratorAdaptor
    {
    public:
        virtual uint8_t get() const = 0;
        virtual void advance() = 0;
    };

    class BAPIToken
    {
    public:
        BAPITokenType tokentype;

        BAPIIteratorAdaptor* iter; //This is singleton and is re-used across tokens
        size_t size;

        void clear()
        {
            this->tokentype = BAPITokenType::Invalid;
            this->size = 0;
        }

        bool matches(const uint8_t* data, size_t len) const
        {
            if(len != this->size) {
                return false;
            }

            for(size_t i = 0; i < this->size; ++i) {
                if(this->iter->get() != data[i]) {
                    return false;
                }
                this->iter->advance();
            }
            return true;
        }

        uint8_t extract() const
        {
            return this->iter->get();
        }

        size_t extract(std::array<uint8_t, 64>& outchars) const
        {
            assert(this->size < 64);

            auto it = this->iter;
            for(size_t i = 0; i < this->size; ++i) {
                outchars[i] = it->get();
                it->advance();
            }
            outchars[this->size] = 0;

            return this->size;
        }
    };

    class BAPILexer
    {
    private:
        BAPIToken ctoken; //This is singleton and is re-used across lex calls

    public:
        bool allowSloppyStrings;

        BAPITokenType getCurrentTokenType() const
        {
            return this->ctoken.tokentype;
        }

        size_t getCurrentTokenDataSize() const
        {
            return this->ctoken.size;
        }

        BAPIIteratorAdaptor* getCurrentTokenIterator() const
        {
            return this->ctoken.iter;
        }

        bool testDataMatches(const uint8_t* data, size_t len) const
        {
            return this->ctoken.matches(data, len);
        }
        
        uint8_t extractSingleCharToken() const
        {
            return this->ctoken.extract();
        }

        //also null terminate the output inbuffer
        size_t extractSmallToken(std::array<uint8_t, 64>& outchars) const
        {
            return this->ctoken.extract(outchars);
        }
        
        void consume()
        {
            this->ctoken.clear(); //make sure we reset the current token before consuming the next one

            xxxx;
        }

        bool testIsNone() const
        {
            return this->getCurrentTokenType() == BAPITokenType::LiteralNone;
        }

        bool testIsTrue() const
        {
            return this->getCurrentTokenType() == BAPITokenType::LiteralTrue;
        }

        bool testIsFalse() const
        {
            return this->getCurrentTokenType() == BAPITokenType::LiteralFalse;
        }

        bool testIsSymbol(char sym) const
        {
            auto tokentype = this->getCurrentTokenType();
            if(tokentype != BAPITokenType::LiteralSymbol || this->getCurrentTokenDataSize() != 1) {
                return false;
            }

            return this->extractSingleCharToken() == sym;
        }

        template<size_t N>
        bool testIsSymbol(char (&sym)[N]) const
        {
            auto tokentype = this->getCurrentTokenType();
            if(tokentype != BAPITokenType::LiteralSymbol || this->getCurrentTokenDataSize() != N - 1) {
                return false;
            }

            return this->testDataMatches(reinterpret_cast<const uint8_t*>(sym), N - 1);
        }

        template<size_t N>
        bool testIsKeyword(char (&sym)[N]) const
        {
            auto tokentype = this->getCurrentTokenType();
            if(tokentype != BAPITokenType::LiteralKeyword || this->getCurrentTokenDataSize() != N - 1) {
                return false;
            }

            return this->testDataMatches(reinterpret_cast<const uint8_t*>(sym), N - 1);
        }

        template<typename T>
        bool tryExtractNumericValue(T& outval)
        {
            std::array<uint8_t, 64> outchars;
            size_t size = this->extractSmallToken(outchars);

            auto [ptr, ec] = std::from_chars(skipPlusSignOpt(reinterpret_cast<char*>(outchars.data())), reinterpret_cast<char*>(outchars.data()) + size - 1, outval);
            return ec == std::errc() && ptr == reinterpret_cast<char*>(outchars.data()) + size - 1;
        }
    };
}