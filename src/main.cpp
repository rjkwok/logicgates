#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

// srand, rand
#include <stdlib.h>
// floor
#include <math.h>
// function
#include <functional>

#include "util.h"
#include "gate.h"

typedef sf::Vector2f Vec2;

int main() {    

    srand(time(NULL));

    // Instantiate a new window
    sf::RenderWindow window(sf::VideoMode(1440, 900), "Debug");

    // Create a view that will save the default window view
    sf::View window_view = window.getView();

    // Create a view that will represent the transformed and scaled view of the scene
    sf::View scene_view = window_view;

    // Load textures
    sf::Texture or_texture;
    sf::Texture and_texture;
    sf::Texture not_texture;
    
    or_texture.loadFromFile("resources/or.png");
    and_texture.loadFromFile("resources/and.png");
    not_texture.loadFromFile("resources/not.png");

    // Make some factory functions for creating certain types of logic gates
    auto or_factory = [&or_texture](){ return new Gate(or_texture, 2, new Vec2[2]{ Vec2(0, 15), Vec2(0, 75) }, Vec2(89, 45), OR); };
    auto and_factory = [&and_texture](){ return new Gate(and_texture, 2, new Vec2[2]{ Vec2(0, 15), Vec2(0, 75) }, Vec2(89, 45), AND); };
    auto not_factory = [&not_texture](){ return new Gate(not_texture, 1, new Vec2[1]{ Vec2(0, 45) }, Vec2(89, 45), NOT); };

    // Put factory functions in an iterable container for easy UI access
    std::vector<std::function<Gate*()> > factories { or_factory, and_factory, not_factory};
    auto factory = factories.begin();

    // Declare gate tracking vector
    std::vector<Gate*> gates;

    // Instantiate our input struct to hold the inputs for each tick
    Input input;

    // Declare grid resolution and grid cursor
    const float tile_width = 30.0f;

    sf::RectangleShape cursor(Vec2(tile_width, tile_width));
    cursor.setOutlineThickness(0);
    cursor.setFillColor(sf::Color(0, 255, 0, 155));

    // Buffer to allow two gates to be selected for wiring
    Gate* wiring_gate = nullptr;

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

        // If the user hits tab, switch the kind of logic gate that is being generated
        if (input.keyReleased(sf::Keyboard::Tab)) {
            factory++;
            if (factory == factories.end()) {
                factory = factories.begin();
            }
        }

        // Update cursor position and snap to grid
        cursor.setPosition(floor(input.scene_mouse.x/tile_width)*tile_width, floor(input.scene_mouse.y/tile_width)*tile_width);

        // Create new gate at cursor position if user clicks
        if (input.lmb_released && !wiring_gate) {
            gates.push_back((*factory)());
            gates[gates.size() - 1]->sprite.setPosition(cursor.getPosition());
        }

        if (input.rmb_released && !wiring_gate) {
            for (auto each : gates) {
                sf::Vector2f output_position = each->sprite.getPosition() + each->output_position;
                if (floor(input.scene_mouse.x/tile_width) == floor(output_position.x/tile_width) &&
                    floor(input.scene_mouse.y/tile_width) == floor(output_position.y/tile_width)) 
                {
                    wiring_gate = each;
                    cursor.setFillColor(sf::Color(255, 0, 0, 155));
                }
            }
        }
        else if (input.rmb_released && wiring_gate) {
            for (auto each : gates) {
                for (int i = 0; i < each->input_count; i++) {
                    sf::Vector2f input_position = each->sprite.getPosition() + *(each->input_position + i);
                    if (floor(input.scene_mouse.x/tile_width) == floor(input_position.x/tile_width) &&
                        floor(input.scene_mouse.y/tile_width) == floor(input_position.y/tile_width)) 
                    {
                        *(each->input + i) = wiring_gate;
                        wiring_gate = nullptr;
                        cursor.setFillColor(sf::Color(0, 255, 0, 155));
                    }
                }
            }
        }

        //////////////////////////////////////////
        // Draw
        //////////////////////////////////////////

        window.clear();

        for (auto each : gates) {
            window.draw(each->sprite);
        }
        window.draw(cursor);

        window.display();
    }

    return 0;
}
