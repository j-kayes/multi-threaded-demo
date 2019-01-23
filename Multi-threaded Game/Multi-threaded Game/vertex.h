#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <list> 
#include "connection.h"
#include "pathfinding.h"

struct Connection;
struct Coordinates {
	int x, y;
	Coordinates(int x, int y) :
		x(x), y(y) {};
};

struct Vertex {
	std::string name;
	Coordinates coordinates_;
	// These values store the verticies coordinates in screen space:
	float screen_x;
	float screen_y;
	bool blocked;
	float g_cost; // Distance to this node.
	float h_cost; // Estimated distance from this node to the end node (for dijkstras algorithm, this is always 0).
	float f_cost; // Sum of the above, this gives an indication of what node to look at next.
	std::vector<Connection>  connections; // I use this in place of the neighbours variable to represent the edges/connections to other nodes.
	Vertex *parent;
	Vertex(std::string name_, int x, int y)
		: name(name_), coordinates_(Coordinates(x, y)), screen_x(x*36.0f + 2.0f), screen_y(y*36.0f), blocked(false), g_cost(std::numeric_limits<float>::infinity()), h_cost(0), f_cost(g_cost + h_cost) {};
	Vertex(int x, int y)
		: name(""), coordinates_(Coordinates(x, y)), screen_x(x*36.0f + 2.0f), screen_y(y*36.0f), blocked(false), g_cost(std::numeric_limits<float>::infinity()), h_cost(0), f_cost(g_cost + h_cost) {};
	Vertex()
		: name(""), coordinates_(Coordinates(0, 0)), blocked(false), g_cost(std::numeric_limits<float>::infinity()), h_cost(0), f_cost(g_cost + h_cost) {};

	bool operator!=(Vertex node);
	bool operator==(Vertex node);
};

struct compare_distances
{
	bool operator() (const Vertex* lhs, const Vertex* rhs) const; // A functor, for use in sorting the set of vertices.
};

void Connect(Vertex& a, Vertex& b, float distance);
void UpdateDistances(Vertex& v, float distance);