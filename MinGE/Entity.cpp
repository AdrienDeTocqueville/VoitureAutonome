#include "MinGE.h"
#include <cstring>

std::list<Entity*> Entity::entities;
SizeMap Entity::scriptSizes;

Entity::Entity(Tag _tag, bool _prototype):
    prototype(_prototype), tag(_tag),
    tr(nullptr)
{
    entities.push_back(this);
}

Entity::~Entity()
{
    // free components
    for (auto& componentPair: components)
    {
        auto csize = componentPair.second.size();
        for (unsigned i(0) ; i < csize ; i++)
        {
            if (!prototype)
                componentPair.second.back()->onDeregister();

            delete componentPair.second.back();
            componentPair.second.pop_back();
        }
    }

    delete tr;
}

/// Methods (static)
Entity* Entity::create(const Tag& _tag, bool _prototype, vec3 _position, vec3 _rotation, vec3 _scale)
{
    return (new Entity(_tag, _prototype))->insert<Transform>(_position, _rotation, _scale);
}

Entity* Entity::clone(Entity* _entity, vec3 _position, vec3 _rotation, vec3 _scale)
{
    Entity* e = Entity::create(_entity->tag, false, _position, _rotation, _scale);

    for (auto& componentPair: _entity->components)
    {
        auto& components = e->components[componentPair.first];

        for (Component* component: componentPair.second)
        {
            Component* c = component->clone();

            if (c == nullptr)
            {
                size_t s = scriptSizes[typeid(*component)];

                c = reinterpret_cast<Component*>(operator new(s));
                memcpy(c, component, s);
                Component::instances++;
            }

            components.push_back(c);

            c->entity = e;
            c->tr = e->tr;
            c->onRegister();
        }
    }

    return e;
}

Entity* Entity::clone(Entity* _entity)
{
    Transform* tr = _entity->find<Transform>();

    return Entity::clone(_entity, tr->position, eulerAngles(tr->rotation), tr->scale);
}

void Entity::clear()
{
    for(Entity* entity: entities)
        delete entity;

    entities.clear();
}

void Entity::destroy(Entity* _entity)
{
    entities.remove(_entity);

    delete _entity;
}

Entity* Entity::findByTag(const Tag& _tag, bool _allowPrototypes)
{
    for (Entity* entity: entities)
    {
        if (entity->tag == _tag)
        {
            if (!entity->prototype || _allowPrototypes)
                return entity;
        }
    }

    return nullptr;
}

std::list<Entity*> Entity::findAllByTag(const Tag& _tag, bool _allowPrototypes)
{
    std::list<Entity*> samedTag;

    for (Entity* entity: entities)
    {
        if (entity->tag == _tag)
        {
            if (!entity->prototype || _allowPrototypes)
                samedTag.push_back(entity);
        }
    }

    return samedTag;
}

/// Methods (private)
void Entity::insertComponent(Component* _component, std::type_index _typeid)
{
    components[_typeid].push_back(_component);

    _component->entity = this;

    if (!prototype)
    {
        _component->tr = tr;
        _component->onRegister();
    }
}

void Entity::removeComponent(std::type_index _componentTypeid, std::type_index _typeid)
{
    std::vector<Component*>& vec = components[_typeid];

    for (unsigned i(0) ; i < vec.size() ; i++)
    {
        Component* c = vec[i];

        if (std::type_index(typeid(*c)) == _componentTypeid)
        {
            vec.erase(vec.begin() + i);

            if (!prototype)
                c->onDeregister();

            delete c;
            return;
        }
    }
}

void Entity::removeComponents(std::type_index _componentTypeid, std::type_index _typeid)
{
    std::vector<Component*>& vec = components[_typeid];

    for (unsigned i(0) ; i < vec.size() ; i++)
    {
        Component* c = vec[i];

        if (std::type_index(typeid(*c)) == _componentTypeid)
        {
            vec.erase(vec.begin() + i--);

            if (!prototype)
                c->onDeregister();

            delete c;
        }
    }
}

/// Getter (private)
std::type_index Entity::getColliderTypeIndex()
{
    return typeid(Collider);
}

std::type_index Entity::getScriptTypeIndex()
{
    return typeid(Script);
}

/// Other
//template<typename... Args>
//Entity* Entity::insert<Transform>(Args&&... args)
//{
//    if (tr != nullptr)
//        Error::add(ErrorType::USER_ERROR, "Impossible to add a Transform component");
//    else
//        tr = new Transform(args...);
//
//    return this;
//}

template <>
void Entity::remove<Transform>()
{
    Error::add(ErrorType::USER_ERROR, "Impossible to remove the Transform component");
}

template <>
Transform* Entity::find<Transform>()
{
    return tr;
}

template <>
void Entity::removeAll<Collider>()
{
    for (Component* collider: components[typeid(Collider)])
    {
        Collider* c = static_cast<Collider*>(collider);

        c->entity = nullptr;
        c->rigidBody = nullptr;

        c->onDeregister();
    }

    components[typeid(Collider)].clear();

    RigidBody* rb = find<RigidBody>();
    if (rb != nullptr)
        rb->computeMass();
}

template <>
std::vector<Component*> Entity::findAll()
{
    std::vector<Component*> _components;

    for (auto& componentPair: components)
        for (Component* component: componentPair.second)
            _components.push_back(component);

    return _components;
}

template <>
std::vector<Collider*> Entity::findAll()
{
    std::vector<Component*>& _colliders = components[typeid(Collider)];
    std::vector<Collider*> colliders; colliders.resize(_colliders.size());

    for (unsigned i(0) ; i < _colliders.size() ; i++)
        colliders[i] = static_cast<Collider*>(_colliders[i]);

    return colliders;
}

