#include <GLES2/gl2.h>

#include "Program.h"
#include "GLException.h"
#include "GLUtils.h"

#include <boost/format.hpp>
#include "../util/Log.h"

namespace rocket { namespace glutils {

using namespace boost;

Program::Program(std::string const& vertexShaderSrc, std::string const& fragmentShaderSrc) : 
		handle(0), 
		vertexShaderSrc(vertexShaderSrc), 
		fragmentShaderSrc(fragmentShaderSrc) {}

Program::~Program() {
	if (handle != 0) {
		LOGD("Deleting program " << handle);
		glDeleteProgram(handle);
	}
}

GLuint Program::loadShader(GLenum shaderType, std::string const& source) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
		const char* source_c_str = source.c_str();
        glShaderSource(shader, 1, &source_c_str, 0);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = new char[infoLen]; // (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, 0, buf);
                    LOGE("Could not compile shader " << shaderType << ":\n" << buf << "%s");
					delete[] buf;
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }

	checkGlError("Program::loadShader");
    return shader;
}

GLuint Program::createProgram(std::string const& vertexSource, std::string const& fragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = new char[bufLength];
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, 0, buf);
                    LOGE("Could not link program:\n" << buf << "\n");
					delete[] buf;
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }

	checkGlError("Program::createProgram");
    return program;
}

void Program::use() {
	if (handle == 0) {
		handle = createProgram(vertexShaderSrc, fragmentShaderSrc);
		LOGD("Created program " << handle);
	}

	glUseProgram(handle);
    checkGlError("glUseProgram");
}

}}
