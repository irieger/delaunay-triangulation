#ifndef H_DELAUNAY
#define H_DELAUNAY

#include "vector2.h"
#include "edge.h"
#include "triangle.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

template <class T, typename Tdata>
class Delaunay
{
	public:
		using TriangleType = Triangle<T, Tdata>;
		using EdgeType = Edge<T, Tdata>;
		using VertexType = Vector2<T, Tdata>;

		const std::vector<TriangleType>& triangulate(std::vector<VertexType> &vertices)
		{
			// Store the vertices localy
			_vertices = vertices;

			// Determinate the super triangle
			float minX = vertices[0].x;
			float minY = vertices[0].y;
			float maxX = minX;
			float maxY = minY;

			for(std::size_t i = 0; i < vertices.size(); ++i)
			{
				if (vertices[i].x < minX) minX = vertices[i].x;
		    	if (vertices[i].y < minY) minY = vertices[i].y;
		    	if (vertices[i].x > maxX) maxX = vertices[i].x;
		    	if (vertices[i].y > maxY) maxY = vertices[i].y;
			}

			float dx = maxX - minX;
			float dy = maxY - minY;
			float deltaMax = std::max(dx, dy);
			float midx = (minX + maxX) / 2.f;
			float midy = (minY + maxY) / 2.f;

			VertexType p1(midx - 20 * deltaMax, midy - deltaMax);
			VertexType p2(midx, midy + 20 * deltaMax);
			VertexType p3(midx + 20 * deltaMax, midy - deltaMax);

			//std::cout << "Super triangle " << std::endl << Triangle(p1, p2, p3) << std::endl;

			// Create a list of triangles, and add the supertriangle in it
			_triangles.push_back(TriangleType(p1, p2, p3));

			for(auto p = begin(vertices); p != end(vertices); p++)
			{
				//std::cout << "Traitement du point " << *p << std::endl;
				//std::cout << "_triangles contains " << _triangles.size() << " elements" << std::endl;

				std::vector<TriangleType> badTriangles;
				std::vector<EdgeType> polygon;

				for(auto t = begin(_triangles); t != end(_triangles); t++)
				{
					//std::cout << "Processing " << std::endl << *t << std::endl;

					if(t->circumCircleContains(*p))
					{
						//std::cout << "Pushing bad triangle " << *t << std::endl;
						badTriangles.push_back(*t);
						polygon.push_back(t->e1);
						polygon.push_back(t->e2);
						polygon.push_back(t->e3);
					}
					else
					{
						//std::cout << " does not contains " << *p << " in his circum center" << std::endl;
					}
				}

				_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [badTriangles](TriangleType &t){
					for(auto bt = begin(badTriangles); bt != end(badTriangles); bt++)
					{
						if(*bt == t)
						{
							//std::cout << "Removing bad triangle " << std::endl << *bt << " from _triangles" << std::endl;
							return true;
						}
					}
					return false;
				}), end(_triangles));

				std::vector<EdgeType> badEdges;
				for(auto e1 = begin(polygon); e1 != end(polygon); e1++)
				{
					for(auto e2 = begin(polygon); e2 != end(polygon); e2++)
					{
						if(e1 == e2)
							continue;

						if(*e1 == *e2)
						{
							badEdges.push_back(*e1);
							badEdges.push_back(*e2);
						}
					}
				}

				polygon.erase(std::remove_if(begin(polygon), end(polygon), [badEdges](EdgeType &e){
					for(auto it = begin(badEdges); it != end(badEdges); it++)
					{
						if(*it == e)
							return true;
					}
					return false;
				}), end(polygon));

				for(auto e = begin(polygon); e != end(polygon); e++)
					_triangles.push_back(TriangleType(e->p1, e->p2, *p));

			}

			_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](TriangleType &t){
				return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
			}), end(_triangles));

			for(auto t = begin(_triangles); t != end(_triangles); t++)
			{
				_edges.push_back(t->e1);
				_edges.push_back(t->e2);
				_edges.push_back(t->e3);
			}

			return _triangles;
		}


		void setNotFoundReturn(const Tdata &data)
		{
			this->_not_found_return = data;
		}


		Tdata interpolateValue(const float px, const float py)
		{
			for (auto &triangle : _triangles)
			{
				if (triangle.isEncircling(px, py))
				{
					// std::cout << "  interpolateValue  at (" << px << ", " << py << ")  in " << triangle << std::endl;
					VertexType* p1 = &triangle.p1;
					VertexType* p2 = &triangle.p2;
					VertexType* p3 = &triangle.p3;

					float x = p1->x - px;
					float y = p1->y - py;
					double weight1 = 1 / std::sqrt(x * x + y * y);

					x = p2->x - px;
					y = p2->y - py;
					double weight2 = 1 / std::sqrt(x * x + y * y);

					x = p3->x - px;
					y = p3->y - py;
					double weight3 = 1 / std::sqrt(x * x + y * y);

					double sum = weight1 + weight2 + weight3;

					weight1 = weight1 / sum;
					weight2 = weight2 / sum;
					weight3 = weight3 / sum;

					// std::cout << " > Interpolate: " << weight1 << " * " << *p1 << std::endl;
					// std::cout << " > Interpolate: " << weight2 << " * " << *p2 << std::endl;
					// std::cout << " > Interpolate: " << weight3 << " * " << *p3 << std::endl;

					return p1->value * weight1 + p2->value * weight2 + p3->value * weight3;
				}
			}
			return this->_not_found_return;
		};


		Tdata interpolateValue(const VertexType &point)
		{
			return this->interpolateValue(point.x, point.y);
		}


		std::shared_ptr<std::vector<Tdata>> createGridInterpolation(const uint32_t grid_size = 2048,
						const float grid_min = 0.0, const float grid_max = 1.0)
		{
			auto grid = std::make_shared<std::vector<Tdata>>(grid_size * grid_size);

			float mult = (grid_max - grid_min) / (grid_size - 1);

			#pragma omp parallel for
			for (uint32_t y = 0; y < grid_size; y++)
			{
				float ypos = grid_min + y * mult;
				uint32_t y_offset = y * grid_size;
				for (uint32_t x = 0; x < grid_size; x++)
				{
					(*grid)[y_offset + x] = this->interpolateValue(grid_min + x * mult, ypos);
				}
			}

			return grid;
		};


		const std::vector<TriangleType>& getTriangles() const { return _triangles; };
		const std::vector<EdgeType>& getEdges() const { return _edges; };
		const std::vector<VertexType>& getVertices() const { return _vertices; };

	private:
		std::vector<TriangleType> _triangles;
		std::vector<EdgeType> _edges;
		std::vector<VertexType> _vertices;

		Tdata _not_found_return;
};

#endif
