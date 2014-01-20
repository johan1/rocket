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

/*
struct ParticleDescriptor {
	ParticleDescriptor(resource::image::ImageId imageId) : imageId(imageId) {}
	resource::image::ImageId imageId;
//	glutils::RGBAColor color;

	std::function<glutils::RGBAColor(std::default_random_engine&)> colorFunction;
	std::function<glm::vec3(std::default_random_engine&)> directionFunction;
	std::pair<float, float> start_distance;     // distance between origin and start position.
	std::pair<float, float> end_distance; 		// distance between origin and end position.
	std::pair<float, float> lifetime;   // life time of the particle
};
*/
class ParticleEmitter : public Renderable {
public:
	// ParticleEmitter(ParticleDescriptor const& descriptor, uint32_t numberOfParticles);
	ParticleEmitter(std::shared_ptr<ParticleGenerator> const& generator, resource::ResourceId const& particleTexture, uint32_t numberOfParticles);

	void start();
	bool isStarted() { return emitting; }
	void stop();

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
		float s1;
		float s2;
		float timestamp;
		float duration;
	};

	/*
	class ParticleGenerator {
	public:
		ParticleGenerator(ParticleDescriptor const& descriptor);
		ParticleData generateParticleData();

	private:
		std::default_random_engine random_engine;
		ParticleDescriptor descriptor;

		std::uniform_real_distribution<float> rStart; 		// random distance start
		std::uniform_real_distribution<float> rEnd; 		// random distance end
		std::uniform_real_distribution<float> rLifetime;	// random lifetime
	};
	*/

	// Render members
	std::shared_ptr<glutils::Program> program;
	glutils::Location vpMatrixLocation;
	glutils::Location timeLocation;
	glutils::Location samplerLocation;
	glutils::Location c1Location;
	glutils::Location c2Location;
	glutils::Location timestampLocation;
	glutils::Location durationLocation;
	glutils::Location p1Location;
	glutils::Location p2Location;
	glutils::Location s1Location;
	glutils::Location s2Location;

	// Particle descriptor, contains information of how to generate particle.
//	ParticleDescriptor descriptor;
	std::shared_ptr<ParticleGenerator> generator;
	resource::ResourceId textureResource;

	float time;
	bool emitting;
	bool hasLiveParticles;
	std::vector<ParticleData> particles;

	virtual void renderImpl(graphics::Canvas &canvas);
};

}}

#endif
