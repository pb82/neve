#include "value.hpp"

namespace JSON {

void Value::toLua(lua_State *L) {
    toLua(L, *this);
}

void Value::toBson(bson_t *doc) {
    bson_init(doc);

    switch(this->getType()) {
    case JSON_OBJECT:
        writeBsonObject(doc, *this);
        return;
    default:
        throw std::runtime_error("");
    }
}


void Value::toLua(lua_State *L, JSON::Value &val) {
    switch(val.getType()) {
    case JSON::JSON_NULL:
        lua_pushnil(L);
        break;
    case JSON::JSON_BOOL:
        lua_pushboolean(L, (int) val.as<bool>());
        break;
    case JSON::JSON_NUMBER:
        lua_pushnumber(L, val.as<double>());
        break;
    case JSON::JSON_STRING:
        lua_pushstring(L, val.as<std::string>().c_str());
        break;
    case JSON::JSON_OBJECT:
        writeLuaObject(L, val);
        break;
    case JSON::JSON_ARRAY:
        writeLuaArray(L, val);
        break;
    default:
        luaL_error(L, "toLua: unsupported type");
    }
}

void Value::fromLua(lua_State *L) {
    // Get the type of the item on top of the stack
    int type = lua_type(L, -1);
    switch(type) {
    case LUA_TTABLE:
        // -2 because readObject needs to push a nil value
        readLuaObject(L, *this, -2);
        break;
    case LUA_TBOOLEAN:
        *this = (bool) lua_toboolean(L, -1);
        break;
    case LUA_TNUMBER:
        *this = lua_tonumber(L, -1);
        break;
    case LUA_TSTRING:
        *this = lua_tostring(L, -1);
        break;
    case LUA_TNIL:
        *this = JSON::null;
        break;
    default:
        luaL_error(L, "fromLua: unsupported type");
    }

    // Pop the value after reading
    lua_pop(L, 1);
}

void Value::readLuaObject(lua_State *L, Value &obj, int tableindex) {
    lua_pushnil(L);
    while(lua_next(L, tableindex)) {
        std::string key;

        if (lua_isnumber(L, -2)) {
            key = toString(lua_tonumber(L, -2));
        } else if (lua_isstring(L, -2)) {
            key = lua_tostring(L, -2);
        } else {
            luaL_error(L, "Key must be a string or a number");
        }

        switch(lua_type(L, -1)) {
        case LUA_TNUMBER:
            obj[key] = lua_tonumber(L, -1);
            break;
        case LUA_TSTRING:
            obj[key] = lua_tostring(L, -1);
            break;
        case LUA_TBOOLEAN:
            obj[key] = (bool) lua_toboolean(L, -1);
            break;
        case LUA_TTABLE:
            {
                JSON::Value nested;
                // The nested table will be at position -2 (from
                // the top) because of the nil that we push initially
                readLuaObject(L, nested, -2);
                obj[key] = nested;
            }
            break;
        default:
            luaL_error(L, "readObject: unsupported type");
        }
        lua_pop(L, 1);
    }
}

void Value::writeLuaArray(lua_State *L, Value &val) {
    Array arr = val.as<Array>();

    lua_createtable(L, arr.size(), 0);

    for (unsigned int i = 1; i < arr.size() + 1; i++) {
        lua_pushnumber(L, i);
        toLua(L, arr[i - 1]);
        lua_settable(L, -3);
    }
}

void Value::writeLuaObject(lua_State *L, Value &val) {
    JSON::Object obj = val.as<JSON::Object>();

    lua_createtable(L, 0, obj.size());

    for (auto const& pair : obj) {
        std::string key = pair.first;
        JSON::Value value = pair.second;

        lua_pushstring(L, key.c_str());
        toLua(L, value);
        lua_settable(L, -3);
    }
}

void Value::writeBsonObject(bson_t *doc, Value &val) {
    JSON::Object obj = val.as<JSON::Object>();

    for (auto const& pair : obj) {
        std::string key = pair.first;
        JSON::Value value = pair.second;
        switch(value.getType()) {
        case JSON_NULL:
            bson_append_null(doc, key.c_str(), key.size());
            break;
        case JSON_BOOL:
            bson_append_bool(doc, key.c_str(), key.size(), value.as<bool>());
            break;
        case JSON_NUMBER:
            bson_append_double(doc, key.c_str(), key.size(), value.as<double>());
            break;
        case JSON_STRING:
            bson_append_utf8(doc, key.c_str(), key.size(),
                             value.as<std::string>().c_str(),
                             value.as<std::string>().size());
            break;
        case EXT_BINARY:
            bson_append_binary(doc, key.c_str(), key.size(), BSON_SUBTYPE_BINARY,
                               (const uint8_t *) value.as<std::string>().c_str(),
                               value.as<std::string>().size());
            break;
        case JSON_OBJECT:
            {
                bson_t obj;
                bson_append_document_begin(doc, key.c_str(), key.size(), &obj);
                writeBsonObject(&obj, value);
                bson_append_document_end(doc, &obj);
                break;
            }
        case JSON_ARRAY:
            {
                bson_t arr;
                // Convert the
                JSON::Value converted = val.as<JSON::Object>();
                bson_append_array(doc, key.c_str(), key.size(), &arr);
                writeBsonObject(doc, converted);
                bson_append_array_end(doc, &arr);
                break;
            }
        }
    }
}

} // End namespace
