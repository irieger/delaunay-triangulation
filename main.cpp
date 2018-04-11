#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <array>

#include <SFML/Graphics.hpp>

#include "vector2.h"
#include "triangle.h"
#include "delaunay.h"

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int main()
{
	// for lut data do cleanup first (point-dist < interp resolution -> preclean)

	srand (time(NULL));
	float numberPoints = roundf(RandomFloat(4, 40));

	std::cout << "Generating " << numberPoints << " random points" << std::endl;

	std::vector<Vector2<float, float>> points;
	for(int i = 0; i < numberPoints; i++) {
		points.push_back(Vector2<float, float>(RandomFloat(0, 800), RandomFloat(0, 600), RandomFloat(-10, 500)));
	}

	std::vector<Vector2<float, float>> interp_pts;
	numberPoints = roundf(RandomFloat(2, 5));
	for(int i = 0; i < numberPoints; i++) {
		interp_pts.push_back(Vector2<float, float>(RandomFloat(0, 800), RandomFloat(0, 600)));
	}

	Delaunay<float, float> triangulation;
	triangulation.setNotFoundReturn(NAN);
	std::vector<Triangle<float, float>> triangles = triangulation.triangulate(points);
	std::cout << triangles.size() << " triangles generated\n";
	std::vector<Edge<float, float>> edges = triangulation.getEdges();

	std::cout << " ========= ";

	std::cout << "\nPoints : " << points.size() << std::endl;
	for(auto &p : points)
		std::cout << p << std::endl;

	std::cout << "\nTriangles : " << triangles.size() << std::endl;
	for(auto &t : triangles)
		std::cout << t << std::endl;

	std::cout << "\nEdges : " << edges.size() << std::endl;
	for(auto &e : edges)
		std::cout << e << std::endl;

	std::cout << " ========  Grid interpolation: " << std::endl;
	uint32_t grid_size = 8;
	auto grid = triangulation.createGridInterpolation(grid_size, 0.0, 800);
	for (uint32_t i = 0; i < grid->size(); i++)
	{
		std::cout << "  x: " << i % grid_size << ", y: " << i / grid_size << ",   value: "
				  << grid->at(i) << std::endl;
	}

	// SFML window
    	sf::RenderWindow window(sf::VideoMode(800, 600), "Delaunay triangulation");

	// Transform each points of each vector as a rectangle
	std::vector<sf::RectangleShape*> squares;

	for(auto p = begin(points); p != end(points); p++) {
		sf::RectangleShape *c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
		c1->setPosition(p->x, p->y);
		squares.push_back(c1);
	}

	sf::Color ptcolor(255, 255, 0);
	for(auto p = begin(interp_pts); p != end(interp_pts); p++) {
		sf::RectangleShape *c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
		c1->setPosition(p->x, p->y);
		c1->setFillColor(ptcolor);
		// c1->setOutlineColor(ptcolor);
		squares.push_back(c1);

		auto tmp = triangulation.interpolateValue(*p);
		std::cout << "Interpolation for point " << *p << ":\n        " << tmp << std::endl;
	}

	ptcolor = sf::Color(0, 0, 255);
	for (uint32_t i = 0; i < grid->size(); i++)
	{
		if (!std::isnan(grid->at(i)))
		{
			const float x = (i % grid_size) * (798.0 / (grid_size - 1));
			const float y = (i / grid_size) * (798.0 / (grid_size - 1));

			sf::RectangleShape *c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
			c1->setPosition(x, y);
			c1->setFillColor(ptcolor);
			// c1->setOutlineColor(ptcolor);
			squares.push_back(c1);
		}
	}

	// Make the lines
	std::vector<std::array<sf::Vertex, 2> > lines;
	for(auto e = begin(edges); e != end(edges); e++) {
		lines.push_back({{
			sf::Vertex(sf::Vector2f((*e).p1.x + 2, (*e).p1.y + 2)),
			sf::Vertex(sf::Vector2f((*e).p2.x + 2, (*e).p2.y + 2))
		}});
	}

	while (window.isOpen())
	{
	        sf::Event event;
	        while (window.pollEvent(event))
	        {
	            if (event.type == sf::Event::Closed)
	                window.close();
	        }

	        window.clear();

		// Draw the squares
		for(auto s = begin(squares); s != end(squares); s++) {
			window.draw(**s);
		}

		// Draw the lines
		for(auto l = begin(lines); l != end(lines); l++) {
			window.draw((*l).data(), 2, sf::Lines);
		}

		window.display();
	}

	return 0;
}
