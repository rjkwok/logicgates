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
    sf::Texture const_texture;
    sf::Texture output_texture;

    or_texture.loadFromFile("resources/or.png");
    and_texture.loadFromFile("resources/and.png");
    not_texture.loadFromFile("resources/not.png");
    const_texture.loadFromFile("resources/const.png");
    output_texture.loadFromFile("resources/output.png");

    // Load fonts
    sf::Font font;
    font.loadFromFile("resources/font1.ttf");

    // Make some factory functions for creating certain types of logic gates
    auto or_factory = [&or_texture](){ return new Gate(or_texture, 2, new Vec2[2]{ Vec2(0, 15), Vec2(0, 75) }, Vec2(89, 45), OR); };
    auto and_factory = [&and_texture](){ return new Gate(and_texture, 2, new Vec2[2]{ Vec2(0, 15), Vec2(0, 75) }, Vec2(89, 45), AND); };
    auto not_factory = [&not_texture](){ return new Gate(not_texture, 1, new Vec2[1]{ Vec2(0, 45) }, Vec2(89, 45), NOT); };
    auto const_factory = [&const_texture](){ return new Gate(const_texture, 0, new Vec2[0], Vec2(89, 45), ZERO); };

    // Put factory functions in an iterable container for easy UI access
    std::vector<std::function<Gate*()> > factories { or_factory, and_factory, not_factory};
    auto factory = factories.begin();

    // Declare gate tracking vector
    std::vector<Gate*> gates;

    // Container for holdings labels
    std::vector<sf::Text> labels;

    // Container for holding visual representations of wires
    std::vector<sf::RectangleShape> wires;
    std::vector<sf::CircleShape>    joints;

    // Instantiate our input struct to hold the inputs for each tick
    Input input;

    // Declare grid resolution and grid cursor
    const float tile_width = 30.0f;

    sf::RectangleShape cursor(Vec2(tile_width, tile_width));
    cursor.setOutlineThickness(0);
    cursor.setFillColor(sf::Color(0, 255, 0, 155));

    std::vector<sf::RectangleShape> wire_previews;
    std::vector<sf::CircleShape>    joint_previews;

    // Buffer to allow two gates to be selected for wiring
    Gate* wiring_gate = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup puzzle
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color gate_color = sf::Color(255, 165, 0, 255);
    gates.push_back(new Gate(output_texture, 1, new Vec2[1]{ Vec2(0, 45) }, Vec2(89, 45), BUFFER));
    gates[gates.size() - 1]->sprite.setPosition(Vec2(floor((window.getSize().x/tile_width)-4)*tile_width, tile_width));
    gates[gates.size() - 1]->sprite.setColor(gate_color);
    Gate* output_gate = gates[gates.size() - 1];
    labels.push_back(makeText("Y", font, Vec2(floor((window.getSize().x/tile_width)-4)*tile_width + 1.5*tile_width, tile_width + 1.5*tile_width), gate_color, 16));

    // Container to track input gates
    std::vector<Gate*> input_gates;

    int total_inputs = 2;

    for (int i = 0; i < total_inputs; i++) {

        gates.push_back(const_factory());
        gates[gates.size() - 1]->sprite.setPosition(Vec2(tile_width, tile_width + 4*i*tile_width));
        gates[gates.size() - 1]->sprite.setColor(gate_color);
        input_gates.push_back(gates[gates.size() - 1]);
        labels.push_back(makeText("X" + asString(i + 1), font, Vec2(tile_width + 1.5*tile_width, tile_width + 4*i*tile_width + 1.5*tile_width), gate_color, 16));
    }

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

        // User can use arrow keys or AWSD or translate view of scene
        if (input.keyPressed(sf::Keyboard::A) || input.keyPressed(sf::Keyboard::Left)) {
            scene_view.move(-tile_width*3, 0.0f);
        }
        if (input.keyPressed(sf::Keyboard::D) || input.keyPressed(sf::Keyboard::Right)) {
            scene_view.move(tile_width*3, 0.0f);
        }
        if (input.keyPressed(sf::Keyboard::W) || input.keyPressed(sf::Keyboard::Up)) {
            scene_view.move(0.0f, -tile_width*3);
        }
        if (input.keyPressed(sf::Keyboard::S) || input.keyPressed(sf::Keyboard::Down)) {
            scene_view.move(0.0f, tile_width*3);
        }

        // User can zoom using mouse wheel
        if (input.mmb_delta > 0) {
            scene_view.zoom(0.5f);
        }
        else if (input.mmb_delta < 0) {
            scene_view.zoom(2.0f);
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

        // Generate a right-angled wire visual to trace the connection the user is hovering over
        if (wiring_gate) {
            wire_previews.clear();
            joint_previews.clear();
            Vec2 a = wiring_gate->sprite.getPosition() + wiring_gate->output_position;
            Vec2 b = cursor.getPosition() + Vec2(0.0f, tile_width/2.0f);
            if (a.y == b.y) {
                wire_previews.push_back(makeLine(a, b, sf::Color(0, 0, 255, 155)));
            }
            else {
                wire_previews.push_back(makeLine(a, Vec2((a.x + b.x)/2.0f, a.y), sf::Color(0, 0, 255, 155)));
                wire_previews.push_back(makeLine(Vec2((a.x + b.x)/2.0f, a.y), Vec2((a.x + b.x)/2.0f, b.y), sf::Color(0, 0, 255, 155)));
                wire_previews.push_back(makeLine(Vec2((a.x + b.x)/2.0f, b.y), b, sf::Color(0, 0, 255, 155)));
                joint_previews.push_back(makeCircle(Vec2((a.x + b.x)/2.0f, a.y), 5.0f, sf::Color(0, 0, 255, 155)));
                joint_previews.push_back(makeCircle(Vec2((a.x + b.x)/2.0f, b.y), 5.0f, sf::Color(0, 0, 255, 155)));
            }
        }

        // Create new gate at cursor position if user clicks
        if (input.lmb_released && !wiring_gate) {
            gates.push_back((*factory)());
            gates[gates.size() - 1]->sprite.setPosition(cursor.getPosition());
        }
        else if (input.lmb_released) {
            // Left click to abort a wire creation
            wiring_gate = nullptr;
            cursor.setFillColor(sf::Color(0, 255, 0, 155));
        }

        // Right click on gate output to begin creating a wire
        if (input.rmb_released && !wiring_gate) {
            for (auto each : gates) {

                if (each == output_gate)    continue;

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
            // Right click on gate input to place a wire and store the visual generated above
            for (auto each : gates) {
                for (int i = 0; i < each->input_count; i++) {
                    sf::Vector2f input_position = each->sprite.getPosition() + *(each->input_position + i);
                    if (floor(input.scene_mouse.x/tile_width) == floor(input_position.x/tile_width) &&
                        floor(input.scene_mouse.y/tile_width) == floor(input_position.y/tile_width))
                    {
                        *(each->input + i) = wiring_gate;
                        wires.insert(wires.end(), wire_previews.begin(), wire_previews.end());
                        joints.insert(joints.end(), joint_previews.begin(), joint_previews.end());
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

        window.setView(scene_view);

        for (auto each : gates) {
            window.draw(each->sprite);
        }
        for (auto each: labels) {
            window.draw(each);
        }
        for (auto each: wires) {
                window.draw(each);
            }
            for (auto each: joints) {
                window.draw(each);
            }
        if (wiring_gate) {
            for (auto each: wire_previews) {
                window.draw(each);
            }
            for (auto each: joint_previews) {
                window.draw(each);
            }
        }
        window.draw(cursor);

        window.display();
    }

    return 0;
}
