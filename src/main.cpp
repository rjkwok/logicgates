#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

// srand, rand
#include <stdlib.h>
// floor
#include <math.h>

#include "util.h"

typedef sf::Vector2f Vec2;

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

    // Declare grid resolution and grid cursor
    const float tile_width = 30.0f;

    sf::RectangleShape cursor(Vec2(tile_width, tile_width));
    cursor.setOutlineThickness(0);
    cursor.setFillColor(sf::Color(0, 255, 0, 155));

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

        cursor.setPosition(floor(input.scene_mouse.x/tile_width)*tile_width, floor(input.scene_mouse.y/tile_width)*tile_width);

        //////////////////////////////////////////
        // Draw
        //////////////////////////////////////////

        window.clear();

        window.draw(cursor);

        window.display();
    }

    return 0;
}
