#ifndef H_EDGE
#define H_EDGE

#include "vector2.h"

template <class T, typename Tdata>
class Edge
{
	public:
		using VertexType = Vector2<T, Tdata>;

		Edge(const VertexType &p1, const VertexType &p2) : p1(p1), p2(p2) {};
		Edge(const Edge &e) : p1(e.p1), p2(e.p2) {};

		VertexType p1;
		VertexType p2;
};

template <class T, typename Tdata>
inline std::ostream &operator << (std::ostream &str, Edge<T, Tdata> const &e)
{
	return str << "Edge " << e.p1 << ", " << e.p2;
}

template <class T, typename Tdata>
inline bool operator == (const Edge<T, Tdata> & e1, const Edge<T, Tdata> & e2)
{
	return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
			(e1.p1 == e2.p2 && e1.p2 == e2.p1);
}

#endif

