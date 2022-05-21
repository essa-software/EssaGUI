#pragma once

#include <memory>
#include "../Object.hpp"
#include "Oct.hpp"

class BHTree {
    std::unique_ptr<Object> m_object;
    Oct* m_oct;
    Oct* m_octs[8];

public:
    BHTree(Oct* q){
        m_oct = q;
        m_object = nullptr;

        for(unsigned i = 0; i < 8; i++)
            m_octs[i] = nullptr;
    }

    bool is_external(){
        for(unsigned i = 0; i < 8; i++){
            if(m_octs[i] != nullptr)
                return false;
        }
        return true;
    }

    void instrt(std::unique_ptr<Object>& obj){
        // if(m_object == nullptr)
        //     m_object = std::move(obj);
        // else if()
    }
};
