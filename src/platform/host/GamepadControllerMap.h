#ifndef _ROCKET_GAMEPAD_CONTROLLER_MAPS_H_
#define _ROCKET_GAMEPAD_CONTROLLER_MAPS_H_

#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <utility>

namespace std {

template <>
class hash<std::pair<uint8_t, bool>> {
public:
    size_t operator()(const pair<uint8_t, bool> &x ) const {
        return std::hash<uint8_t>()(x.first) + std::hash<bool>()(x.second);
    }
};

}

namespace rocket {

class GamepadControllerMap {
public:
	boost::optional<uint32_t> buttonIdFromAxis(uint8_t id, bool positive) const {
		auto p = std::make_pair(id, positive);
		auto it = axisMapping.find(p);
		if (it != axisMapping.end()) {
			return it->second;
		} else {
			return boost::optional<uint32_t>();
		}
	}

	boost::optional<uint32_t> buttonIdFromButton(uint8_t id) const {
		auto it = buttonsMapping.find(id);
		if (it != buttonsMapping.end()) {
			return it->second;
		} else {
			return boost::optional<uint32_t>();
		}
	}

	void mapButton(uint8_t id, uint32_t buttonId) {
		buttonsMapping[id] = buttonId;
	}

	void mapAxis(uint8_t id, bool positive, uint32_t buttonId) {
		axisMapping[std::make_pair(id, positive)] = buttonId;
	}

private:
	std::unordered_map<std::pair<uint8_t, bool>, uint32_t> axisMapping;
	std::unordered_map<uint8_t, uint32_t> buttonsMapping;
};

boost::optional<GamepadControllerMap> lookupControllerMapping(std::string const& deviceName);

}

#endif
