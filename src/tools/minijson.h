#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <optional>

// A minimal JSON parser and serializer for C++

namespace minijson {

struct Value;

using Object = std::unordered_map<std::string, Value>;
using Array = std::vector<Value>;

struct Value {
    enum class Type {Null, Bool, Number, String, Object, Array} type = Type::Null;
    bool b = false;
    double n = 0.0;
    std::string s;
    Array a;
    Object o;

    bool isNull() const {return type == Type::Null;}
    bool isBool() const {return type == Type::Bool;}
    bool isNumber() const {return type == Type::Number;}
    bool isString() const {return type == Type::String;}
    bool isObject() const {return type == Type::Object;}
    bool isArray() const {return type == Type::Array;}

    // Helpers to get value with type checking
    const Value* get(const std::string& key) const{
        if(!isObject()) return nullptr;
        auto it = o.find(key);
        return it == o.end() ? nullptr : &it->second;
    }
    bool getBool(const std::string& key, bool def = false) const{
        if(auto v = get(key); v && v->isBool()) return v->b;
        return def;
    }
    int getInt(const std::string& key, int def = 0) const{
        if(auto v = get(key); v && v->isNumber()) return static_cast<int>(v->n);
        return def;
    }
    double getNumber(const std::string& key, double def = 0.0) const{
        if(auto v = get(key); v && v->isNumber()) return v->n;
        return def;
    }
    std::string getString(const std::string& key, const std::string& def = "") const{
        if(auto v = get(key); v && v->isString()) return v->s;
        return def;
    }
    const Array* getArray(const std::string& key) const{
        if(auto v = get(key); v && v->isArray()) return &v->a;
        return nullptr;
    }
    const Object* getObject(const std::string& key) const{
        if(auto v = get(key); v && v->isObject()) return &v->o;
        return nullptr;
    }
};

struct ParseResult {
    bool ok = false;
    std::string error;
    Value root;
};

ParseResult parse(const std::string& jsonText);

}   // namespace minijson