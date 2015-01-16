#include "../src/common/resource/image/ImageAtlas.h"
#include "../src/common/resource/FSResourcePackage.h"
#include "../src/common/Application.h"
#include "../src/platform/host/OpenAlPlayer.h"

#include <rocket/Log.h>

using namespace rocket;

int main() {
	// Setup application state.
	std::shared_ptr<ResourcePackage> rp = std::make_shared<FSResourcePackage>("./tests/assets");
	ResourceManager rm {"assets"};
	rm.addResourcePackage("assets", rp);
	Application::init(std::move(rm), std::unique_ptr<PlatformAudioPlayer>(new OpenAlPlayer()));

	auto ia = ImageAtlas(ResourceId("./test_atlas.json"));
	for (auto const& it : ia.getImages()) {
		LOGD(it.first << ", " << it.second.getResourceId() <<
				", x=" << it.second.getX() <<
				", y=" << it.second.getY() <<
				", width=" << it.second.getWidth() << 
				", height=" << it.second.getHeight());
	}
}
