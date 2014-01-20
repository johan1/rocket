#ifndef _ROCKET_CONTROLLER_H_
#define _ROCKET_CONTROLLER_H_

#include <cstdint>
#include <string>
#include <unordered_map>

namespace rocket { namespace input {

class Controller {
public:
	Controller(std::string const& name);
	Controller(Controller &&) = default;
	Controller& operator=(Controller &&) = default;

	uint8_t getButtonValue(uint32_t buttonId) const;
	void setButtonValue(uint32_t buttonId, uint8_t value);

private:
	std::string name;
	std::unordered_map<uint32_t, uint8_t> buttonState;
};

}}

#endif
