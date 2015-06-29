#include "ParticleEmitter.h"
#include "../Director.h"
#include "../../glutils/ProgramPool.h"

#include <glm/gtc/type_ptr.hpp>

using namespace rocket::game2d;
using namespace rocket::glutils;
using namespace rocket::resource;

namespace rocket { namespace game2d {

// The intention is to create som vertex buffer later. 
// With this design we shouldn't need to update the attributes unless the particle data is changed. == WIN
static const std::string vertexShader =
	"precision highp float;\n"
	"uniform mat4 vpMatrix;\n" // View projection matrix. I.e. P*V
	"uniform float time;\n"
	"uniform float unitSize;\n"
	"attribute float timestamp;\n"
	"attribute float duration;\n"
	"attribute vec3 p1;\n" // Start point
	"attribute vec3 p2;\n" // End point
	"attribute float s1;\n" // Start size (in units)
	"attribute float s2;\n" // End size (in units)
	"attribute vec4 c1;\n" // Start color
	"attribute vec4 c2;\n" // End color
	"varying vec4 fragment_color;\n"
	"\n"
	"void main() {\n"
	"  float nlTime = (time-timestamp)/duration;\n" // normalized localtime
	"  if (nlTime <= 1.0) {\n"
	"    vec4 localPosition;\n"
	"    localPosition.xyz = p1 + (p2 - p1) * nlTime;\n"
	"    localPosition.w = 1.0;\n"
	"    gl_Position = vpMatrix * localPosition;\n"
	"    gl_PointSize = (s1 + (s2 - s1) * nlTime)*unitSize;\n"
	"    fragment_color = c1 + (c2 - c1) * nlTime;\n"
	"  } else {\n"
    "    gl_Position = vec4(1000, 1000, 1000, 0);\n"
	"  }"
	"}\n";

static const std::string fragmentShader =
	"precision highp float;\n"
	"uniform sampler2D sampler; \n"
	"varying vec4 fragment_color;\n"
	"void main() {\n"
	"  vec4 texColor;\n"
	"  texColor = texture2D(sampler, gl_PointCoord);\n"
	"  gl_FragColor = fragment_color * texColor;\n"
	"}\n";

ParticleEmitter::ParticleEmitter(std::shared_ptr<ParticleGenerator> const& generator, ResourceId const& textureResource, float unitSize, uint32_t numberOfParticles) :
	generator(generator),
	textureResource(textureResource),
	unitSize(unitSize),
	particles(numberOfParticles) {

	auto& programPool = Director::getDirector().getProgramPool();
	auto programKey = programPool.getProgramKey(vertexShader, fragmentShader);
	program = programPool.lookupProgram(programKey);
}

void ParticleEmitter::renderImpl(graphics::Canvas &canvas) {
	if (!hasLiveParticles) {
		return;
	}

	program->use();

	auto& vpMatrix = canvas.getViewProjectionMatrix();
	auto& modelMatrix = canvas.getModelMatrix();
	auto& texture = canvas.getTextureManager().getTexture(textureResource);

	if (time == 0.0f) {
		for (auto& particleData : particles) {
			particleData = ParticleData(generator->generate(), modelMatrix, time);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Pushing uniforms...
	glUniformMatrix4fv(vpMatrixLocation.get(program), 1, GL_FALSE, glm::value_ptr(vpMatrix) );
	checkGlError("glUniformMatrix4fv");

	glUniform1f(timeLocation.get(program), time);
	checkGlError("glUniform1f");

	glUniform1f(unitSizeLocation.get(program), unitSize);
	checkGlError("glUniform1f");

	texture.setActive(samplerLocation.get(program), GL_TEXTURE0);

	for (auto &particleData : particles) {
		auto liveParticles = particles.size();
		if (time > particleData.timestamp + particleData.duration) {
			if (emitting) {
				particleData = ParticleData(generator->generate(), modelMatrix, time);
			} else {
				--liveParticles;
			}
		}
	}

	// Pushing attributes.
	glVertexAttribPointer(timestampLocation.get(program), 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].timestamp));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(durationLocation.get(program), 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].duration));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(p1Location.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].p1));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(p2Location.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].p2));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(c1Location.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].c1));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(c2Location.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].c2));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(s1Location.get(program), 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].s1));
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(s2Location.get(program), 1, GL_FLOAT, GL_FALSE, sizeof(ParticleData),
			&(particles[0].s2));
	checkGlError("glVertexAttribPointer");

	glEnableVertexAttribArray(timestampLocation.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(durationLocation.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(p1Location.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(p2Location.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(c1Location.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(c2Location.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(s1Location.get(program));
	checkGlError("glEnableVertiexAttribArray");
	glEnableVertexAttribArray(s2Location.get(program));
	checkGlError("glEnableVertiexAttribArray");

	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.size()));

	// Draw particles...
	time += 1.0/static_cast<float>(TARGET_FPS);

	glDisable(GL_BLEND);
	canvas.clear();
}

void ParticleEmitter::start() {
	if (emitting) {
		return;
	}

	time = 0;
	emitting = true;
	hasLiveParticles = true;
}

void ParticleEmitter::stop() {
	emitting = false;
}

}}
