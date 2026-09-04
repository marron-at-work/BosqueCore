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

    template <typename T>
    concept BAPITokenIterator = std::forward_iterator<T> && std::same_as<std::iter_value_t<T>, std::uint8_t>;

    template <BAPITokenIterator Iter>
    class BAPIToken
    {
    public:
        BAPITokenType tokentype;

        Iter begin;
        Iter end;
        size_t size;

        void clear()
        {
            this->tokentype = BAPITokenType::Invalid;
        }

        size_t size() const
        {
            return this->size;
        }

        bool matches(const uint8_t* data, size_t len) const
        {
            if(len != this->size) {
                return false;
            }

            return std::equal(this->begin, this->end, data, data + len);
        }

        uint8_t extract() const
        {
            return *(this->begin);
        }

        size_t extract(std::array<uint8_t, 64>& outchars) const
        {
            assert(this->size < 64);

            std::copy(this->begin, this->end, outchars.begin());
            outchars[this->size] = 0;

            return this->size;
        }

        void extract(BSQStreamingBuilder* builder) const
        {
            for(auto it = this->begin; it != this->end; ++it) {
                builder->appendByte(*it);
            }
        }
    };

    class BAPILexer
    {
    public:
        bool allowSloppyStrings;

        virtual BAPITokenType getCurrentTokenType() const = 0;
        virtual size_t getCurrentTokenDataSize() const = 0;
        virtual bool testDataMatches(const uint8_t* data, size_t len) const = 0;
        
        virtual uint8_t extractSingleCharToken() const = 0;
        virtual size_t extractSmallToken(std::array<uint8_t, 64>& outchars) const = 0; //also null terminate the output inbuffer
        virtual bool extractBuilder(BSQStreamingBuilder* builder) const = 0;

        virtual void consume() = 0;

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

    template <BAPITokenIterator Iter>
    class BAPILexerImpl : public BAPILexer
    {
    public:
        BAPITokenType getCurrentTokenType() const override;
        size_t getCurrentTokenDataSize() const override;
        bool testDataMatches(const uint8_t* data, size_t len) const override;
        
        uint8_t extractSingleCharToken() const override;
        size_t extractSmallToken(std::array<uint8_t, 64>& outchars) const override;
        bool extractBuilder(BSQStreamingBuilder* builder) const override;

        void consume() override;

        xxxx;
    };
}