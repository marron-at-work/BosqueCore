#pragma once

#include "common.h"
#include "brex_engine.h"

#include <sstream>

namespace BREX
{
    struct SingleCharRange
    {
        RegexChar low;
        RegexChar high;
    };

    class RegexOpt
    {
    public:
        RegexOpt() {;}
        virtual ~RegexOpt() {;}

        virtual bool needsParens() const { return false; }
        virtual bool needsSequenceParens() const { return false; }
        virtual std::u8string toBSQONFormat() const = 0;

        static RegexOpt* jparse(json j);
    };

    class LiteralOpt : public RegexOpt
    {
    public:
        const std::vector<RegexChar> codes;
        const bool isunicode;

        LiteralOpt(std::vector<RegexChar> codes, bool isunicode) : RegexOpt(), codes(codes), isunicode(isunicode) {;}
        virtual ~LiteralOpt() = default;

        virtual std::u8string toBSQONFormat() const override
        {
            auto bbytes = escapeRegexLiteralCharBuffer(this->codes);
            if(this->isunicode) {
                return std::u8string{'"'} + std::u8string(bbytes.cbegin(), bbytes.cend()) + std::u8string{'"'};
            }
            else {
                return std::u8string{'\''} + std::u8string(bbytes.cbegin(), bbytes.cend()) + std::u8string{'\''};
            }
        }

        static LiteralOpt* jparse(json j)
        {
            std::vector<RegexChar> codes;
            auto jcodes = j[1]["charcodes"];
            std::transform(jcodes.cbegin(), jcodes.cend(), std::back_inserter(codes), [](const json& rv) {
                return rv.get<RegexChar>();
            });

            const bool isunicode = j[1]["isunicode"].get<bool>();

            return new LiteralOpt(codes, isunicode);
        }
    };

    class CharRangeOpt : public RegexOpt
    {
    public:
        const bool compliment;
        const std::vector<SingleCharRange> ranges;

        CharRangeOpt(bool compliment, std::vector<SingleCharRange> ranges) : RegexOpt(), compliment(compliment), ranges(ranges) {;}
        virtual ~CharRangeOpt() = default;

        virtual std::u8string toBSQONFormat() const override
        {
            std::u8string rngs = {'['};
            if(this->compliment) {
                rngs.push_back('^');
            }

            for(auto ii = this->ranges.cbegin(); ii != this->ranges.cend(); ++ii) {
                auto cr = *ii;

                auto lowbytes = escapeSingleRegexChar(cr.low);
                rngs.append(lowbytes.cbegin(), lowbytes.cend());

                if(cr.low != cr.high) {
                    rngs.push_back('-');
                    
                    auto highbytes = escapeSingleRegexChar(cr.high);
                    rngs.append(highbytes.cbegin(), highbytes.cend());
                }
            }
            rngs.push_back(']');

            return std::move(rngs);
        }

        static CharRangeOpt* jparse(json j)
        {
            const bool compliment = j[1]["compliment"].get<bool>();

            std::vector<SingleCharRange> ranges;
            auto jranges = j[1]["range"];
            std::transform(jranges.cbegin(), jranges.cend(), std::back_inserter(ranges), [](const json& rv) {
                auto lb = rv["lb"].get<RegexChar>();
                auto ub = rv["ub"].get<RegexChar>();

                return SingleCharRange{lb, ub};
            });

            return new CharRangeOpt(compliment, ranges);
        }
    };

    class CharClassDotOpt : public RegexOpt
    {
    public:
        CharClassDotOpt() : RegexOpt() {;}
        virtual ~CharClassDotOpt() = default;

        virtual std::u8string toBSQONFormat() const override
        {
            return std::u8string{'.'};
        }

        static CharClassDotOpt* jparse(json j)
        {
            return new CharClassDotOpt();
        }
    };


    class NamedRegexOpt : public RegexOpt
    {
    public:
        const std::string ns;
        const std::string rname;

        NamedRegexOpt(const std::string& ns, const std::string& rname) : RegexOpt(), ns(ns), rname(rname) {;}
        virtual ~NamedRegexOpt() = default;

        virtual std::u8string toBSQONFormat() const override
        {
            std::string fns = this->ns + "::" + this->rname;
            return std::u8string{'{'} + std::u8string(fns.cbegin(), fns.cend()) + std::u8string{'}'};
        }

        static NamedRegexOpt* jparse(json j)
        {
            const std::string ns = j[1]["ns"].get<std::string>();
            const std::string rname = j[1]["rname"].get<std::string>();

            return new NamedRegexOpt(ns, rname);
        }
    };

