#include "Geometry.h"
// #include "GeometryGeneric.h"

#include <glm/glm.hpp>
#include <sstream>
std::ostream& operator<<(std::ostream& os, glm::mat4 const& m) {
	os << "\n";
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			os << m[i][j] << "\t";
		}
		os << "\n";
	}
	return os;
}

std::string matrixToString(glm::mat4 const& m) {
	std::stringstream ss;
	ss << m;

	return ss.str();
}

// TODO: Move to test folder...
// Currently tests geometry and geometrygeneric
namespace rocket { namespace util {

/*
// Template specializations for glm::vec3
template <>
struct PointTraits<glm::vec3> {
	typedef float value_type;
	enum { element_count = 3 };
};

template <uint32_t index>
typename PointTraits<glm::vec3>::value_type get(glm::vec3 const& point) {
	static_assert(index >= 0, "index < 0");
	static_assert(index < PointTraits<glm::vec3>::element_count, "index >= element size");

	if (index == 0) {
		return point.x;
	} else if (index == 1) {
		return point.y;
	} else { // index == 2 {
		return point.z;
	}
}

template <uint32_t index>
void set(glm::vec3 &point, typename PointTraits<glm::vec3>::value_type const& value) {
	static_assert(index >= 0, "index < 0");
	static_assert(index < PointTraits<glm::vec3>::element_count, "index >= element size");

	if (index == 0) {
		point.x = value;
	} else if (index == 1) {
		point.y = value;
	} else { // index == 2 {
		point.z = value;
	}
}

void geometryTest() {
	std::vector<glm::vec3> points;
	points.push_back(glm::vec3(1.0, 2.0, 3.0));
	points.push_back(glm::vec3(-1.0, 2.0, 3.0));
	points.push_back(glm::vec3(1.0, -2.0, 3.0));
	points.push_back(glm::vec3(1.0, 2.0, -3.0));
	points.push_back(glm::vec3(1.0, -2.0, -3.0));
	points.push_back(glm::vec3(-1.0, -2.0, 3.0));

	createAABB<glm::vec3, glm::vec3>(points);
}
*/
/*
void geometryTest() {
	std::vector<Point> points;
	points.push_back(createPoint(1.0, 2.0, 3.0));
	points.push_back(createPoint(-1.0, 2.0, 3.0));
	points.push_back(createPoint(1.0, -2.0, 3.0));
	points.push_back(createPoint(1.0, 2.0, -3.0));
	points.push_back(createPoint(1.0, -2.0, -3.0));
	points.push_back(createPoint(-1.0, -2.0, 3.0));

	auto aabb = createAABB(points.begin(), points.end());
}
*/
}}
