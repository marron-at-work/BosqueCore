#pragma once

#include "common.h"
#include "brex_engine.h"

#include <sstream>

namespace BREX
{
    template <typename C>
    struct SingleCharRange
    {
        C low;
        C high;
    };

    template <typename C, typename S>
    class RegexOpt
    {
    public:
        RegexOpt() {;}
        virtual ~RegexOpt() {;}

        virtual std::string toString() const = 0;

        static RegexOpt* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const = 0;
        virtual StateID compileReverse(StateID follows, std::vector<NFAOpt*>& states) const = 0;
    };

    template <typename S>
    std::vector<uint8_t> convertToRegexLiteralBytes(const S& str)
    {
        return {};
    }

    template <>
    std::vector<uint8_t> convertToRegexLiteralBytes<UnicodeString>(const UnicodeString& str)
    {
        return escapeString(str);
    }

    template <>
    std::vector<uint8_t> convertToRegexLiteralBytes<ASCIIString>(const ASCIIString& str)
    {
        return escapeASCIIString(str);
    }

    template <typename S>
    S convertJSONBytesToRegexLiteral(const std::vector<uint8_t>& bytes)
    {
        S s;
        return s;
    }

    template <>
    UnicodeString convertJSONBytesToRegexLiteral<UnicodeString>(const std::vector<uint8_t>& bytes)
    {
        return unescapeString(bytes.data(), bytes.size()).value();
    }

    template <>
    ASCIIString convertJSONBytesToRegexLiteral<ASCIIString>(const std::vector<uint8_t>& bytes)
    {
        return unescapeASCIIString(bytes.data(), bytes.size()).value();
    }

    template <typename C, typename S>
    class LiteralOpt : public RegexOpt
    {
    public:
        S litstr;

        LiteralOpt(S litstr) : RegexOpt(), litstr(litstr) {;}
        virtual ~LiteralOpt() = default;

        virtual std::u8string toString() const override
        {
            auto bytes = convertToRegexLiteralBytes<S>(this->litstr);
            return "\"" + std::u8string(bytes.begin(), bytes.end()) + "\"";
        }

