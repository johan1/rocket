# Required
NAME := rocket
TYPE := static

SOURCE_DIR := $(PROJECT_ROOT)/src

SOURCES := $(shell find $(PROJECT_ROOT)/src/common -name *.cpp | sed 's/.*src\///g' | tr '\n' ' ')

# Add platform sources for host
ifeq ($(TARGET),host)
SOURCES += $(shell find $(PROJECT_ROOT)/src/platform/host -name *.cpp | sed 's/.*src\///g' | tr '\n' ' ')
endif

# Add platform sources for android
ifneq ($(findstring android,$(TARGET)),)
SOURCES += $(shell find $(PROJECT_ROOT)/src/platform/android -name *.cpp | sed 's/.*src\///g' | tr '\n' ' ')
endif

# Deploy
EXPORT_AUTO_INCLUDES := 1
EXPORT_INCLUDE_SOURCE_DIR := $(SOURCE_DIR)/common
EXPORT_INCLUDE_FOLDER_NAME := rocket
EXPORT_LIBNAME := rocket
EXPORT_LIBVERSION := 0.0.1
