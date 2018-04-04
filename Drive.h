#pragma once

#include <fstream>
#include <MinGE.h>

#include "RNA/RNA.h"

struct IAScript: public Script
{
    void start() override
    {
        steering = 0.0f;
        camera = find<Camera>();

        target = Entity::findByTag("Car")->find<Transform>();
        offset = vec3(-0.5, 0.1565, 2.5);

        network.loadFromFile("model.rna");
        network.openCL(CL_DEVICE_TYPE_GPU);

        commandQueue.create(network.getContext(), true);

        running = false;

        Entity* gui = Entity::create("GUI", false, vec3(-0.5, 0.5, 0));
            gui->insert<Graphic>(Mesh::createQuad(new GUIMaterial("gui", camera->getColorBuffer()), VERTICES|TEXCOORDS, vec2(0.25)));
    }

    void lateUpdate() override
    {
        tr->setPosition(target->getToWorldSpace(offset));
        tr->lookAt(target->getPosition() + 8.0f * target->getDirection());
    }

    float getSteering()
    {
        if (running)
        {
            cl_int status;
            clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &status, nullptr);

            if (status == CL_COMPLETE)
            {
                steering = network.getOutput()[0];
                running = false;
            }
        }

        if (!running)
        {
            sf::Image image;
            camera->toImage(image);

            size_t w = image.getSize().x;
            size_t h = image.getSize().y;

            Tensor t{1, 3, w, h};

            for (unsigned i(0) ; i < w ; i++)
            {
                for (unsigned j(0) ; j < h ; j++)
                {
                    t({0, 0, i, j}) = image.getPixel(i, j).r / 127.5 - 1.0;
                    t({0, 1, i, j}) = image.getPixel(i, j).g / 127.5 - 1.0;
                    t({0, 2, i, j}) = image.getPixel(i, j).b / 127.5 - 1.0;
                }
            }


            running = true;
            const Tensor& output = network.feedForward(commandQueue, t);
            output.readBuffer(commandQueue);
            clEnqueueMarkerWithWaitList(commandQueue(), 0, nullptr, &event);
        }

        return steering;
    }

    float steering;

    Camera* camera;

    Transform* target;
    vec3 offset;

    rna::Network network;
    cl::CommandQueue commandQueue;
    cl_event event;
    bool running;
};

struct Drive : public Script
{
    void start()
    {
        speed = 10.0f;
        steering = 0.0f;

        genData = false;
        manual = true;

        Entity* e = Entity::create("IA")
            ->insert<Camera>(70, 0.1f, 1000.0f, vec3(0.67f, 0.92f, 1.0f), new RenderTexture(256, 144))
            ->insert<IAScript>();

        ia = e->find<IAScript>();
    }

    void update() override
    {
        if (Input::getKeyReleased(sf::Keyboard::F5))
        {
            std::cout << "Database generating mode" << std::endl;

            genData = true;
            timer.restart();
            file.open("data/testing.csv", std::ofstream::out | std::ofstream::app);
        }

        if (Input::getKeyReleased(sf::Keyboard::F6))
        {
            if (manual)
            {
                std::cout << "Switching to automatic mode" << std::endl;

                manual = false;
            }
            else
            {
                std::cout << "Switching to manual mode" << std::endl;

                manual = true;
            }
        }

        if (manual)
        {
            steering = 0.2f*steering + 0.8f*(Input::getMouseDelta().x/10.f);

            if (Input::getKeyDown(sf::Keyboard::Z))
                speed = 15.0f;
            else if (Input::getKeyDown(sf::Keyboard::S))
                speed = -5.0f;
            else
                speed = 0.0f;
        }
        else
        {
            speed = 10.0f;
            steering = 0.2f*steering + 0.8f*ia->getSteering();
//            steering = ia->getSteering();
        }

        if (genData && timer.getElapsedTime().asSeconds() > 0.25f)
        {
            timer.restart();

            std::string fileName = "data/testing/img_" + toString(Random::getSeed()) + "_" + toString(clock()) + ".png";
            ia->find<Camera>()->saveToFile(fileName);

            file << fileName << "  " << steering << '\n';
        }


        setCommands(speed, steering);
    }

    void setCommands(float _speed, float _steering)
    {
        if (_speed == 0.0f)
            return;

        vec3 speed(_speed, 0, 0);
        vec3 steering(0, 0, _steering);

        if (_speed < 0.0f)
            steering *= -1.0f;

        tr->rotate(steering * Time::deltaTime);
        tr->translate(tr->getVectorToWorldSpace(speed) * Time::deltaTime);
    }

    float speed, steering;

    bool genData, manual;
    IAScript* ia;

    sf::Clock timer;
    std::ofstream file;
};