        static LiteralOpt* parse(json j)
        {
            std::vector<uint8_t> bytes;
            auto jbytes = j[1]["bytes"];
            std::transform(jbytes.cbegin(), jbytes.cend(), std::back_inserter(bytes), [](const json& rv) {
                return rv.get<uint8_t>();
            });

            return new LiteralOpt(convertJSONBytesToRegexLiteral<S>(bytes));
        }

        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
        virtual StateID compilReverse(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    template <typename C>
    std::vector<uint8_t> convertToRegexRangeCharBytes(C c)
    {
        return {};
    }

    template <>
    std::vector<uint8_t> convertToRegexRangeCharBytes<UnicodeCharCode>(UnicodeCharCode c)
    {
        return escapeRegexCharRangeValue(c);
    }

    template <>
    std::vector<uint8_t> convertToRegexRangeCharBytes<ASCIICharCode>(ASCIICharCode c)
    {
        return escapeASCIIRegexCharRangeValue(c);
    }

    template <typename C>
    C convertJSONBytesToRegexRangeChar(const std::vector<uint8_t>& bytes)
    {
        return 0;
    }

    template <>
    UnicodeCharCode convertJSONBytesToRegexRangeChar<UnicodeCharCode>(const std::vector<uint8_t>& bytes)
    {
        return unescapeRegexCharRangeValue(bytes.data(), bytes.size()).value();
    }

    template <>
    ASCIICharCode convertJSONBytesToRegexRangeChar<ASCIICharCode>(const std::vector<uint8_t>& bytes)
    {
        return unescapeASCIIRegexCharRangeValue(bytes.data(), bytes.size()).value();
    }

    //TODO: ADD STATE template <typename C, typename S>
    class CharRangeOpt : public RegexOpt
    {
    public:
        const bool compliment;
        const std::vector<SingleCharRange<C>> ranges;

        CharRangeOpt(bool compliment, std::vector<SingleCharRange<C>> ranges) : RegexOpt(), compliment(compliment), ranges(ranges) {;}
        virtual ~CharRangeOpt() = default;

        virtual std::string toString() const override
        {
            return "[" + std::accumulate(this->ranges.cbegin(), this->ranges.cend(), std::string(this->compliment ? "^" : ""), [](std::string acc, SingleCharRange<C> cr) {
                if(cr.low == cr.high) {
                    return acc + convertToRegexRangeCharBytes<C>(cr.low);
                }
                else {
                    return acc + convertToRegexRangeCharBytes<C>(cr.low) + "-" + convertToRegexRangeCharBytes<C>(cr.high);
                }
            }) + "]";
        }

        static CharRangeOpt* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
        virtual StateID compileReverse(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    //TODO: ADD STATE template <typename C, typename S>
    class CharClassDotOpt : public RegexOpt
    {
    public:
        CharClassDotOpt() : RegexOpt() {;}
        virtual ~CharClassDotOpt() = default;

        virtual std::string toString() const override
        {
            return ".";
        }

        static CharClassDotOpt* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
        virtual StateID compileReverse(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    //TODO: ADD STATE template <typename C, typename S>
    class NegateOpt : public RegexOpt
    {
    public:
        const RegexOpt* opt;

        NegateOpt(RegexOpt* opt) : RegexOpt() opt(opt) {;}
        virtual ~NegateOpt() = default;

        virtual std::string toString() const override
        {
            return ".";
        }

        static NegateOpt* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
        virtual StateID compileReverse(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class BSQStarRepeatRe : public BSQRegexOpt
    {
    public:
        const BSQRegexOpt* opt;

        BSQStarRepeatRe(const BSQRegexOpt* opt) : BSQRegexOpt(), opt(opt) {;}

        virtual ~BSQStarRepeatRe() 
        {
            delete this->opt;
        }

        virtual std::string toString() const override
        {
            return "(" + this->opt->toString() + "*)";
        }

        static BSQStarRepeatRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class BSQPlusRepeatRe : public BSQRegexOpt
    {
    public:
        const BSQRegexOpt* opt;

        BSQPlusRepeatRe(const BSQRegexOpt* opt) : BSQRegexOpt(), opt(opt) {;}
        
        virtual ~BSQPlusRepeatRe()
        {
            delete this->opt;
        }

        virtual std::string toString() const override
        {
            return "(" + this->opt->toString() + "+)";
        }

        static BSQPlusRepeatRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class BSQRangeRepeatRe : public BSQRegexOpt
    {
    public:
        const BSQRegexOpt* opt;
        const uint16_t low;
        const uint16_t high;

        BSQRangeRepeatRe(uint16_t low, uint16_t high, const BSQRegexOpt* opt) : BSQRegexOpt(), opt(opt), low(low), high(high) {;}
        
        virtual ~BSQRangeRepeatRe() 
        {
            delete this->opt;
        }

        virtual std::string toString() const override
        {
            if(this->high == UINT16_MAX)
            {
                return "(" + this->opt->toString() + "{" + std::to_string(this->low) + ",})";
            }
            else if(this->low == this->high)
            {
                return "(" + this->opt->toString() + "{" + std::to_string(this->low) + "})";
            }
            else {
                return "(" + this->opt->toString() + "{" + std::to_string(this->low) + "," + std::to_string(this->high) + "})";
            }
        }

        static BSQRangeRepeatRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class BSQOptionalRe : public BSQRegexOpt
    {
    public:
        const BSQRegexOpt* opt;

        BSQOptionalRe(const BSQRegexOpt* opt) : BSQRegexOpt(), opt(opt) {;}
        virtual ~BSQOptionalRe() {;}

        virtual std::string toString() const override
        {
            return "(" + this->opt->toString() + "?)";
        }

        static BSQOptionalRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class AllOfOpt : public RegexOpt
    {
    public:
        const std::vector<const BSQRegexOpt*> opts;

        BSQAlternationRe(std::vector<const BSQRegexOpt*> opts) : BSQRegexOpt(), opts(opts) {;}

        virtual ~BSQAlternationRe()
        {
            for(size_t i = 0; i < this->opts.size(); ++i) {
                delete this->opts[i];
            }
        }

        virtual std::string toString() const override
        {
            return "(" + std::accumulate(this->opts.cbegin(), this->opts.cend(), std::string(), [](std::string&& acc, const BSQRegexOpt* re) {
                return std::move(acc) + re->toString() + "|";
            }) + ")";
        }

        static BSQAlternationRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class AnyOfOpt : public BSQRegexOpt
    {
    public:
        const std::vector<const BSQRegexOpt*> opts;

        BSQAlternationRe(std::vector<const BSQRegexOpt*> opts) : BSQRegexOpt(), opts(opts) {;}

        virtual ~BSQAlternationRe()
        {
            for(size_t i = 0; i < this->opts.size(); ++i) {
                delete this->opts[i];
            }
        }

        virtual std::string toString() const override
        {
            return "(" + std::accumulate(this->opts.cbegin(), this->opts.cend(), std::string(), [](std::string&& acc, const BSQRegexOpt* re) {
                return std::move(acc) + re->toString() + "|";
            }) + ")";
        }

        static BSQAlternationRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class SequenceOpt : public BSQRegexOpt
    {
    public:
        const std::vector<const BSQRegexOpt*> opts;

        BSQSequenceRe(std::vector<const BSQRegexOpt*> opts) : BSQRegexOpt(), opts(opts) {;}

        virtual ~BSQSequenceRe()
        {
            for(size_t i = 0; i < this->opts.size(); ++i) {
                delete this->opts[i];
            }
        }

        virtual std::string toString() const override
        {
            return "(" + std::accumulate(this->opts.cbegin(), this->opts.cend(), std::string(), [](std::string&& acc, const BSQRegexOpt* re) {
                return std::move(acc) + re->toString();
            }) + ")";
        }

        static BSQSequenceRe* parse(json j);
        virtual StateID compile(StateID follows, std::vector<NFAOpt*>& states) const override final;
    };

    class BSQRegex
    {
    public:
        const BSQRegexOpt* re;
        const NFA* nfare;

        BSQRegex(const BSQRegexOpt* re, NFA* nfare): re(re), nfare(nfare) {;}
        ~BSQRegex() {;}

        static BSQRegex* jparse(json j);

        std::string toString() const 
        {
            return re->toString();
        }

        bool test(CharCodeIterator& cci) const
        {
            return this->nfare->test(cci);
        }

        bool test(const UnicodeString* s) const
        {
            UnicodeIterator siter(s);
            return this->nfare->test(siter);
        }

        bool test(const std::string* s) const
        {
            ASCIIIterator siter(s);
            return this->nfare->test(siter);
        }
    };

    class RegexParser 
    {
    private:
        UnicodeString restr;
        size_t pos;

        RegexParser(const UnicodeString& restr) : restr(restr), pos(0) { ; }

        bool done()
        {
            return this->restr.size() <= this->pos;
        }

        bool isToken(CharCode tk)
        {
            return this->restr[this->pos] == tk;
        }

        CharCode token() {
            return this->restr[this->pos];
        }

        void advance() {
            this->pos++;
        }

        void advance(size_t dist) {
            this->pos = this->pos + dist;
        }

        bool matchLiteralPrefix(UnicodeString pfx)
        {
            for(size_t i = 0; i < pfx.size(); ++i) {
                if(this->pos + i >= this->restr.size()) {
                    return false;
                }

                if(pfx[i] != this->restr[this->pos + i]) {
                    return false;
                }
            }

            return true;
        }

        CharCode readUnescapedChar()
        {
            auto c = this->token();
            this->advance();

            return c;
        }

        const BSQRegexOpt* parseBaseComponent() 
        {
            const BSQRegexOpt* res = nullptr;
            if(this->isToken(U'(')) {
                this->advance();

                res = this->parseComponent();
                if(!this->isToken(U')')) {
                    return nullptr;
                }

                this->advance();
            }
            else if(this->isToken(U'[')) {
                this->advance();

                auto compliment = this->isToken(U'^');
                if(compliment) {
                    this->advance();
                }

                std::vector<SingleCharRange> range;
                while(!this->isToken(U']')) {
                    auto lb = this->readUnescapedChar();

                    if (!this->isToken(U'-')) {
                        range.push_back({ lb, lb });
                    }
                    else {
                        this->advance();

                        auto ub = this->token();
                        range.push_back({ lb, ub });
                    }
                }

                if(!this->isToken(U']')) {
                    return nullptr;
                }
                this->advance();

                return new BSQCharRangeRe(compliment, range);
            }
            else {
                res = new BSQLiteralRe({ this->readUnescapedChar() });
            }

            return res;
        }

        const BSQRegexOpt* parseCharClassOrEscapeComponent()
        {
            if(this->isToken(U'.')) {
                this->advance();
                return new BSQCharClassDotRe();
            }
            else {
                return this->parseBaseComponent();
            }
        }

        const BSQRegexOpt* parseRepeatComponent()
        {
            auto rcc = this->parseCharClassOrEscapeComponent();
            if(rcc == nullptr) {
                return nullptr;
            }

            while(this->isToken(U'*') || this->isToken(U'+') || this->isToken(U'?') || this->isToken(U'{')) {
                if(this->isToken(U'*')) {
                    rcc = new BSQStarRepeatRe(rcc);
                    this->advance();
                }
                else if(this->isToken(U'+')) {
                    rcc = new BSQPlusRepeatRe(rcc);
                    this->advance();
                }
                else if(this->isToken(U'?')) {
                    rcc = new BSQOptionalRe(rcc);
                    this->advance();
                }
                else {
                    this->advance();
                    uint16_t min = 0;
                    while(!this->done() && U'0' < this->token() && this->token() < U'9') {
                        min = min * 10 + (this->token() - U'0');
                        this->advance();
                    }

                    while(!this->done() && this->isToken(U' ')) {
                        this->advance();
                    }

                    uint16_t max = min;
                    if (!this->done() && this->isToken(U',')) {
                        this->advance();

                        while(!this->done() && this->isToken(U' ')) {
                            this->advance();
                        }

                        if(!this->done() && !this->isToken(U'}')) {
                            max = 0;
                            while(!this->done() && U'0' < this->token() && this->token() < U'9') {
                                max = max * 10 + (this->token() - U'0');
                                this->advance();
                            }
                        }
                    }

                    if(this->done() || !this->isToken(U'}')) {
                        return nullptr;
                    }
                    this->advance();

                    rcc = new BSQRangeRepeatRe(min, max, rcc);
                }
            }   

            return rcc;
        }

        const BSQRegexOpt* parseSequenceComponent()
        {
            std::vector<const BSQRegexOpt*> sre;

            while(!this->done() && !this->isToken(U'|') && !this->isToken(U')')) {
                auto rpe = this->parseRepeatComponent();
                if(rpe == nullptr) {
                    return nullptr;
                }

                if(sre.empty()) {
                    sre.push_back(rpe);
                }
                else {
                    auto lcc = sre[sre.size() - 1];
                    if(lcc->isLiteral() && rpe->isLiteral()) {
                        sre[sre.size() - 1] = BSQLiteralRe::mergeLiterals(static_cast<const BSQLiteralRe*>(lcc), static_cast<const BSQLiteralRe*>(rpe));
                        delete lcc;
                        delete rpe;
                    }
                    else {
                        sre.push_back(rpe);
                    }
                }
            }

            if(sre.empty()) {
                return nullptr;
            }

            if (sre.size() == 1) {
                return sre[0];
            }
            else {
                return new BSQSequenceRe(sre);
            }
        }

        const BSQRegexOpt* parseAlternationComponent()
        {
            auto rpei = this->parseSequenceComponent();
            if (rpei == nullptr) {
                return nullptr;
            }

            std::vector<const BSQRegexOpt*> are = {rpei};

            while (!this->done() && this->isToken(U'|')) {
                this->advance();
                auto rpe = this->parseSequenceComponent();
                if (rpe == nullptr) {
                    return nullptr;
                }

                are.push_back(rpe);
            }

            if(are.size() == 1) {
                return are[0];
            }
            else {
                return new BSQAlternationRe(are);
            }
        }

        const BSQRegexOpt* parseComponent()
        {
            return this->parseAlternationComponent();
        }

    public:
        static BSQRegex* parseRegex(UnicodeString restr)
        {
            auto parser = RegexParser(restr);

            auto re = parser.parseComponent();
            if(re == nullptr) {
                return nullptr;
            }

            std::vector<NFAOpt*> nfastates = { new NFAOptAccept(0) };
            auto nfastart = re->compile(0, nfastates);

            auto nfare = new NFA(nfastart, 0, nfastates);
            return new BSQRegex(re, nfare);
        }
    };
}