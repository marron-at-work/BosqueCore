#include "bytestring.h"

ByteString* bstrAlloc(ByteString dst)
{
    ByteString* res = (ByteString*)malloc(sizeof(ByteString) + dst.len + 1);
    memset((char*)res, 0, sizeof(ByteString) + dst.len + 1);

    res->bytes = (uint8_t*)res + sizeof(ByteString);
    memcpy(res->bytes, dst.bytes, dst.len);
    res->len = dst.len;

    return res;
}
