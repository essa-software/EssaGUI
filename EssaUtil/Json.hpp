#pragma once

#include <EssaUtil/Config.hpp>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace JSON {
class Object;
class Value;

template<class T> class PrimitiveWrapper {
private:
    T val;

public:
    PrimitiveWrapper(T val = 0)
        : val(val) { }

    operator T&() { return val; }
    T* operator&() { return &val; }
};

using Number = PrimitiveWrapper<double>;

class Array : public std::vector<Value> {
public:
    Array() = default;
    Array(std::initializer_list<Value> const& _list);
};

class Object : public std::map<std::string, Value> {
public:
    Object() = default;
    Object(std::initializer_list<std::pair<std::string, Value>> _list);

    template<typename T> Object& add_value(std::string const& key, T const& val);

    Object& add_value(std::string const& key, Value const& val);

    std::string stringify() const;

    Value const& operator[](std::string const& key) const;
    std::optional<std::reference_wrapper<Value const>> get(std::string const& key) const;
};

class Value {
    using Type = std::variant<Number, bool, std::string, Object, Array>;
    Type m_data;
    static Value parse_value(std::string str);
    static Array parse_array(std::string str);

public:
    template<typename T> Value(T const& val) { m_data = val; }

    template<typename T> bool is_type_of() const { return std::holds_alternative<T>(m_data); }

    template<typename T> T const& get() const { return std::get<T>(m_data); }

    static Object parse_object(std::string str);
    std::string stringify() const;
};

inline Value const& Object::operator[](std::string const& key) const {
    auto it = find(key);
    if (it == end()) {
        ESSA_UNREACHABLE;
    }
    return it->second;
}

inline std::optional<std::reference_wrapper<Value const>> Object::get(std::string const& key) const {
    auto it = find(key);
    if (it == end()) {
        return std::nullopt;
    }
    return it->second;
}

inline Array::Array(std::initializer_list<Value> const& _list) {
    for (auto const& n : _list) {
        push_back(n);
    }
}

inline std::string Value::stringify() const {
    std::stringstream ss;
    if (is_type_of<Number>()) {
        Number v = get<Number>();
        ss << v;
    }
    else if (is_type_of<bool>()) {
        bool v = get<bool>();
        ss << (v ? "true" : "false");
    }
    else if (is_type_of<std::string>()) {
        std::string v = get<std::string>();
        ss << "\"" << v << "\"";
    }
    else if (is_type_of<Object>()) {
        Object v = get<Object>();
        ss << v.stringify();
    }
    else if (is_type_of<Array>()) {
        Array v = get<Array>();
        ss << "[";
        int i = 0;
        for (auto const& n : v) {
            ss << n.stringify();
            if (i < (int)v.size() - 1) {
                ss << ",";
            }
            i++;
        }
        ss << "]";
    }
    return ss.str();
}

inline Object::Object(std::initializer_list<std::pair<std::string, Value>> _list) {
    for (auto const& p : _list) {
        insert(p);
    }
}

inline std::string Object::stringify() const {
    std::stringstream ss;
    ss << "{";
    int it = 0;

    for (auto const& p : *this) {
        ss << "\"" << p.first << "\":" << p.second.stringify();
        if (it < (int)size() - 1) {
            ss << ",";
        }
        it++;
    }

    ss << "}";

    return ss.str();
}

template<typename T> Object& Object::add_value(std::string const& key, T const& val) {
    insert({ key, val });

    return *this;
}

inline Object& Object::add_value(std::string const& key, Value const& val) {
    insert({ key, val });

    return *this;
}

inline Array Value::parse_array(std::string str) {
    if (!str.starts_with('[') || !str.ends_with(']')) {
        throw std::runtime_error("JSON array not limited!");
    }

    Array result;

    str = str.substr(1, str.size() - 2);
    bool escape = false;
    bool read_str = false;
    bool read_arr = false;
    std::string val_str;

    for (auto const& c : str) {
        if (read_arr) {
            val_str += c;
        }

        if (c == '[' || c == '{') {
            read_arr = true;
        }
        else if (c == ']' || c == '}') {
            read_arr = false;

            continue;
        }

        if (read_arr)
            continue;

        if (c == ' ' && !read_str)
            continue;
        if (c == '\\' && read_str) {
            escape = true;
            continue;
        }
        if (c == '\"' && !escape) {
            read_str = !read_str;
            val_str += c;

            continue;
        }
        if (c == ',') {
            Value obj = Value::parse_value(val_str);
            result.push_back(obj);

            val_str.clear();
            continue;
        }
        val_str += c;

        escape = false;
    }

    Value obj = Value::parse_value(val_str);
    result.push_back(obj);

    return result;
}

inline Value Value::parse_value(std::string str) {
    if (str.starts_with('{')) {
        Value result(Value::parse_object(str));
        return result;
    }
    if (str.starts_with('[')) {
        Value result(Value::parse_array(str));
        return result;
    }
    if (str.starts_with('\"')) {
        if (!str.ends_with('\"')) {
            throw std::runtime_error("Missing \'\"\' to close JSON string!");
        }
        Value result(str.substr(1, str.size() - 2));
        return result;
    }
    if (str == "true" || str == "false") {
        Value result(str == "true" ? true : false);
        return result;
    }
    try {
        Value result(std::stod(str));
        return result;
    } catch (...) {
        throw std::runtime_error("Unrecognized JSON value!");
    }
}

inline Object Value::parse_object(std::string str) {
    if (!str.starts_with('{') || !str.ends_with('}')) {
        throw std::runtime_error("JSON object not limited!");
    }

    str = str.substr(1, str.size() - 2);
    bool escape = false;
    bool key = true;
    bool val = false;
    bool read_str = false;
    bool read_arr = false;
    std::string key_str;
    std::string val_str;

    Object result;

    for (auto const& c : str) {
        if (read_arr) {
            val_str += c;
        }

        if (c == '[' || c == '{') {
            read_arr = true;
            val_str += c;
        }
        else if (c == ']' || c == '}') {
            read_arr = false;
            val_str += c;

            continue;
        }

        if (read_arr)
            continue;

        if (c == ' ' && !read_str)
            continue;
        if (c == '\\' && read_str) {
            escape = true;
            continue;
        }
        if (c == '\"' && !escape) {
            read_str = !read_str;

            // if(key){
            //     key_str += c;
            // }else if(val){
            //     val_str += c;
            // }

            continue;
        }
        if (c == ':') {
            if (!key)
                throw std::runtime_error("Missing key!");
            key = false;
            val = true;
            continue;
        }
        if (c == ',') {
            if (!val)
                throw std::runtime_error("Missing value!");
            key = true;
            val = false;
            Value obj = Value::parse_value(val_str);
            result.add_value(key_str, obj);

            key_str.clear();
            val_str.clear();
            continue;
        }

        if (key) {
            key_str += c;
        }
        else if (val) {
            val_str += c;
        }

        escape = false;
    }
    Value obj = Value::parse_value(val_str);
    result.add_value(key_str, obj);

    return result;
}

inline Object Marshall(std::string const& str) { return Value::parse_object(str); }

}
