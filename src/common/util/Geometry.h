#ifndef _ROCKET2D_GEOMETRY_H_
#define _ROCKET2D_GEOMETRY_H_

#include <cassert>

#include <boost/optional.hpp>
#include <glm/glm.hpp>
#include <ostream>

namespace rocket { namespace util {
	typedef glm::vec4 Point;
	typedef glm::vec3 Dimension;
	typedef glm::mat4 Matrix;

inline
Point createPoint(float x, float y) {
	return Point(x, y, 0, 1.0f);
}

inline
Point createPoint(float x, float y, float z) {
	return Point(x, y, z, 1.0f);
}

inline
Point createPoint(glm::vec2 v) {
	return Point(v.x, v.y, 0, 1.0f);
}

inline
Point createPoint(glm::vec3 v) {
	return Point(v.x, v.y, v.z, 1.0f);
}

inline
Dimension newDimension(float w, float h, float d) {
	return Dimension(w, h, d);
}

inline 
float distance2(Point const& p1, Point const& p2) {
	return (p1.x-p2.x)*(p1.x-p2.x) +
		(p1.y-p2.y)*(p1.y-p2.y) +
		(p1.z-p2.z)*(p1.z-p2.z);
}

inline 
float distance(Point const& p1, Point const& p2) {
	return std::sqrt(distance2(p1, p2));
}

inline
bool allElementsLess(Point const& p1, Point const& p2) {
	return (p1.x < p2.x) && (p1.y < p2.y) && (p1.z < p2.z);
}

struct Sphere {
	Point center;
	float radius;
	
	Sphere() : center(createPoint(0, 0, 0)), radius(0) {}
	Sphere(Point const& center, float radius) : center(center), radius(radius) {}

	bool contains(Point const& otherPoint) {
		return distance2(center, otherPoint) < (radius*radius);
	}

	bool intersects(Sphere const& other) {
		return distance2(center, other.center) < (radius+other.radius)*(radius+other.radius);
	}
};

struct AABox {
	Point p1;
	Point p2;

	AABox() : p1(Point()), p2(Point()) {}

	AABox(Point const& p1, Point const& p2) : p1(p1), p2(p2) {}

	Point const& getP1() const {
		return p1; 
	}

	Point const& getP2() const {
		return p2;
	}

	void changeP1(Point const& p) {
		if (allElementsLess(p2, p)) {
			std::abort();
		}
		p1 = p;
	}

	void changeP2(Point const& p) {
		if (allElementsLess(p, p1)) {
			std::abort();
		}
		p2 = p;
	}

	bool contains(Point const& p) {
		if ((p.x >= p1.x && p.y >= p1.y && p.z >= p1.z) &&
				(p.x <= p2.x && p.y <= p2.y && p.z <= p2.z)) {
			return true;
		}

		return false;
	}

	bool intersects(AABox const& other) const {
		if (other.p2.x < p1.x || other.p1.x > p2.x) {
			return false;
		}
		if (other.p2.y < p1.y || other.p1.y > p2.y) {
			return false;
		}
		if (other.p2.z < p1.z || other.p1.z > p2.z) {
			return false;
		}
		return true;
	}

	Dimension getDimension() const {
		return Dimension(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
	}
};

inline
boost::optional<AABox> intersection(AABox const& box1, AABox const& box2) {
	if (box1.intersects(box2)) {
		Point p1;
		Point p2;
		
		p1.x = std::max(box1.getP1().x, box2.getP1().x);
		p1.y = std::max(box1.getP1().y, box2.getP1().y);
		p1.z = std::max(box1.getP1().z, box2.getP1().z);

		p2.x = std::min(box1.getP2().x, box2.getP2().x);
		p2.y = std::min(box1.getP2().y, box2.getP2().y);
		p2.z = std::min(box1.getP2().z, box2.getP2().z);

		return AABox(p1, p2);
	}

	return boost::optional<AABox>();	
}

// Damn union keyword is taken, C. 
inline
AABox union_box(AABox const& box1, AABox const& box2) {
	Point p1;
	Point p2;
	
	p1.x = std::min(box1.getP1().x, box2.getP1().x);
	p1.y = std::min(box1.getP1().y, box2.getP1().y);
	p1.z = std::min(box1.getP1().z, box2.getP1().z);

	p2.x = std::max(box1.getP2().x, box2.getP2().x);
	p2.y = std::max(box1.getP2().y, box2.getP2().y);
	p2.z = std::max(box1.getP2().z, box2.getP2().z);

	return AABox(p1, p2);
}

template <typename ForwardIt>
// requires ForwardIt is a ForwardIterator, iterating over at least 1 Point elements
AABox createAABB(ForwardIt begin, ForwardIt end) {
	if (begin == end) {
		std::abort();
	}

	auto it = begin;
	Point p1 = *it;
	Point p2 = *it;
	++begin;

	for (; it != end; ++it) {
		p1.x = std::min(it->x, p1.x);
		p1.y = std::min(it->y, p1.y);
		p1.z = std::min(it->z, p1.z);
		p2.x = std::max(it->x, p2.x);
		p2.y = std::max(it->y, p2.y);
		p2.z = std::max(it->z, p2.z);
	}

	return AABox(p1, p2);
}

// inline, is it needed for constexpr?
constexpr float deg2rad(float deg) {
	return deg/360.0 * 2.0 * M_PI;
}

// inline
constexpr float rad2deg(float rad) {
	return rad/(2.0*M_PI) * 360.0;
}

}

using namespace util;
}

std::ostream& operator<<(std::ostream& os, glm::mat4 const& m);

std::string matrixToString(glm::mat4 const& m);

#endif
