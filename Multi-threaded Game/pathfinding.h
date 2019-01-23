// Constants and typedefs for the original pathfinding app:
#pragma once
#include "vertex.h"
#include <SFML/Graphics.hpp>

typedef unsigned int UInt32;
typedef std::vector<std::vector<Vertex*>> Grid; // This is a vector of vectors, like a 2d array of vertices.
const float kSquareRoot2 = 1.41421356237f; // Following the google C++ style guide convention for naming constants.
const float kDiagonalDistance = 52.9116882454f;
const sf::Color colour_blocked = sf::Color(0x66, 0x66, 0x66, 0xFF);
const sf::Color colour_open_set = sf::Color(0x00, 0x33, 0xCC, 0x66);
const sf::Color colour_closed_set = sf::Color(0x99, 0xFF, 0xCC, 0x66);

