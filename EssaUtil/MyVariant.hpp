#pragma once
#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <utility>

template<size_t arg1, size_t... others>
struct static_max;

template<size_t val>
struct static_max<val>{
    static const size_t value = val;
};

template<size_t arg1, size_t arg2, size_t... others>
struct static_max<arg1, arg2, others...>{
    static const size_t value = (arg1 >= arg2) ? static_max<arg1, others...>::value : static_max<arg2, others...>::value;
};

template<typename... Types>
struct variant_helper;

template<typename T, typename... Types>
struct variant_helper<T, Types...>{
    inline static void destroy(size_t id, void* data){
        if(id == typeid(T).hash_code()){
            reinterpret_cast<T*>(data)->~T();
        }else{
            variant_helper<Types...>::destroy(id, data);
        }
    }

    inline static void copy(size_t id, void* old_data, void* new_data){
        if(id == typeid(T).hash_code()){
            new (new_data) T(std::copy(reinterpret_cast<T*>(old_data)));
        }else{
            variant_helper<Types...>::copy(id, old_data, new_data);
        }
    }

    inline static void move(size_t id, void* old_data, void* new_data){
        if(id == typeid(T).hash_code()){
            new (new_data) T(std::move(reinterpret_cast<T*>(old_data)));
        }else{
            variant_helper<Types...>::move(id, old_data, new_data);
        }
    }
};

template<>
struct variant_helper<>{
    inline static void destroy(size_t, void*){ }
    inline static void copy(size_t, void*, void*){ }
    inline static void move(size_t, void*, void*){ }
};

template<typename... Types>
class MyVariant{
    static const size_t data_size = static_max<sizeof(Types)...>::value;
    static const size_t data_align = static_max<alignof(Types)...>::value;

    size_t invalid_type(){
        return typeid(void).hash_code();
    }

    using helper = variant_helper<Types...>;
    using data_t = typename std::aligned_storage<data_size, data_align>::type;

    data_t data;
    size_t type_id;

public:
    MyVariant() : type_id(invalid_type()){ }
    
    template<typename T>
    MyVariant(const T& val){
        set<T>(val);
    }

    MyVariant(MyVariant<Types...>& other) : type_id(other.type_id) {
        helper::copy(type_id, &other.data, &data);
    }

    MyVariant(MyVariant<Types...>&& other) : type_id(other.type_id) {
        helper::move(type_id, &other.data, &data);
    }

    MyVariant<Types...>& operator=(MyVariant<Types...> other){
        std::swap(type_id, other.type_id);
        std::swap(data, other.data);

        return *this;
    }

    template<typename T>
    bool is_type_of() const{
        return type_id == typeid(T).hash_code();
    }

    template<typename T, typename... Args>
    void set(Args&&... args){
        helper::destroy(type_id, &data);
        new (&data) T(std::forward<Args>(args)...);
        type_id = typeid(T).hash_code();
    }

    template<typename T>
    T& get(){
        if(type_id == typeid(T).hash_code()){
            return *reinterpret_cast<T*>(&data);
        }else{
            throw std::bad_cast();
        }
    }

    template<typename T>
    T get() const{
        if(type_id == typeid(T).hash_code()){
            return *reinterpret_cast<T*>(&data);
        }else{
            throw std::bad_cast();
        }
    }
};
