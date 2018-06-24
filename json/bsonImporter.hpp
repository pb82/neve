#ifndef BSON_IMPORTER_H
#define BSON_IMPORTER_H

#include <libbson-1.0/bson.h>

#include "value.hpp"

#define GET_THIS(name) BsonImporter *name = static_cast<BsonImporter *>(data);

namespace JSON {
    class BsonImporter {
    public:
        BsonImporter(bson_iter_t *source, Value *target);

    private:
        static bool onUtf8(const bson_iter_t *iter,
                    const char *key,
                    size_t v_utf8_len,
                    const char *v_utf8,
                    void *data);

        static bool onNull(const bson_iter_t *iter, const char *key, void *data);

        static bool onBool(const bson_iter_t *iter,
                           const char *key,
                           bool v_bool,
                           void *data);

        static bool onDouble(const bson_iter_t *iter,
                        const char *key,
                        double v_double,
                        void *data);

        static bool onInt32(const bson_iter_t *iter,
                            const char *key,
                            int32_t v_int32,
                            void *data);

        static bool onInt64(const bson_iter_t *iter,
                            const char *key,
                            int64_t v_int64,
                            void *data);

        static bool onBinary(const bson_iter_t *iter,
                             const char *key,
                             bson_subtype_t v_subtype,
                             size_t v_binary_len,
                             const uint8_t *v_binary,
                             void *data);

        bson_iter_t *source;
        Value *target;
    };
}

#endif // BSON_IMPORTER_H
