# Text rendering library
LOCAL_LIBNAME 		:= freetype
LOCAL_LIBVERSION	:= 2.5.2
include $(BUILD_ROOT)/common/repo_dependency.mk

# GLM Matrix gl library
LOCAL_LIBNAME 		:= glm
LOCAL_LIBVERSION	:= 0.9.4.3
include $(BUILD_ROOT)/common/repo_dependency.mk

# Boost dependencies
LOCAL_LIBNAME 		:= BoostThread
LOCAL_LIBVERSION	:= 1.55
include $(BUILD_ROOT)/common/repo_dependency.mk

LOCAL_LIBNAME 		:= BoostFileSystem
LOCAL_LIBVERSION	:= 1.55
include $(BUILD_ROOT)/common/repo_dependency.mk

LOCAL_LIBNAME 		:= BoostChrono
LOCAL_LIBVERSION	:= 1.55
include $(BUILD_ROOT)/common/repo_dependency.mk

# Ogg/Vorbis
LOCAL_LIBNAME 		:= vorbis
LOCAL_LIBVERSION	:= 1.3.3
include $(BUILD_ROOT)/common/repo_dependency.mk

# PNG
LOCAL_LIBNAME 		:= png15
LOCAL_LIBVERSION	:= 1.15.3
include $(BUILD_ROOT)/common/repo_dependency.mk

# JsonCpp
LOCAL_LIBNAME 		:= jsoncpp
LOCAL_LIBVERSION	:= 0.5.0
include $(BUILD_ROOT)/common/repo_dependency.mk

# Rocket core
LOCAL_LIBNAME 		:= rocketcore
LOCAL_LIBVERSION	:= 0.0.1
include $(BUILD_ROOT)/common/repo_dependency.mk

# OpenAL for targets other than android.
ifeq ($(findstring android,$(TARGET)),)
LOCAL_LIBNAME 		:= openal-soft
LOCAL_LIBVERSION	:= 1.17.2
include $(BUILD_ROOT)/common/repo_dependency.mk
endif

# Host target specifics
ifeq ($(TARGET),host)
DEFINES += -DUSE_GLEW
#DEFINES += -DUSE_GLES2
SYSTEM_LIBRARIES += $(shell pkg-config x11 --libs)
SYSTEM_LIBRARIES += $(shell pkg-config zlib --libs)
ifneq ($(findstring -DUSE_GLES2,$(DEFINES)),)
SYSTEM_LIBRARIES += $(shell pkg-config glesv2 --libs)
SYSTEM_LIBRARIES += $(shell pkg-config egl --libs)
endif
#SYSTEM_LIBRARIES += $(shell pkg-config openal --libs)
endif

# Windows target specifics
ifeq ($(TARGET),i686-w64-mingw32)
DEFINES += -DUSE_GLEW
SYSTEM_LIBRARIES += -lz 
endif

# Android target specifics
ifneq ($(findstring android,$(TARGET)),)
SYSTEM_LIBRARIES += -llog -landroid -lGLESv2 -lEGL -lz -lOpenSLES
DEFINES += -DUSE_GLES2
endif

# If using glew include library.
ifneq ($(findstring -DUSE_GLEW,$(DEFINES)),)
LOCAL_LIBNAME 		:= glew
LOCAL_LIBVERSION	:= 2.0.0
include $(BUILD_ROOT)/common/repo_dependency.mk
endif
