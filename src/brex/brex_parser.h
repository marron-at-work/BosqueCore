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

        inline bool isEOS() const
        {
            return this->cpos == this->epos;
        }

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
