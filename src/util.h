#ifndef LG_UTIL
#define LG_UTIL

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

// atan2
#include <math.h>
#include <sstream>

template <typename T>
std::string asString(T number) {
    std::stringstream sstream;
    sstream << number;
    return sstream.str();
}

constexpr double pi = std::atan(1)*4;

sf::RectangleShape makeLine(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Color& color) {

    static const float thickness = 5.0f;

    float h = hypot(b.x - a.x, b.y - a.y);

    sf::RectangleShape line(sf::Vector2f(h, thickness));
    line.setOutlineThickness(0);
    line.setFillColor(color);
    line.setOrigin(0.0f, thickness/2.0f);
    line.setRotation(atan2(b.y - a.y, b.x - a.x)*180.0f/pi);
    line.setPosition(a);

    return line;
}

sf::CircleShape makeCircle(const sf::Vector2f a, float radius, const sf::Color& color) {

    sf::CircleShape circle(radius);
    circle.setOutlineThickness(0);
    circle.setFillColor(color);
    circle.setOrigin(radius, radius);
    circle.setPosition(a);

    return circle;
}

sf::Text makeText(const std::string& str, sf::Font& font, const sf::Vector2f& centre, const sf::Color& color, const unsigned int size) {

    sf::Text text(str, font, size);
    text.setColor(color);
    sf::FloatRect rect = text.getLocalBounds();
    text.setOrigin(rect.width/2.0f, rect.height/2.0f);
    text.setPosition(centre);

    return text;
}

void populateTruthTable(const int total_inputs, std::vector<std::vector<bool> >& truth_table, std::vector<bool> base, int i) {

    std::vector<bool> f = base;
    std::vector<bool> t = base;
    f[i] = false;
    t[i] = true;
    if (i == total_inputs-1) {
        truth_table.push_back(f);
        truth_table.push_back(t);
    }
    else {
        populateTruthTable(total_inputs, truth_table, f, i + 1);
        populateTruthTable(total_inputs, truth_table, t, i + 1);
    }
}

std::string truthTableAsString(std::vector<std::vector<bool> >& truth_table, const int total_inputs) {

    std::stringstream sstream;
    sstream << "|";
    for (int j = 0; j < total_inputs; j++) {
        sstream << " X" << (j + 1) << " |";
    }
    sstream << " Y' | Y  |";

    sstream << "\n";

    for (int i = 0; i < truth_table.size(); i++) {
        sstream << "|";
        for (int j = 0; j < truth_table[i].size(); j++) {
            sstream << " " << truth_table[i][j] << "  |";
        }

        sstream << "\n";
    }
    return sstream.str();
}

struct Input {

    Input() = default;

    void collect(sf::RenderWindow& window, sf::View& scene_view){

        mouse = sf::Mouse::getPosition(window);
        scene_mouse = window.mapPixelToCoords(mouse, scene_view);

        mmb_delta = 0.0f;
        lmb_released = false;
        rmb_released = false;
        lmb_held = false;
        rmb_held = false;

        keys_released.clear();
        keys_pressed.clear();
        keys_held.clear();

        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseWheelMoved:
                    mmb_delta = event.mouseWheel.delta;
                    break;

                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button==sf::Mouse::Left)          lmb_released = true;
                    else if (event.mouseButton.button==sf::Mouse::Right)    rmb_released = true;
                    break;

                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button==sf::Mouse::Left)          lmb_held = true;
                    else if (event.mouseButton.button==sf::Mouse::Right)    rmb_held = true;
                    break;

                case sf::Event::KeyReleased:
                    keys_released.push_back(event.key.code);
                    break;

                case sf::Event::KeyPressed:
                    keys_pressed.push_back(event.key.code);
                    break;
            }
        }

        for (int key = sf::Keyboard::A; key != sf::Keyboard::KeyCount; ++key) {

            if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)key)) keys_held.push_back((sf::Keyboard::Key)key);
        }
    }

    bool keyPressed(const sf::Keyboard::Key& key) const {
        return std::find(keys_pressed.begin(), keys_pressed.end(), key) != keys_pressed.end();
    }

    bool keyHeld(const sf::Keyboard::Key& key) const {
        return std::find(keys_held.begin(), keys_held.end(), key) != keys_held.end();
    }

    bool keyReleased(const sf::Keyboard::Key& key) const {
        return std::find(keys_released.begin(), keys_released.end(), key) != keys_released.end();
    }

    sf::Vector2i mouse;
    sf::Vector2f scene_mouse;

    bool lmb_released;
    bool lmb_held;
    bool rmb_released;
    bool rmb_held;

    float mmb_delta;

    std::vector<sf::Keyboard::Key> keys_released;
    std::vector<sf::Keyboard::Key> keys_held;
    std::vector<sf::Keyboard::Key> keys_pressed;
};

#endif
