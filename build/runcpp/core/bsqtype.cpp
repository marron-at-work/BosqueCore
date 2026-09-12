#include "bsqtype.h"

namespace ᐸRuntimeᐳ
{
    ////////////////////////////////
    //Standard processing functions for Enum types
    ////////////////////////////////

    void jsonParseToBSQ_Enum(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        bsq_validate(j.is_string(), "JSON -> BSQ", 0, nullptr, "Expected string for enum type");

        std::string sstr = j.get<std::string>();
        size_t hashpos = sstr.find('#');
        bsq_validate(hashpos != std::string::npos, "JSON -> BSQ", 0, nullptr, "Expected '#' in enum string");
        std::string typekey = sstr.substr(0, hashpos);
        std::string member = sstr.substr(hashpos + 1);

        bsq_validate(typekey == tinfo->typekey, "JSON -> BSQ", 0, nullptr, "Enum type key mismatch");

        std::pair<size_t, const char**> members = TypeInfo::enuminfomap.at(tinfo->bsqtypeid);
        auto eiter = std::find_if(members.second, members.second + members.first, [member](const char* ev) { return member == ev; });

        bsq_validate(eiter != members.second + members.first, "JSON -> BSQ", 0, nullptr, "Expected valid enum member");
        uint64_t idx = (uint64_t)std::distance(members.second, eiter);

        void* args[1] = { &idx };
        tinfo->opdispatch.validatingConstructorFp(args, resptr);
    }

    void parseToBSQ_Enum(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        bsq_validate(lexer->testIsType(tinfo->typekey), "BAPI -> BSQ", 0, nullptr, "Expected identifier for enum type");
        lexer->consume();

        bsq_validate(lexer->getCurrentTokenType() == BAPITokenType::LiteralSymbol && lexer->testIsSymbol('#'), "BAPI -> BSQ", 0, nullptr, "Expected '#' symbol for enum type");
        lexer->consume();

        std::pair<size_t, const char**> members = TypeInfo::enuminfomap.at(tinfo->bsqtypeid);
        auto eiter = std::find_if(members.second, members.second + members.first, [lexer](const char* ev) { return lexer->testDataMatchesID(ev); });
    
        bsq_validate(eiter != members.second + members.first, "BAPI -> BSQ", 0, nullptr, "Expected valid enum member");
        uint64_t idx = (uint64_t)std::distance(members.second, eiter);
        
        void* args[1] = { &idx };
        tinfo->opdispatch.validatingConstructorFp(args, resptr);
    }

    json bsqToJSON_Enum(const TypeInfo* tinfo, const void* valptr)
    {
        uint64_t vv = *(const uint64_t*)valptr;
        std::pair<size_t, const char**> members = TypeInfo::enuminfomap.at(tinfo->bsqtypeid);

        return json(std::string(tinfo->typekey) + '#' + members.second[vv]);
    }
    
    void bsqToBAPI_Enum(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        uint64_t vv = *(const uint64_t*)valptr;
        std::pair<size_t, const char**> members = TypeInfo::enuminfomap.at(tinfo->bsqtypeid);

        builder->appendConstString(tinfo->typekey);
        builder->appendChar('#');
        builder->appendConstString(members.second[vv]);
    }

    void displayValue_Enum(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        uint64_t vv = *(const uint64_t*)valptr;
        std::pair<size_t, const char**> members = TypeInfo::enuminfomap.at(tinfo->bsqtypeid);

        os << getDisplayIndent(indent) << tinfo->typekey << '#' << members.second[vv];
    }

    ////////////////////////////////
    //Standard processing functions for Typedecl types
    ////////////////////////////////
    void jsonParseToBSQ_Typedecl(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        const TypeInfo* ofinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);

        void* val = alloca(ofinfo->bytesize);
        ofinfo->opdispatch.jsonParseToBSQFp(ofinfo, j, val);

        tinfo->opdispatch.validatingConstructorFp(&val, resptr);
    }

    void parseToBSQ_Typedecl(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        const TypeInfo* ofinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);

        void* val = alloca(ofinfo->bytesize);
        ofinfo->opdispatch.parseToBSQFp(ofinfo, lexer, val);

        tinfo->opdispatch.validatingConstructorFp(&val, resptr);

        if(lexer->testIsSymbol('<')) {
            lexer->consume();
    
            bsq_validate(lexer->testIsType(tinfo->typekey), "BAPI -> BSQ", 0, nullptr, "Expected type for typedecl");
            lexer->consume();
            bsq_validate(lexer->testIsSymbol('>'), "BAPI -> BSQ", 0, nullptr, "Expected symbol '>'");
            lexer->consume();
        }
    }

    json bsqToJSON_Typedecl(const TypeInfo* tinfo, const void* valptr)
    {
        const TypeInfo* ofinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);
        return ofinfo->opdispatch.bsqToJSONFp(ofinfo, valptr);
    }

    void bsqToBAPI_Typedecl(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        const TypeInfo* ofinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);
        ofinfo->opdispatch.bsqToBAPIFp(ofinfo, valptr, builder);

        builder->appendChar('<');
        builder->appendConstString(tinfo->typekey);
        builder->appendChar('>');
    }

    void displayValue_Typedecl(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        const TypeInfo* ofinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);

        os << getDisplayIndent(indent);
        ofinfo->opdispatch.displayFp(ofinfo, valptr, os, indent);
        os << '<' << tinfo->typekey << '>';
    }
}
