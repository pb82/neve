#ifndef VALUE_H
#define VALUE_H

#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <cmath>
#include <map>

#include <lua.h>

namespace JSON {
	enum JsonType {
		JSON_STRING = 0,
		JSON_NUMBER = 1,
		JSON_BOOL =   2,
		JSON_ARRAY =  3,
		JSON_OBJECT = 4,
		JSON_NULL
	};

    // Forward declaration needed for typedefs.
    struct Value;
    
    // JSON Objects and Arrays are actually only typedef'd
    // std maps and vectors.
    typedef std::vector<Value>            Array;
    typedef std::map<std::string, Value>  Object;
    
	// Convert x to String
	template<typename T> std::string toString(const T& t) {
		std::ostringstream stream;
		stream << t;
		return stream.str();
	}

	// Convert String to x
	template<typename T> T fromString(const std::string& s) {
		std::istringstream stream (s);
		T t;
		stream >> t;
		return t;
	}

    // A JSON::Value may represent every possible JSON type.
    struct Value {
        // Construction with no argument is interpreted as
        // JSON null.
        Value() 
        : type(JSON_NULL) { 
        }
        
        // JSON_NUMBER
        Value(int val) 
        : type(JSON_NUMBER) {
            std::get<JSON_NUMBER>(value) = val;
        }
    
        Value(long int val) 
        : type(JSON_NUMBER) {
            std::get<JSON_NUMBER>(value) = (int) val;
        }
    
        Value(unsigned int val) 
        : type(JSON_NUMBER) {
            std::get<JSON_NUMBER>(value) = (int) val;
        }
    
        Value(double val) 
        : type(JSON_NUMBER) {
            std::get<JSON_NUMBER>(value) = val;
        }

        // JSON_STRING
        Value(const char * val)
        : type(JSON_STRING) {
            std::get<JSON_STRING>(value) = std::string(val);
        }
    
        Value(const std::string& val)
        : type(JSON_STRING) {
            std::get<JSON_STRING>(value) = val;
        }

        // JSON_BOOL
        Value(bool val)
        : type(JSON_BOOL) {
            std::get<JSON_BOOL>(value) = val;
        }

        // JSON_ARRAY
        Value(const Array& val) 
        : type(JSON_ARRAY) {
            std::get<JSON_ARRAY>(value) = val;
        }

        // Array construction from initializer list
        // Value a {1, 2, 3};
        Value(std::initializer_list<Value> val) 
        : type(JSON_ARRAY) {
            std::get<JSON_ARRAY>(value) = Array(val);
        }

        // JSON_OBJECT
        Value(const Object& val) 
        : type(JSON_OBJECT) {
            std::get<JSON_OBJECT>(value) = val;
        }
        
        // Access and construction by [] operator
        Value& operator[](const std::string& key) {
            // This may also be used for construction so
            // ensure that the value is object type.
            type = JSON_OBJECT;
            return std::get<JSON_OBJECT>(value)[key];
        }

        // Array access and manipulation
        Value& operator[](int index) {
            return std::get<JSON_ARRAY>(value)[index];
        }

        bool is(JsonType type) const {
            return this->type == type;
        }

        JsonType getType() const {
            return type;
        }

        void push_back(const Value& val) {
            std::get<JSON_ARRAY>(value).push_back(val);
        }

        // Value access (and conversion)
		template <typename T> T as() const;
		template <typename T> T& asMutable();
    private:
        // The actual type of the value.
        JsonType type;
        
        // The actual value is stored in the appropriate slot
        // of the tuple.
        std::tuple<
            std::string,
            double,
            bool,
            Array,
            Object
        > value;
    };
    
    // Null value in literals:
    // Value val = {1,null,2};
    static Value null;

	template<> inline Object& Value::asMutable() {
		return std::get<JSON_OBJECT>(value);
	}

	template<> inline Array& Value::asMutable() {
		return std::get<JSON_ARRAY>(value);
	}

	// Template specializations for Value::as
	// JSON_NUMBER
	template <> inline double Value::as() const {
		switch(type) {
		case JSON_NUMBER:
			// Number -> Number
			return std::get<JSON_NUMBER>(value);
		case JSON_STRING:
			// String -> Number
			return fromString<double>(std::get<JSON_STRING>(value));
		case JSON_BOOL:
			// Bool -> Number
			return std::get<JSON_BOOL>(value) ? 1 : 0;
		default:
			throw(std::runtime_error("Error converting value to double"));
		}
	}

    // JSON_NUMBER
    // Simply cast to other numeric types
	template <> inline int Value::as() const {
		return (int) as<double>();
	}
    
	template <> inline unsigned int Value::as() const {
		return (unsigned int) std::abs(as<double>());
	}
    
	template <> inline long Value::as() const {
		return (long) as<double>();
	}

    // JSON_STRING
	template <> inline std::string Value::as() const {
        switch(type) {
        case JSON_STRING:
            // String -> String
            return std::get<JSON_STRING>(value);
        case JSON_NUMBER:
            // Number -> String
            return toString<double>(std::get<JSON_NUMBER>(value));
        case JSON_BOOL:
            // Bool -> String
            return std::get<JSON_BOOL>(value) ? "true" : "false";
        case JSON_NULL:
            // Null -> String
			return "null";
        default:
			throw(std::runtime_error("Error converting value to string"));
        }
    }
    
    // JSON_BOOL
	template <> inline bool Value::as() const {
        switch(type) {
        case JSON_BOOL:
            // Bool -> Bool
            return std::get<JSON_BOOL>(value);
        case JSON_NUMBER:
            // Number -> Bool
            // Interpret everything < 0 as false otherwise as true
            return std::get<JSON_NUMBER>(value) < 0 ? false : true;
        default:
			throw(std::runtime_error("Error converting value to boolean"));
        }
    }   
    
    // JSON_ARRAY
	template <> inline Array Value::as() const {
        switch(type) {
        case JSON_ARRAY:
            // Array -> Array
            return std::get<JSON_ARRAY>(value);
        default:
			throw(std::runtime_error("Error converting value to array"));
        }
    }
    
    // JSON_OBJECT
	template <> inline Object Value::as() const {
        switch(type) {
        case JSON_OBJECT:
            // Object -> Object
            return std::get<JSON_OBJECT>(value);
        default:
			throw(std::runtime_error("Error converting value to object"));
        }
    }    
}

#endif // VALUE_H
