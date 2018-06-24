#include "bsonImporter.hpp"

namespace JSON {

bool BsonImporter::onUtf8(const bson_iter_t *iter, const char *key, size_t v_utf8_len,
                          const char *v_utf8, void *data) {

    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = std::string(v_utf8, v_utf8_len);
    return false;
}

bool BsonImporter::onNull(const bson_iter_t *iter, const char *key, void *data) {
    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = JSON::null;
    return false;
}

bool BsonImporter::onBool(const bson_iter_t *iter, const char *key, bool v_bool,
                          void *data) {
    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = v_bool;
    return false;
}

bool BsonImporter::onDouble(const bson_iter_t *iter, const char *key, double v_double,
                            void *data) {
    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = v_double;
    return false;
}

bool BsonImporter::onInt32(const bson_iter_t *iter, const char *key, int32_t v_int32,
                           void *data) {
    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = v_int32;
    return false;
}

bool BsonImporter::onInt64(const bson_iter_t *iter, const char *key, int64_t v_int64,
                           void *data) {
    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = v_int64;
    return false;
}

bool BsonImporter::onBinary(const bson_iter_t *iter, const char *key,
                            bson_subtype_t v_subtype, size_t v_binary_len,
                            const uint8_t *v_binary, void *data) {
    GET_THIS(This);
    std::string keyString(key);
    (*This->target)[keyString] = JSON::Value((const char *) v_binary, v_binary_len);
    return false;
}

BsonImporter::BsonImporter(bson_iter_t *source, Value *target)
    : source(source), target(target) {
    bson_visitor_t visitor = {0};
    visitor.visit_utf8 = onUtf8;
    visitor.visit_null = onNull;
    visitor.visit_bool = onBool;
    visitor.visit_double = onDouble;
    visitor.visit_int32 = onInt32;
    visitor.visit_int64 = onInt64;
    visitor.visit_binary = onBinary;

    bson_iter_visit_all(source, &visitor, this);
}



}
