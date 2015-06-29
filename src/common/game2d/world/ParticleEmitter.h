#ifndef _ROCKET_PARTICLE_EMITTER_H_
#define _ROCKET_PARTICLE_EMITTER_H_

#include "Renderable.h"

#include "../../graphics/Canvas.h"
#include "../../resource/image/ImageId.h"
#include "ParticleGenerator.h"

#include <glm/glm.hpp>

#include <functional>
#include <random>
#include <tuple>
#include <vector>

namespace rocket { namespace game2d {

class ParticleEmitter : public Renderable {
public:
	ParticleEmitter(std::shared_ptr<ParticleGenerator> const& generator,
			resource::ResourceId const& particleTexture, float unitSize, uint32_t numberOfParticles);
	virtual ~ParticleEmitter() = default;

	void start();
	bool isStarted() { return emitting; }
	void stop();

	void setUnitSize(float unitSize) {
		this->unitSize = unitSize;
	}

private:
	struct ParticleData {
		ParticleData() {}

		ParticleData(Particle const& particle, glm::mat4 const& modelMatrix, float timestamp) :
				p1(modelMatrix*particle.p1), p2(modelMatrix*particle.p2),
				c1(particle.c1), c2(particle.c2),
				s1(particle.s1), s2(particle.s2), timestamp(timestamp), duration(particle.duration) {}

		glm::vec4 p1;
		glm::vec4 p2;
		glutils::RGBAColor c1;
		glutils::RGBAColor c2;
		float s1; // Size in unitSize
		float s2; // Size in unitSize
		float timestamp;
		float duration;
	};

	// Render members
	std::shared_ptr<glutils::Program> program;
	glutils::Location vpMatrixLocation = glutils::Location(Location::Type::Uniform, "vpMatrix");
	glutils::Location timeLocation = glutils::Location(Location::Type::Uniform, "time");
	glutils::Location samplerLocation = glutils::Location(Location::Type::Uniform, "sampler");
	glutils::Location c1Location = glutils::Location(Location::Type::Attribute, "c1");
	glutils::Location c2Location = glutils::Location(Location::Type::Attribute, "c2");
	glutils::Location timestampLocation = glutils::Location(Location::Type::Attribute, "timestamp");
	glutils::Location durationLocation = glutils::Location(Location::Type::Attribute, "duration");
	glutils::Location p1Location = glutils::Location(Location::Type::Attribute, "p1");
	glutils::Location p2Location = glutils::Location(Location::Type::Attribute, "p2");
	glutils::Location unitSizeLocation = glutils::Location(Location::Type::Uniform, "unitSize");
	glutils::Location s1Location = glutils::Location(Location::Type::Attribute, "s1");
	glutils::Location s2Location = glutils::Location(Location::Type::Attribute, "s2");

	// Particle descriptor, contains information of how to generate particle.
	std::shared_ptr<ParticleGenerator> generator;
	resource::ResourceId textureResource;

	float time = 0.0f;
	bool emitting = false;
	bool hasLiveParticles = false;
	float unitSize;
	std::vector<ParticleData> particles;

	virtual void renderImpl(graphics::Canvas &canvas);
};

}
using namespace game2d; // Collapse
}

#endif
