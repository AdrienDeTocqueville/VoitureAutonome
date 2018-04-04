#include "includes.h"

#include <windows.h>

vec2 toVec2(sf::Vector2i v)
{
    return vec2(v.x, v.y);
}
vec2 toVec2(sf::Vector2u v)
{
    return vec2(v.x, v.y);
}
sf::Vector2i toSFVec2i(vec2 v)
{
    return sf::Vector2i(v.x, v.y);
}
sf::Vector2u toSFVec2u(vec2 v)
{
    return sf::Vector2u(v.x, v.y);
}

vec3 vecClamp(vec3 v)
{
    for (unsigned i(0) ; i < 3 ; i++)
        if (epsilonEqual(v[i], 0.0f, EPSILON))
            v[i] = 0.0f;

    return v;
}

std::ostream& operator<<(std::ostream& os, const vec3& _vec)
{
    return os << _vec.x << " " << _vec.y << " " << _vec.z;
}

std::istream& operator>>(std::istream& os, vec3& _vec)
{
    return os >> _vec.x >> _vec.y >> _vec.z;
}

bool epsilonEqual(const vec3& a, const vec3& b, float epsilon)
{
    for (unsigned i(0) ; i < 3 ; i++)
    if (!epsilonEqual(a[i], b[i], epsilon))
        return false;

    return true;
}

bool epsilonEqual(const quat& a, const quat& b, float epsilon)
{
    for (unsigned i(0) ; i < 4 ; i++)
    if (!epsilonEqual(a[i], b[i], epsilon))
        return false;

    return true;
}