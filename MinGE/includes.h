#ifndef INCLUDES_H_INCLUDED
#define INCLUDES_H_INCLUDED

#include <GL/glew.h>

#include <SFML/System.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <unordered_map>
#include <vector>
#include <list>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/quaternion.hpp>


const float PI = 3.14159265358979323846f;
const float EPSILON = 0.0001f;

using namespace glm;

vec2 toVec2(sf::Vector2i v);
vec2 toVec2(sf::Vector2u v);
sf::Vector2i toSFVec2i(vec2 v);
sf::Vector2u toSFVec2u(vec2 v);

vec3 vecClamp(vec3 v);

std::ostream& operator<<(std::ostream& os, const vec3& _vec);
std::istream& operator>>(std::istream& os, vec3& _vec);

bool epsilonEqual(const vec3& a, const vec3& b, float epsilon = EPSILON);
bool epsilonEqual(const quat& a, const quat& b, float epsilon = EPSILON);

#endif // INCLUDES_H_INCLUDED
