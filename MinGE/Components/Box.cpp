#include "Components/Transform.h"
#include "Components/RigidBody.h"
#include "Components/Box.h"

#include <glm/gtx/matrix_operation.hpp>

Box::Box(vec3 _halfExtent, vec3 _center, PhysicMaterial* _material, bool _isTrigger):
    Collider(_material, _isTrigger, _center),
    halfExtent(_halfExtent)
{ }

Box::~Box()
{ }

/// Methods (public)
Box* Box::clone() const
{
    return new Box(halfExtent, center, material, isTrigger);
}

void Box::computeMass()
{
    vec3 d = getDimensions();

    float x2 = d.x*d.x;
    float y2 = d.y*d.y;
    float z2 = d.z*d.z;


    mass = rigidBody->getDensity() * d.x*d.y*d.z;
    inertia = diagonal3x3((1.0f/12.0f)*mass * vec3(y2 + z2, x2 + z2, x2 + y2));
}

void Box::computeAABB()
{
//    vec3 base[3] = {    tr->getToWorldSpace(vec3(1, 0, 0)),
//                        tr->getToWorldSpace(vec3(0, 1, 0)),
//                        tr->getToWorldSpace(vec3(0, 0, 1))    };
//
//    vec3 halfExtent2;
//    for (unsigned i(0) ; i < 3 ; i++)
//        halfExtent2[i] = dot(halfExtent2, abs(base[i] - tr->position));
//
//    Debug::drawVector(origin, halfExtent2);
//    Debug::drawVector(origin, -halfExtent2);


    aabb.center = min(tr->getToWorldSpace(-halfExtent),
                   tr->getToWorldSpace(vec3(-halfExtent.x, -halfExtent.y, halfExtent.z)));  // Valeur initiale

    for (int i(1) ; i < 4 ; i++)    // On cherche le point le plus bas
    {
        vec3 point(halfExtent.x *(2.0f*(i>1) -1.0f), halfExtent.y *(2.0f*(i%2) -1.0f), -halfExtent.z);

        aabb.center = min(aabb.center, tr->getToWorldSpace(point));
        point.z *= -1.0f;

        aabb.center = min(aabb.center, tr->getToWorldSpace(point));
    }

    aabb.dim = (tr->getToWorldSpace(vec3(0.0f)) - aabb.center);
    aabb.center = tr->getToWorldSpace(center);
}

RayHit Box::raycast(vec3 _o, vec3 _d)
{
    RayHit r;

    vec3 OC = tr->position - _o;

    if (dot(OC, _d) < 0)
        return r;

    vec3 o = tr->getToLocalSpace(_o);
    vec3 d = tr->getVectorToLocalSpace(_d);

    // https://tavianator.com/fast-branchless-raybounding-box-intersections/
    float tmin = FLT_MIN, tmax = FLT_MAX;
    float t1, t2;
    unsigned axis = 0;

    for (unsigned i(0) ; i < 3 ; i++)
    {
        t1 = (-halfExtent[i] + center[i] - o[i]) / d[i];
        t2 = ( halfExtent[i] + center[i] - o[i]) / d[i];

        if (t1 > t2) std::swap(t1, t2);

        if (t1 > tmin)
        {
            tmin = t1;
            axis = i;
        }

        tmax = min(tmax, t2);
    }

    if (tmin > tmax)
        return r;

    r.collider = this;
    r.point = tr->getToWorldSpace(o + tmin*d);

    r.normal[axis] = 2 * (0.0f < o[axis]) - 1.0f;
    r.normal = normalize(tr->getVectorToWorldSpace(r.normal));

    r.distance = tmin;

    return r;
}

/// Setters
void Box::setHalfExtent(vec3 _halfExtent)
{
    halfExtent = _halfExtent;
}

/// Getters
vec3 Box::getSupport(vec3 _axis)
{
    _axis = tr->getVectorToLocalSpace(_axis);

    vec3 support(halfExtent);

    for (unsigned i(0) ; i < 3; i++)
        if (_axis[i] < 0.0f)
            support[i] *= -1.0f;

    return tr->getToWorldSpace(support + center);
}

vec3 Box::getHalfExtent() const
{
    return halfExtent * tr->scale;
}

vec3 Box::getDimensions() const
{
    return 2.0f * halfExtent * tr->scale;
}
