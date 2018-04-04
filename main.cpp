#include <MinGE.h>

#include "Drive.h"
#include "CameraScript.h"

#define WINDOWED

#ifdef WINDOWED
    #define VIDEOMODE sf::VideoMode(1280, 720)
    #define STYLE sf::Style::Default
#else
    #define VIDEOMODE sf::VideoMode::getDesktopMode()
    #define STYLE sf::Style::Fullscreen
#endif

void loadUdacity();

int main()
{
    std::cout << "  -- MinGE --" << std::endl;


    /// Create window
        sf::RenderWindow window(VIDEOMODE, "Udacity", STYLE, sf::ContextSettings(24, 0, 0, 4, 3));

    std::cout << window.getSize().x << "  " << window.getSize().y << std::endl;


    /// Create engine
        Engine* engine = new Engine(&window, 60);
        std::cout << "Seed: " << Random::getSeed() << std::endl;

        Input::setCursorMode(GE_CAPTURE);
        PhysicEngine::get()->setGravity(vec3(0.0f));
        loadUdacity();



    /// Main loop
        engine->start();

        while ( Input::isOpen() )
        {
            /// Handle events
                if (Input::getKeyReleased(sf::Keyboard::F1))
                    AABB::drawAABBs = !AABB::drawAABBs;

                if (Input::getKeyReleased(sf::Keyboard::F2))
                    PhysicEngine::get()->setGravity();

                if (Input::getKeyReleased(sf::Keyboard::F3))
                    GraphicEngine::get()->toggleWireframe();

                if (Input::getKeyReleased(sf::Keyboard::Escape) || !Input::hasFocus())
                {
                    engine->setPause(true);
                    Input::setCursorMode(GE_FREE);
                }

                if (Input::getMousePressed(sf::Mouse::Left) && Input::hasFocus())
                {
                    engine->setPause(false);
                    Input::setCursorMode(GE_CAPTURE);
                }

            /// Render
                if (engine->update())
                    window.display();
        }

        std::cout << '\n' << '\n' << std::endl;

    /// Delete resources
        delete engine;

    #ifdef DEBUG
        sf::sleep(sf::seconds(1.0f));
    #endif // DEBUG

    return 0;
}

void loadUdacity()
{
    Entity* car = Entity::create("Car", false, vec3(0, 0, 0), vec3(0, 0, PI/2));
        car->insert<Graphic>(new Model("Models/Car/car.obj"));
        car->insert<Drive>();


    Entity::create("Track", false, vec3(0, 0, 0), vec3(0))
        ->insert<Graphic>(new Model("Models/FullTrack/fullTrack.obj"));

    Entity::create("Terrain", false, vec3(40, 50, -1), vec3(0, 0, PI), vec3(1, 1, 0.7f))
        ->insert<Graphic>(new Terrain("Heightmaps/Montagnes/montagnes.terrain"))
        ->insert<Box>(vec3(256, 256, 0.5));


    // Light source
        ModelMaterial* bright = new ModelMaterial("bright");
            bright->ambient = vec3(10.0f/4.0f);
            bright->diffuse = vec3(0.0f);
            bright->specular = vec3(0.0f);
            bright->texture = Texture::get("Textures/white.png");

        Entity::create("Light", false, vec3(0, 0, 30))->insert<Light>(GE_POINT_LIGHT, vec3(0.0f), vec3(1.0f), 0.2f, 1.0f, 0.0f, 0.0f);

    // Camera
        Entity::create("MainCamera")
            ->insert<Camera>(70, 0.1f, 1000.0f, vec3(0.67f, 0.92f, 1.0f))
//            ->insert<TPSCamera>(car->find<Transform>(), 20.0f, vec3(-8, 0.1565, 3));
            ->insert<FreeFlyCamera>();
}
