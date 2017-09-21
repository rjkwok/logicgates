#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

// srand, rand
#include <stdlib.h>

#include "util.h"

int main() {

    srand(time(NULL));

    // Instantiate a new window
    sf::RenderWindow window(sf::VideoMode(1440, 900), "Debug");

    // Create a view that will save the default window view
    sf::View window_view = window.getView();

    // Create a view that will represent the transformed and scaled view of the scene
    sf::View scene_view = window_view;

    // Instantiate our input struct to hold the inputs for each tick
    Input input;

    // Initialize loop timer
    sf::Clock timer;
    double dt = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main game loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while (window.isOpen()) {

        //////////////////////////////////////////
        // Prepare the inputs to this iteration
        //////////////////////////////////////////

        dt = timer.getElapsedTime().asSeconds();
        timer.restart();

        input.collect(window, scene_view);

        //////////////////////////////////////////
        // Change things
        //////////////////////////////////////////

        if (input.keyReleased(sf::Keyboard::Escape)) {
            window.close();
        }

        //////////////////////////////////////////
        // Draw
        //////////////////////////////////////////

        window.clear();
        window.display();
    }

    return 0;
}
