#ifndef H_TRIANGLE
#define H_TRIANGLE

#include "vector2.h"
#include "edge.h"

#include <assert.h>
#include <math.h>

template <class T, typename Tdata>
class Triangle
{
	public:
		using EdgeType = Edge<T, Tdata>;
		using VertexType = Vector2<T, Tdata>;

		Triangle(const VertexType &_p1, const VertexType &_p2, const VertexType &_p3)
		:	p1(_p1), p2(_p2), p3(_p3),
			e1(_p1, _p2), e2(_p2, _p3), e3(_p3, _p1)
		{}

		bool containsVertex(const VertexType &v)
		{
			return p1 == v || p2 == v || p3 == v;
		}

		bool circumCircleContains(const VertexType &v)
		{
			float ab = (p1.x * p1.x) + (p1.y * p1.y);
			float cd = (p2.x * p2.x) + (p2.y * p2.y);
			float ef = (p3.x * p3.x) + (p3.y * p3.y);

			float circum_x = (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y)) / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y)) / 2.f;
			float circum_y = (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x)) / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x)) / 2.f;
			float circum_radius = sqrtf(((p1.x - circum_x) * (p1.x - circum_x)) + ((p1.y - circum_y) * (p1.y - circum_y)));

			float dist = sqrtf(((v.x - circum_x) * (v.x - circum_x)) + ((v.y - circum_y) * (v.y - circum_y)));
			return dist <= circum_radius;
		}

        // via https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
		bool isEncircling(const float x, float y)
		{
			float dX = x - p3.x;
    		float dY = y - p3.y;
            float dX32 = p3.x - p2.x;
            float dY23 = p2.y - p3.y;
            float D = dY23 * (p1.x - p3.x) + dX32 * (p1.y - p3.y);
            float s = dY23 * dX + dX32 * dY;
            float t = (p3.y - p1.y) * dX + (p1.x - p3.x) * dY;
    		if (D < 0)
        		return s <= 0  &&  t <= 0  &&  s + t >= D;
    		return     s >= 0  &&  t >= 0  &&  s + t <= D;
		}

		VertexType p1;
		VertexType p2;
		VertexType p3;
		EdgeType e1;
		EdgeType e2;
		EdgeType e3;
};

template <class T, typename Tdata>
inline std::ostream &operator << (std::ostream &str, const Triangle<T, Tdata> & t)
{
	return str << "Triangle:" << std::endl << "\t" << t.p1 << std::endl << "\t" << t.p2 << std::endl << "\t" << t.p3 << std::endl << "\t" << t.e1 << std::endl << "\t" << t.e2 << std::endl << "\t" << t.e3 << std::endl;

}

template <class T, typename Tdata>
inline bool operator == (const Triangle<T, Tdata> &t1, const Triangle<T, Tdata> &t2)
{
	return	(t1.p1 == t2.p1 || t1.p1 == t2.p2 || t1.p1 == t2.p3) &&
			(t1.p2 == t2.p1 || t1.p2 == t2.p2 || t1.p2 == t2.p3) &&
			(t1.p3 == t2.p1 || t1.p3 == t2.p2 || t1.p3 == t2.p3);
}


#endif