    class EnvRegexOpt : public RegexOpt
    {
    public:
        const std::string ename;

        EnvRegexOpt(const std::string& ename) : RegexOpt(), ename(ename) {;}
        virtual ~EnvRegexOpt() = default;

        virtual std::u8string toBSQONFormat() const override
        {
            return std::u8string{'{'} + std::u8string(this->ename.cbegin(), this->ename.cend()) + std::u8string{'}'};
        }

        static EnvRegexOpt* jparse(json j)
        {
            const std::string ename = j[1]["ename"].get<std::string>();

            return new EnvRegexOpt(ename);
        }
    };

    class NegateOpt : public RegexOpt
    {
    public:
        const RegexOpt* opt;

        NegateOpt(RegexOpt* opt) : RegexOpt(), opt(opt) {;}
        virtual ~NegateOpt() { delete this->opt; }

        virtual bool needsParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            if(!this->opt->needsParens()) {
                return std::u8string{'!'} + this->opt->toBSQONFormat();
            }
            else {
                return std::u8string{'!'} + std::u8string{'('} + this->opt->toBSQONFormat() + std::u8string{')'};
            }
        }

        static NegateOpt* jparse(json j)
        {
            auto opt = RegexOpt::jparse(j[1]["opt"]);
            return new NegateOpt(opt);
        }
    };

    class StarRepeatOpt : public RegexOpt
    {
    public:
        const RegexOpt* repeat;

        StarRepeatOpt(const RegexOpt* repeat) : RegexOpt(), repeat(repeat) {;}
        virtual ~StarRepeatOpt() { delete this->repeat; }

        virtual bool needsParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            if(!this->repeat->needsParens()) {
                return this->repeat->toBSQONFormat() + std::u8string{'*'};
            }
            else {
                return std::u8string{'('} + this->repeat->toBSQONFormat() + std::u8string{')'} + std::u8string{'*'};
            }
        }

        static StarRepeatOpt* parse(json j)
        {
            auto repeat = RegexOpt::jparse(j[1]["repeat"]);
            return new StarRepeatOpt(repeat);
        }
    };

    class PlusRepeatOpt : public RegexOpt
    {
    public:
        const RegexOpt* repeat;

        PlusRepeatOpt(const RegexOpt* repeat) : RegexOpt(), repeat(repeat) {;}
        virtual ~PlusRepeatOpt() { delete this->repeat; }

        virtual bool needsParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            if (!this->repeat->needsParens()) {
                return this->repeat->toBSQONFormat() + std::u8string{'+'};
            }
            else {
                return std::u8string{'('} + this->repeat->toBSQONFormat() + std::u8string{')'} + std::u8string{'+'};
            }
        }

        static PlusRepeatOpt* jparse(json j)
        {
            auto repeat = RegexOpt::jparse(j[1]["repeat"]);
            return new PlusRepeatOpt(repeat);
        }
    };

    class RangeRepeatOpt : public RegexOpt
    {
    public:
        const RegexOpt* repeat;
        const uint16_t low;
        const uint16_t high;

        RangeRepeatOpt(uint16_t low, uint16_t high, const RegexOpt* repeat) : RegexOpt(), repeat(repeat), low(low), high(high) {;}
        virtual ~RangeRepeatOpt() { delete this->repeat; }

        virtual bool needsParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            std::u8string repeatstr;
            if(!this->repeat->needsParens()) {
                repeatstr = this->repeat->toBSQONFormat();
            }
            else {
                repeatstr = std::u8string{'('} + this->repeat->toBSQONFormat() + std::u8string{')'};
            }

            std::string iterstr{'{'};
            if(this->low == this->high) {
                iterstr += std::to_string(this->low) + std::string{'}'};
            }
            else {
                if(this->low == 0) {
                    iterstr += std::string{','} + std::to_string(this->high) + std::string{'}'};
                }
                else if(this->high == INT16_MAX) {
                    iterstr += std::to_string(this->low) + std::string{','} + std::string{'}'};
                }
                else {
                    iterstr += std::to_string(this->low) + std::string{','} + std::to_string(this->high) + std::string{'}'};
                }   
            }

            return repeatstr + std::u8string(iterstr.cbegin(), iterstr.cend());
        }

        static RangeRepeatOpt* jparse(json j)
        {
            auto repeat = RegexOpt::jparse(j[1]["repeat"]);
            auto low = j[1]["low"].get<uint16_t>();
            auto high = j[1]["high"].is_null() ? INT16_MAX : j[1]["high"].get<uint16_t>();

            return new RangeRepeatOpt(low, high, repeat);
        }
    };

    class OptionalOpt : public RegexOpt
    {
    public:
        const RegexOpt* opt;

        OptionalOpt(const RegexOpt* opt) : RegexOpt(), opt(opt) {;}
        virtual ~OptionalOpt() { delete this->opt; }

        virtual bool needsParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            if (!this->opt->needsParens()) {
                return this->opt->toBSQONFormat() + std::u8string{'?'};
            }
            else {
                return std::u8string{'('} + this->opt->toBSQONFormat() + std::u8string{')'} + std::u8string{'?'};
            }
        }

        static OptionalOpt* jparse(json j)
        {
            auto opt = RegexOpt::jparse(j[1]["opt"]);
            return new OptionalOpt(opt);
        }
    };

    class AllOfOpt : public RegexOpt
    {
    public:
        const std::vector<const RegexOpt*> musts;

        AllOfOpt(std::vector<const RegexOpt*> AllOfOpt) : RegexOpt(), musts(musts) {;}

        virtual ~AllOfOpt()
        {
            for(size_t i = 0; i < this->musts.size(); ++i) {
                delete this->musts[i];
            }
        }

        virtual bool needsParens() const override { return true; }
        virtual bool needsSequenceParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            std::u8string muststr;
            for(auto ii = this->musts.cbegin(); ii != this->musts.cend(); ++ii) {
                if(ii != this->musts.cbegin()) {
                    muststr += std::u8string{'&'};
                }

                if(!(*ii)->needsParens()) {
                    muststr += (*ii)->toBSQONFormat();
                }
                else {
                    muststr += std::u8string{'('} + (*ii)->toBSQONFormat() + std::u8string{')'};
                }
            }
            
            return muststr;
        }

        static AllOfOpt* jparse(json j)
        {
            std::vector<const RegexOpt*> musts;
            auto jmusts = j[1]["musts"];
            std::transform(jmusts.cbegin(), jmusts.cend(), std::back_inserter(musts), [](json arg) {
                return RegexOpt::jparse(arg);
            });

            return new AllOfOpt(musts);
        }
    };

    class AnyOfOpt : public RegexOpt
    {
    public:
        const std::vector<const RegexOpt*> opts;

        AnyOfOpt(std::vector<const RegexOpt*> opts) : RegexOpt(), opts(opts) {;}

        virtual ~AnyOfOpt()
        {
            for(size_t i = 0; i < this->opts.size(); ++i) {
                delete this->opts[i];
            }
        }

        virtual bool needsParens() const override { return true; }
        virtual bool needsSequenceParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            std::u8string optstr;
            for(auto ii = this->opts.cbegin(); ii != this->opts.cend(); ++ii) {
                if(ii != this->opts.cbegin()) {
                    optstr += std::u8string{' ', '|', ' '};
                }

                if(!(*ii)->needsParens()) {
                    optstr += (*ii)->toBSQONFormat();
                }
                else {
                    optstr += std::u8string{'('} + (*ii)->toBSQONFormat() + std::u8string{')'};
                }
            }
            
            return optstr;
        }

        static AnyOfOpt* jparse(json j)
        {
            std::vector<const RegexOpt*> opts;
            auto jopts = j[1]["opts"];
            std::transform(jopts.cbegin(), jopts.cend(), std::back_inserter(opts), [](json arg) {
                return RegexOpt::jparse(arg);
            });

            return new AnyOfOpt(opts);
        }
    };

    class SequenceOpt : public RegexOpt
    {
    public:
        const std::vector<const RegexOpt*> regexs;

        SequenceOpt(std::vector<const RegexOpt*> regexs) : RegexOpt(), regexs(regexs) {;}

        virtual ~SequenceOpt()
        {
            for(size_t i = 0; i < this->regexs.size(); ++i) {
                delete this->regexs[i];
            }
        }

        virtual bool needsParens() const override { return true; }
        virtual std::u8string toBSQONFormat() const override
        {
            std::u8string regexstr;
            for(auto ii = this->regexs.cbegin(); ii != this->regexs.cend(); ++ii) {
                if(!(*ii)->needsSequenceParens()) {
                    regexstr += (*ii)->toBSQONFormat();
                }
                else {
                    regexstr += std::u8string{'('} + (*ii)->toBSQONFormat() + std::u8string{')'};
                }
            }
            
            return regexstr;
        }

        static SequenceOpt* jparse(json j)
        {
            std::vector<const RegexOpt*> regexs;
            auto jregexs = j[1]["regexs"];
            std::transform(jregexs.cbegin(), jregexs.cend(), std::back_inserter(regexs), [](json arg) {
                return RegexOpt::jparse(arg);
            });

            return new SequenceOpt(regexs);
        }
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