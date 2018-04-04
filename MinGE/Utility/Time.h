#pragma once

class Time
{
    friend class Engine;

    public:
        static float deltaTime;
        static float timeScale;

    private:
        /// Methods (private)
            static void init();
};
