#ifndef _ROCKET_GEOMETRY_MAP_H_
#define _ROCKET_GEOMETRY_MAP_H_

#include "Geometry.h"
#include <unordered_map>
#include "Assert.h"

// #define DEBUG_GM 1

#ifdef DEBUG_GM
#include "Log.h"
#include <iosfwd>
#endif

namespace rocket { namespace util {

namespace gmdetails {
	struct BoxId {
		int x;
		int y;
		int z;

		BoxId() : x(0), y(0), z(0) {}

		BoxId(int x, int y, int z) : x(x), y(y), z(z) {}

		friend bool operator==(BoxId const& box1, BoxId const& box2) {
 			return box1.x == box2.x && box1.y == box2.y && box1.z == box2.z;
		}

		friend bool operator!=(BoxId const& box1, BoxId const& box2) {
			return !(box1 == box2);
		}

#ifdef DEBUG_GM
		friend std::ostream& operator<<(std::ostream& os, BoxId const& box) {
			return os << "{x=" << box.x << ", y=" << box.y << ", z=" << box.z << "}";
		}
#endif
	};
}

//! A container class that groups elements in boxes according to their position.
template <typename Element>
// Requires Element EquallyComparable
// std::hash needs to be specialized for Element
class GeometryMap {
public:
	typedef glm::vec3 Point;

	GeometryMap() : boxDimension(Dimension(1, 1, 1)) {}
	GeometryMap(Dimension const& boxDimension) : boxDimension(boxDimension) {}

//	std::vector<Element> const& getLocal(Point const& p);

	void add(Element const& element, Point const& point) {
		elements[getBox(point)].push_back(element);
		elementLocations[element] = point;
	}

	bool contains(Element const& element) {
		return elementLocations.find(element) != elementLocations.end();
	}

	void remove(Element const& element) {
		ROCKET_ASSERT_TRUE(contains(element));

		auto& location = elementLocations[element];
		gmdetails::BoxId box = getBox(location);
		auto& elementContainer = elements[box];

		if (std::find(elementContainer.begin(), elementContainer.end(), element) == elementContainer.end()) {
			ROCKET_ASSERT_TRUE(false);
		}

		elementContainer.erase(std::remove(elementContainer.begin(), elementContainer.end(),
				element), elementContainer.end());
		if (elementContainer.empty()) {
			elements.erase(box);
		}

		elementLocations.erase(element);

		ROCKET_ASSERT_TRUE(!contains(element));
		// return true;
	}

	void move(Element const& element, Point const& newLocation) {
		auto& oldLocation = elementLocations[element];
		if (getBox(oldLocation) == getBox(newLocation)) {
			// Let's just update the element location
			elementLocations[element] = newLocation;
		} else {
			// Ok, we need to change box, we do this by removing and re-adding
			remove(element);
			add(element, newLocation);
		}
//		LOGD(boost::format("Moved element from %s to %s") % getBox(oldLocation) % getBox(newLocation));

//		return true;
	}

	void resize(Dimension const& boxDimension) {
		this->boxDimension = boxDimension;
		elements.clear();
		for (auto& elementInfo : elementLocations) {
			elements[getBox(elementInfo.second)].push_back(elementInfo.first);
		}
	}

	//! inserts elements from the box containing the point p and the elements from that box's neighbours, within a distance of depth.
	void get(Point const& p, int depth, std::vector<Element> &result) {
		auto b = getBox(p);

		for (int x = -1 * depth; x <= depth; ++x) {
			for (int y = -1 * depth; y <= depth; ++y) {
				for (int z = -1 * depth; z <= depth; ++z) {
					gmdetails::BoxId b2(b.x + x, b.y + y, b.z + z);
					auto& v = elements[b2];
					result.insert(result.end(), v.begin(), v.end());	
				}
			}
		}
	}

private:
	std::unordered_map<gmdetails::BoxId, std::vector<Element>> elements;
	std::unordered_map<Element, Point> elementLocations;

	Dimension boxDimension;

	gmdetails::BoxId getBox(Point const& p) {
		gmdetails::BoxId box;
		box.x = std::floor(p.x/boxDimension.x);
		box.y = std::floor(p.y/boxDimension.y);
		box.z = std::floor(p.z/boxDimension.z);
		return box;	
	}
};

}
using namespace util; // Collapse
}

namespace std {
    template<>
    struct hash<rocket::util::gmdetails::BoxId> {
		std::size_t operator()(rocket::util::gmdetails::BoxId const& b) const {
        	std::size_t hx = std::hash<int>()(b.x);
        	std::size_t hy = std::hash<int>()(b.y);
        	std::size_t hz = std::hash<int>()(b.z); // std::string>()(s.last_name);
        	return hx ^ (hy << 1) ^ (hz << 2);
		}
    };
}

#endif
