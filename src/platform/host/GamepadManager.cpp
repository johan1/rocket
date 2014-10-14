#include "GamepadManager.h"

#include <rocket/Algorithm.h>
#include <rocket/Log.h>
#include <boost/filesystem.hpp>
#include <string>

using namespace boost::filesystem;

namespace rocket {

GamepadManager::GamepadManager() {}

void GamepadManager::detectDevices() {
	path devices("/dev/input");
	for (auto it = directory_iterator(devices); it != directory_iterator{}; ++it) {
		auto filename = it->path().filename().string();
		if (!filename.compare(0, 2, "js")) {
			auto devicePath = it->path().string();
			if (find(devicePaths, devicePath) == devicePaths.end()) {
				uint32_t controllerId = std::stoi(filename.substr(2, 1));

				try {
					GamepadController controller{controllerId, devicePath};
					LOGD("Added controller " << devicePath << " { name=" << controller.getName() <<
							", driver-version=" << controller.getDriverVersion() <<
							", #axes=" << static_cast<int>(controller.getNumberOfAxes()) <<
							", #buttons=" << static_cast<int>(controller.getNumberOfButtons()) <<
							" }");
					controllers.push_back(std::move(controller));
					devicePaths.push_back(devicePath);
				} catch (GamepadUnavailableException e) {
					// Controller not yet available
				}
			}
		}
	}
}

boost::optional<ControllerEvent> GamepadManager::pollControllers() {
	detectDevices();

	auto it = controllers.begin();
	while (it != controllers.end()) {
		try {
			auto event = it->readEvent();
			++it;
			if (event) {
				return event;
			}
		} catch (GamepadUnavailableException const& e) {
			LOGD("Removed controller " << it->getDevicePath());
			erase(devicePaths, it->getDevicePath());
			it = controllers.erase(it);
		}
	}
	//}

	return boost::optional<ControllerEvent>();
}

}
