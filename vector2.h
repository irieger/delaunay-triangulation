#ifndef H_VECTOR2
#define H_VECTOR2

#include <iostream>
#include <cmath>

template <typename T, typename Tdata>
class Vector2
{
	public:
		//
		// Constructors
		//

		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2(T _x, T _y)
		{
			x = _x;
			y = _y;
		}

		Vector2(T _x, T _y, Tdata _data)
		{
			x = _x;
			y = _y;
			value = _data;
		}

		Vector2(const Vector2 &v)
		{
			x = v.x;
			y = v.y;
			value = v.value;
		}

		void set(const Vector2 &v)
		{
			x = v.x;
			y = v.y;
			value = v.value;
		}

		//
		// Operations
		//
		T dist2(const Vector2 &v)
		{
			T dx = x - v.x;
			T dy = y - v.y;
			return dx * dx + dy * dy;
		}

		float dist(const Vector2 &v)
		{
			return sqrtf(dist2(v));
		}

		T x;
		T y;


		Tdata value;

};

template<typename T, typename Tdata>
std::ostream &operator << (std::ostream &str, Vector2<T, Tdata> const &point)
{
	return str << "Point x: " << point.x << " y: " << point.y << " value: " << point.value;
}

template<typename T, typename Tdata>
bool operator == (Vector2<T, Tdata> v1, Vector2<T, Tdata> v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}

#endif
