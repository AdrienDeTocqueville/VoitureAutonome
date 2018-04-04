#pragma once

#include <MinGE.h>

class TPSCamera : public Script
{
    public:
        TPSCamera(Transform* _target, float _distance = 1.0f, vec3 _offset = vec3(0.0f)):
            target(_target), distance(_distance), offset(_offset)
        { }

        void start() override
        {
            Entity* obstacle = Entity::create("Obstacle", true);
            obstacle->insert<Graphic>(Mesh::createSphere(Material::base, ALLFLAGS, 1.0f));

            vec3 pos;
            FileReader file("obstacles.txt");

            while (file.read("obstacle", pos))
                Entity::clone(obstacle, pos + vec3(0,0, 0.25));
        }

        void lateUpdate() override
        {
            tr->setPosition(target->getToWorldSpace(offset));
            tr->lookAt(target->getPosition() + distance * target->getDirection());
        }

    private:
        Transform* target;

        float distance;

        vec3 offset;
};

class FreeFlyCamera : public Script
{
    public:
        FreeFlyCamera(float _sensivity = 0.2f):
            angles(0.0f), clampAngleY(-0.499f*PI, 0.499f*PI),
            sensivity(_sensivity)
        { }

        /// Methods (public)
            void start() override
            {
                Input::getMouseDelta();
                Input::setCursorMode(GE_CAPTURE);

                obstacle = Entity::create("Obstacle", true);
                obstacle->insert<Graphic>(Mesh::createSphere(Material::base, ALLFLAGS, 1.0f));


                vec3 pos;
                file.load("obstacles.txt");

                while (file.read("obstacle", pos))
                    Entity::clone(obstacle, pos + vec3(0,0, 0.25));

                file.clear();
            }

            void onDestroy() override
            {
                file.save();
            }

            void update() override
            {
                Debug::drawVector(vec3(0.0f), vec3(1, 0, 0), vec3(1, 0, 0));
                Debug::drawVector(vec3(0.0f), vec3(0, 1, 0), vec3(0, 1, 0));
                Debug::drawVector(vec3(0.0f), vec3(0, 0, 1), vec3(0, 0, 1));

                angles += radians(Input::getMouseDelta() * sensivity);
                angles.y = clamp(angles.y, clampAngleY.x, clampAngleY.y);

                auto hit = PhysicEngine::get()->raycast(tr->position, tr->getDirection());
                if (hit)
                {
                    Debug::drawPoint(hit.point + 0.5f*hit.normal);
                    if (Input::getMouseReleased(sf::Mouse::Button::Left))
                    {
                        vec3 pos = hit.point + 0.5f*hit.normal;

                        Entity::clone(obstacle, pos);
                        file.write("obstacle", pos);
                    }
                }
                if (Input::getMouseReleased(sf::Mouse::Button::Right))
                {
                    file.clear();

                    for (auto* obs: Entity::findAllByTag("Obstacle", false))
                        Entity::destroy(obs);
                }
            }

            void lateUpdate() override
            {
                if (Input::getCursorMode() != GE_CAPTURE)
                    return;

                vec3 dir = tr->getVectorToWorldSpace(vec3(1, 0, 0));
                vec3 left = tr->getVectorToWorldSpace(vec3(0, 1, 0));

                float speed = (Input::getKeyDown(sf::Keyboard::LShift)?20.0f:10.0f) * Time::deltaTime;

                if (Input::getKeyDown(sf::Keyboard::Z)) tr->position +=  dir * speed;
                if (Input::getKeyDown(sf::Keyboard::S)) tr->position -=  dir * speed;
                if (Input::getKeyDown(sf::Keyboard::Q)) tr->position += left * speed;
                if (Input::getKeyDown(sf::Keyboard::D)) tr->position -= left * speed;

                tr->position += Input::getMouseWheelDelta()*2*speed*dir;
                tr->rotation = quat(vec3(0.0f, angles.y, angles.x));

                tr->toMatrix();
            }

            void lookAt(vec3 _pos)
            {
                tr->lookAt(_pos);
                vec3 ea = eulerAngles(tr->rotation);

                angles.x = ea.z;
                angles.y = ea.y;
            }

    private:
        FileReader file;
        Entity* obstacle;

        vec2 angles;
        vec2 clampAngleY;

        float sensivity;
};
