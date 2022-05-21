#pragma once
#include "../math/Vector3.hpp"

class Oct{
    double m_length;
    Vector3 m_midpoint;
public:
    Oct(Vector3 mid, double len) : m_midpoint(mid), m_length(len){}

    double length() const{return m_length;}


    bool contains(Vector3 point) {
        if (point.x <= m_midpoint.x + m_length / 2.0 && point.x >= m_midpoint.x - m_length / 2.0 && 
            point.y <= m_midpoint.y + m_length / 2.0 && point.y >= m_midpoint.y - m_length / 2.0 && 
            point.z <= m_midpoint.z + m_length / 2.0 && point.z >= m_midpoint.z - m_length / 2.0) {
        return true;
    }else {
        return false;
        }
    }

    Oct* create_child(int q1, int q2, int q3) const{
        Oct* new_oct;
        new_oct = new Oct(Vector3(m_midpoint.x + m_length / 4 * q1, m_midpoint.y + m_length / 4 * q2, m_midpoint.z + m_length / 4 * q3), m_length / 2);

        return new_oct;
    }
};
