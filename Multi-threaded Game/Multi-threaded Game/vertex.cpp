#include "vertex.h"
#include "connection.h"
#include <list>

bool compare_distances::operator() (const Vertex *lhs, const Vertex *rhs) const{
	if (lhs == rhs)
	{
		return (lhs < rhs); // This check is needed as an item is determined equal if both < and !< return false.
	}
	if (lhs->f_cost == rhs->f_cost)
	{
		return (lhs < rhs); // This is needed so that nodes with the same f-cost can be added to the set, this will break ties in f-cost by comparing addresses.
	}
	return (lhs->f_cost < rhs->f_cost); // Otherwise, compare distances rather than addresses.
}

void Connect(Vertex& a, Vertex& b, float distance) {
	a.connections.push_back(Connection(&b, distance));
	b.connections.push_back(Connection(&a, distance));
}

// Updates each of this nodes connections with a new distance:
void UpdateDistances(Vertex& v, float distance)
{
	for (auto it = v.connections.begin(); it != v.connections.end(); ++it)
	{
		auto connection = &(*it);
		connection->distance = distance;
	}

}

// Operator compares coordinates of verticies:
bool Vertex::operator!=(Vertex node)
{
	if ((node.coordinates_.x == coordinates_.x) && (node.coordinates_.y == coordinates_.y))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Vertex::operator==(Vertex node)
{
	if ((node.coordinates_.x == coordinates_.x) && (node.coordinates_.y == coordinates_.y))
	{
		return true;
	}
	else
	{
		return false;
	}
}
