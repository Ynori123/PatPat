#include "minijson.h"
#include <cctype>

namespace minijson{

struct Lexer {
    const char* p;
    const char* end;

    explicit Lexer(const std::string& s) : p(s.c_str()), end(s.c_str() + s.size()) {}

    void skipWS(){
        while(p < end && std::isspace(static_cast<unsigned char>(*p)))
            ++p;
    }

    bool match(char c){
        skipWS();
        if(p < end && *p == c){
            ++p;
            return true;
        }
        return false;
    }

    bool peek(char c){
        skipWS();
        return(p < end && *p == c);
    }

    bool eof() const {
        const char* q = p;
        while(q < end && std::isspace(static_cast<unsigned char>(*q)))
            ++q;
        return q >= end;
    }
};

static bool parseString(Lexer& lx, std::string& out, std::string& err){
    lx.skipWS();
    if(lx.p >= lx.end || *lx.p != '"'){
        err = "Expected '\"' at beginning of string";
        return false;
    }
    ++lx.p;
    std::string s;
    while(lx.p < lx.end){
        char c = *lx.p++;
        if(c == '"') {out = std::move(s); return true;}
        if(c == '\\') {
            if(lx.p >= lx.end) {
                err = "bad escape";
                return false;
            }
            char e = *lx.p++;
            switch(e){
                case '"': s.push_back('"'); break;
                case '\\': s.push_back('\\'); break;
                case '/': s.push_back('/'); break;
                case 'b': s.push_back('\b'); break;
                case 'f': s.push_back('\f'); break;
                case 'n': s.push_back('\n'); break;
                case 'r': s.push_back('\r'); break;
                case 't': s.push_back('\t'); break;
                case 'u':{
                    for(int i = 0; i < 4 && lx.p < lx.end; i++){
                        ++lx.p;
                    }
                    s.push_back('?'); // placeholder for unicode
                    break;
                }
                default: s.push_back(e); break;
            }
        } else{
            s.push_back(c);
        }
    }
    err = "Unterminated string";
    return false;
}

static bool parseNumber(Lexer& lx, double& out, std::string& err){
    lx.skipWS();
    const char* start = lx.p;
    if(lx.p < lx.end && (*lx.p == '-' || *lx.p == '+')) ++lx.p;
    while(lx.p < lx.end && std::isdigit(static_cast<unsigned char>(*lx.p))) ++lx.p;
    if(lx.p < lx.end && *lx.p == '.'){
        ++lx.p;
        while(lx.p < lx.end && std::isdigit(static_cast<unsigned char>(*lx.p))) ++lx.p;
    }
    if(lx.p < lx.end && (*lx.p == 'e' || *lx.p == 'E')){
        ++lx.p;
        if(lx.p < lx.end && (*lx.p == '-' || *lx.p == '+')) ++lx.p;
        while(lx.p < lx.end && std::isdigit(static_cast<unsigned char>(*lx.p))) ++lx.p;
    }
    if(start == lx.p){
        err = "expected number";
        return false;
    }
    try{
        out = std::stod(std::string(start, lx.p));
    } catch(...){
        err = "Invalid number format";
        return false;
    }
    return true;
}

static bool parseValue(Lexer& lx, Value& out, std::string& err);

static bool parseArray(Lexer& lx, Value& v, std::string& err){
    if(!lx.match('[]')) {
        err = "Expected '[' at beginning of array";
        return false;
    }
    v.type = Value::Type::Array;
    v.a.clear();
    lx.skipWS();
    if(lx.peek(']')){
        lx.match(']');
        return true;
    }
    while(true){
        Value elem;
        if(!parseValue(lx, elem, err)) return false;
        v.a.push_back(std::move(elem));
        lx.skipWS();
        if(lx.match(']')) break;
        if(!lx.match(',')){
            err = "Expected ',' or ']' in array";
            return false;
        }
    }
    return true;
}

static bool parseObject(Lexer& lx, Value& v, std::string& err){
    if(!lx.match('{')){
        err = "Expected '{' at beginning of object";
        return false;
    }
    v.type = Value::Type::Object;
    v.o.clear();
    lx.skipWS();
    if(lx.peek('}')){
        lx.match('}');
        return true;
    }
    while(true){
        std::string key;
        if(!parseString(lx, key, err))  return false;
        if(!lx.match(':')){
            err = "Expected ':' after key in object";
            return false;
        }
        Value val;
        if(!parseValue(lx, val, err)) return false;
        v.o.emplace(std::move(key), std::move(val));
        lx.skipWS();
        if(lx.match('}')) break;
        if(!lx.match(',')){
            err = "Expected ',' or '}' in object";
            return false;
        }
    }
    return true;
}

static bool parseLiteral(Lexer& lx, const char* lit){
    while(*lit){
        if(lx.p >= lx.end || *lx.p != *lit) return false;
        ++lx.p;
        ++lit;
    }
    return true;
}

static bool parseValue(Lexer& lx, Value& v, std::string& err){
    lx.skipWS();
    if(lx.p >= lx.end){
        err = "Unexpected end of input";
        return false;
    }
    char c = *lx.p;
    if(c == '{') return parseObject(lx, v, err);
    if(c == '[') return parseArray(lx, v, err);
    if(c == '"') {
        v.type = Value::Type::String;
        return parseString(lx, v.s, err);
    }
    if(std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+'){
        v.type = Value::Type::Number;
        return parseNumber(lx, v.n, err);
    }
    if(c == 't'){
        if(parseLiteral(lx, "true")){
            v.type = Value::Type::Bool;
            v.b = true;
            return true;
        }
    }
    if(c == 'f'){
        if(parseLiteral(lx, "false")){
            v.type = Value::Type::Bool;
            v.b = false;
            return true;
        }
    }
    if(c == 'n'){
        if(parseLiteral(lx, "null")){
            v.type = Value::Type::Null;
            return true;
        }
    }
    err = "Invalid value";
    return false;
}

ParseResult parse(const std::string& text){
    ParseResult r;
    Lexer lx(text);
    if(!parseValue(lx, r.root, r.error)){
        r.ok = false;
        return r;
    }
    if(!lx.eof()){
        r.ok = false;
        r.error = "Extra data after valid JSON";
        return r;
    }
    r.ok = true;
    return r;
};

}   // namespace minijson