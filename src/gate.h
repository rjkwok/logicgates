#ifndef LG_GATE
#define LG_GATE

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

typedef sf::Vector2f Vec2;

class Gate {
    
public:

    Gate(sf::Texture& texture, const int input_count, Vec2* input_position, Vec2 output_position, bool (*logic_operation)(Gate**)) 
        : sprite(texture)
        , input_count(input_count)
        , input_position(input_position)
        , output_position(output_position)
        , logic_operation(logic_operation)
    {
        input = new Gate*[input_count];    
        for (int i = 0; i < input_count; i++) {
            *(input + i) = nullptr;
        }
    }

    ~Gate() 
    {
        delete input;
        delete input_position;
    }

    bool operator()() 
    { 
        return logic_operation(input); 
    }   

    bool (*logic_operation)(Gate**);

    Gate**      input;
    Vec2*       input_position;
    int         input_count;
    Vec2        output_position;
    sf::Sprite  sprite;
};

bool BUFFER(Gate** input) {
    return (*input ? (**input)() : 0);
}

bool ONE(Gate** input) {
    return true;
}

bool ZERO(Gate** input) {
    return false;
}

bool OR(Gate** input) {
    return (*input ? (**input)() : 0) || (*(input + 1) ? (**(input + 1))() : 0);
}

bool AND(Gate** input) {
    return (*input ? (**input)() : 0) && (*(input + 1) ? (**(input + 1))() : 0);
}

bool NOT(Gate** input) {
    return !(*input ? (**input)() : 0);
}

#endif