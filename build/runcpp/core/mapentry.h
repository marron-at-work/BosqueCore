#pragma once

#include "../common.h"

#include "bsqtype.h"

namespace ᐸRuntimeᐳ
{
    template<typename K, typename V>
    class XMapEntry
    {
    public:
        K key;
        V value;

        XMapEntry() = default;
        XMapEntry(const K &k, const V &v) : key{k}, value{v} {}
    };

    template<typename K, typename V>
    void jsonParseToBSQ_MapEntry(const TypeInfo* tinfo, const json& j, void* resptr)
    {
        bsq_validate(j.is_array() && j.size() == 2, "JSON -> BSQ", 0, nullptr, "Expected JSON array of size 2 for MapEntry<K, V>");

        K k;
        const TypeInfo* kinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);
        kinfo->opdispatch.jsonParseToBSQFp(kinfo, j[0], &k);

        V v;
        const TypeInfo* vinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[1].fieldbsqtypeid);
        vinfo->opdispatch.jsonParseToBSQFp(vinfo, j[1], &v);

        *(XMapEntry<K, V>*)resptr = XMapEntry<K, V>{k, v};
    }

    template<typename K, typename V>
    void parseToBSQ_MapEntry(const TypeInfo* tinfo, BAPILexer* lexer, void* resptr)
    {
        xxxx;
    }

    template<typename K, typename V>
    json bsqToJSON_MapEntry(const TypeInfo* tinfo, const void* valptr)
    {
        const XMapEntry<K, V>* entry = (const XMapEntry<K, V>*)valptr;

        const TypeInfo* kinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[0].fieldbsqtypeid);
        json jk = kinfo->opdispatch.bsqToJSONFp(kinfo, &entry->key);

        const TypeInfo* vinfo = TypeInfo::getTypeInfoForID(tinfo->ftable[1].fieldbsqtypeid);
        json jv = vinfo->opdispatch.bsqToJSONFp(vinfo, &entry->value);

        j.push_back(jk);
        j.push_back(jv);
        return j;
    }

    template<typename K, typename V>
    void bsqToBAPI_MapEntry(const TypeInfo* tinfo, const void* valptr, BSQStreamingBuilder* builder)
    {
        xxxx;
    }

    template<typename K, typename V>
    void displayValue_MapEntry(const TypeInfo* tinfo, const void* valptr, std::ostream& os, std::optional<std::string> indent)
    {
        xxxx;
    }

    template<typename K, typename V>
    consteval TypeInfo g_typeinfo_MapEntry_generate(uint32_t id, const TypeLayoutInfo* layout, const char* mask, const char* name) 
    {
        return TypeInfo{
            id,
            sizeof(XMapEntry<K, V>),
            byteSizeToSlotCount(sizeof(XMapEntry<K, V>)),
            LayoutTag::Value,
            mask,
            nullptr,
            0,
            layout,
            2,
            nullptr,
            0,
            name,
            false
        };
    }
}
