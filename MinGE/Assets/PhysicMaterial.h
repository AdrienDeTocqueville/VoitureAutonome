#ifndef PHYSICMATERIAL_H
#define PHYSICMATERIAL_H

#include "includes.h"

class PhysicMaterial
{
    friend class Collider;

    public:
        PhysicMaterial(std::string _name, float _restitution = 0.2f, float _dF = 0.6f, float _sF = 0.6f);

        /// Methods (static)
            static PhysicMaterial* get(std::string _name);
            static void clear();

        /// Attributes (static)
            static std::unordered_map<std::string, PhysicMaterial*> materials;
            static PhysicMaterial* base;

        /// Setter
            void setRestitution(float _restitution);

    protected:
        ~PhysicMaterial() {}

        std::string name;

        float restitution;
        float dynamicFriction, staticFriction;
};

#endif // PHYSICMATERIAL_H
