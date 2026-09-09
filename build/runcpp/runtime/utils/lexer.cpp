#include "lexer.h"

namespace ᐸRuntimeᐳ 
{
    constexpr auto s_regexflags = boost::regex_constants::ECMAScript | boost::regex_constants::nosubs | boost::regex_constants::optimize;

    static boost::regex s_ws_re("^\\s+", s_regexflags);
    static boost::regex s_line_comment_re("^%%[^\\n]*", s_regexflags);

    static boost::regex s_nat_re("^(0|[+-]?[1-9][0-9]*)n", s_regexflags);
    static boost::regex s_int_re("^(0|[+-]?[1-9][0-9]*)i", s_regexflags);
    static boost::regex s_chknat_re("^(ChkNat::npos|((0|[+-]?[1-9][0-9]*)N))", s_regexflags);
    static boost::regex s_chkint_re("^(ChkInt::npos|((0|[+-]?[1-9][0-9]*)I))", s_regexflags);
    static boost::regex s_float_re("^[+-]?(0|[1-9][0-9]*)(\\.[0-9]+)([eE][+-]?[0-9]+)?f", s_regexflags);

    static boost::regex s_byte_re("^0x[0-9a-fA-F]{1,2}", s_regexflags);
    static boost::regex s_cchar_re("^c'[^']{1,16}'", s_regexflags);
    static boost::regex s_uchar_re("^c\"[^\"]{1,16}\"", s_regexflags);

    static boost::regex s_bytebuffer_prefix_re("^0x\\[", s_regexflags);
    static boost::regex s_bytebuffer_empty_re("^0x\\[\\]", s_regexflags);
    
    //constexpr std::array<char, 11> s_symbol_tokens = { '(', ')', '{', '}', '[', ']', '<', '>', ',', '#', '|' };
    
    constexpr std::array<const char*, 6> s_keyword_tokens = { "none", "true", "false", "some", "ok", "fail" };

    static boost::regex s_identifierlike_re("^([a-zA-Z_][a-zA-Z0-9_]*)", s_regexflags);

    bool BAPILexer::tryLexWS()
    {
        boost::match_results<IOBufferIterator> mm;
        if(!boost::regex_search(this->iter, this->end, mm, s_ws_re, boost::match_continuous)) {
            return false;
        }

        std::advance(this->iter, mm[0].length());
        return true;
    }

    bool BAPILexer::tryLexComment()
    {
        xxxx;
    }

    bool BAPILexer::tryLexNat()
    {
        xxxx;
    }

    bool BAPILexer::tryLexInt()
    {
        xxxx;
    }

    bool BAPILexer::tryLexChkNat()
    {
        xxxx;
    }

    bool BAPILexer::tryLexChkInt()
    {
        xxxx;
    }

    bool BAPILexer::tryLexFloat()
    {
        xxxx;
    }

    bool BAPILexer::tryLexByte()
    {
        xxxx;
    }

    bool BAPILexer::tryLexCChar()
    {
        xxxx;
    }

    bool BAPILexer::tryLexUnicodeChar()
    {
        xxxx;
    }

    bool BAPILexer::tryLexCString()
    {
        xxxx;
    }

    bool BAPILexer::tryLexString()
    {
        xxxx;
    }

    bool BAPILexer::tryLexByteBuffer()
    {
        xxxx;
    }

    bool BAPILexer::tryLexSymbol()
    {
        xxxx;
    }

    bool BAPILexer::tryLexIdentifierLike()
    {
        xxxx;
    }

    void BAPILexer::consume()
    {
        while(!this->iter->isEOF() && (this->tryLexWS() || this->tryLexComment())) {
            ;
        }

        if(this->iter->isEOF()) {
            this->ctoken.tokentype = BAPITokenType::EOFToken;
            return;
        }

        if(this->tryLexNat() || this->tryLexInt() || this->tryLexChkNat() || this->tryLexChkInt() || this->tryLexFloat()) {
            return;
        }
        else if(this->tryLexByte() || this->tryLexCChar() || this->tryLexUnicodeChar()) {
            return;
        }
        else if(this->tryLexCString() || this->tryLexString() || this->tryLexByteBuffer()) {
            return;
        }
        else if(this->tryLexSymbol() || this->tryLexIdentifierLike()) {
            return;
        }
        else {
            this->advanceToken(BAPITokenType::ErrorToken, 1);
            return;
        }
    }
}
