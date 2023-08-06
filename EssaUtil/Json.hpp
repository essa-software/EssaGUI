#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>
namespace JSON {
class Node;
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

class Array : public std::vector<Node> {
public:
    Array() = default;
    Array(std::initializer_list<Value> const& _list);
};

class Node {
    std::map<std::string, Value> m_data;
    std::shared_ptr<Value> m_value;

    std::string stringify_value(Value const& val) const;

public:
    Node() = default;
    Node(std::initializer_list<std::pair<std::string, Value>> _list);

    Node(Value val);

    template<typename T> Node& add_value(std::string const& key, T const& val);

    Node& add_value(std::string const& key, Value const& val);

    std::string stringify() const;
};

class Value {
    using Type = std::variant<Number, bool, std::string, Node, Array>;
    Type m_data;
    static Value parse_value(std::string str);
    static Array parse_array(std::string str);

public:
    template<typename T> Value(T const& val) { m_data = val; }

    template<typename T> bool is_type_of() const { return std::holds_alternative<T>(m_data); }

    template<typename T> T get() const { return std::get<T>(m_data); }

    static Node parse_node(std::string str);
};

inline Array::Array(std::initializer_list<Value> const& _list) {
    for (auto const& n : _list) {
        push_back(n);
    }
}

inline std::string Node::stringify_value(Value const& val) const {
    std::stringstream ss;
    if (val.is_type_of<Number>()) {
        Number v = val.get<Number>();
        ss << v;
    }
    else if (val.is_type_of<bool>()) {
        bool v = val.get<bool>();
        ss << (v ? "true" : "false");
    }
    else if (val.is_type_of<std::string>()) {
        std::string v = val.get<std::string>();
        ss << "\"" << v << "\"";
    }
    else if (val.is_type_of<Node>()) {
        Node v = val.get<Node>();
        ss << v.stringify();
    }
    else if (val.is_type_of<Array>()) {
        Array v = val.get<Array>();
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

inline Node::Node(std::initializer_list<std::pair<std::string, Value>> _list) {
    for (auto const& p : _list) {
        m_data.insert(p);
    }
}

inline Node::Node(Value val) { m_value = std::make_shared<Value>(val); }

inline std::string Node::stringify() const {
    std::stringstream ss;
    if (m_value) {
        ss << stringify_value(*m_value);
    }
    else {
        ss << "{";
        int it = 0;

        for (auto const& p : m_data) {
            ss << "\"" << p.first << "\":" << stringify_value(p.second);
            if (it < (int)m_data.size() - 1) {
                ss << ",";
            }
            it++;
        }

        ss << "}";
    }

    return ss.str();
}

template<typename T> Node& Node::add_value(std::string const& key, T const& val) {
    m_data.insert({ key, val });

    return *this;
}

inline Node& Node::add_value(std::string const& key, Value const& val) {
    m_data.insert({ key, val });

    return *this;
}

inline Array Value::parse_array(std::string str) {
    if (!str.starts_with('[') || !str.ends_with(']')) {
        throw std::runtime_error("JSON array not limited!");
    }

    Array result;

    str = str.substr(1, str.size() - 2);
    bool escape = false, read_str = false, read_arr = false;
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
        else if (c == '\\' && read_str) {
            escape = true;
            continue;
        }
        else if (c == '\"' && !escape) {
            read_str = !read_str;
            val_str += c;

            continue;
        }
        else if (c == ',') {
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
        Value result(Value::parse_node(str));
        return result;
    }
    else if (str.starts_with('[')) {
        Value result(Value::parse_array(str));
        return result;
    }
    else if (str.starts_with('\"')) {
        if (!str.ends_with('\"')) {
            throw std::runtime_error("Missing \'\"\' to close JSON string!");
        }
        Value result(str.substr(1, str.size() - 2));
        return result;
    }
    else if (str == "true" || str == "false") {
        Value result(str == "true" ? true : false);
        return result;
    }
    else {
        try {
            Value result(std::stod(str));
            return result;
        } catch (...) {
            throw std::runtime_error("Unrecognized JSON value!");
        }
    }
}

inline Node Value::parse_node(std::string str) {
    if (!str.starts_with('{') || !str.ends_with('}')) {
        throw std::runtime_error("JSON node not limited!");
    }

    str = str.substr(1, str.size() - 2);
    bool escape = false, key = true, val = false, read_str = false, read_arr = false;
    std::string key_str, val_str;

    Node result;

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
        else if (c == '\\' && read_str) {
            escape = true;
            continue;
        }
        else if (c == '\"' && !escape) {
            read_str = !read_str;

            // if(key){
            //     key_str += c;
            // }else if(val){
            //     val_str += c;
            // }

            continue;
        }
        else if (c == ':') {
            if (!key)
                throw std::runtime_error("Missing key!");
            key = false;
            val = true;
            continue;
        }
        else if (c == ',') {
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

inline Node Marshall(std::string const& str) { return Value::parse_node(str); }

}