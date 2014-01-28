#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "GLUtils.h"

#include <functional>
#include <string>
#include <vector>
#include <GLES2/gl2.h>

namespace rocket { namespace glutils {

class Program {
public:
	Program(std::string const& vertexShaderSrc, std::string const& fragmentShaderSrc);
	~Program();

	void use();
	GLint getAttribLocation(std::string const& name);
	GLint getUniformLocation(std::string const& name);

	std::string const& getVertexShaderSrc() const;
	std::string const& getFragmentShaderSrc() const;

//	std::string getInfoLog();

private:
	GLuint handle;

	std::string const vertexShaderSrc;
	std::string const fragmentShaderSrc;

	static GLuint loadShader(GLenum shaderType, std::string const& source);
	static GLuint createProgram(std::string const& vertexSource, std::string const& fragmentSource);
};

inline 
GLint Program::getAttribLocation(std::string const& name) {
	GLint loc = glGetAttribLocation(handle, name.c_str());
    checkGlError("glGetAttribLocation");
	return loc;
}

inline 
GLint Program::getUniformLocation(std::string const& name) {
	GLint loc = glGetUniformLocation(handle, name.c_str());
    checkGlError("glGetUniformLocation");
	return loc;
}

inline
std::string const& Program::getVertexShaderSrc() const {
	return vertexShaderSrc;
}

inline
std::string const& Program::getFragmentShaderSrc() const {
	return fragmentShaderSrc;
}
}}

namespace std {
	template<>
	struct hash<rocket::glutils::Program> {
		size_t operator()(rocket::glutils::Program const& program) const {
			size_t result = hash<string>()(program.getVertexShaderSrc());
			result += hash<string>()(program.getFragmentShaderSrc());
			return result;
		}
	};
}

#endif
