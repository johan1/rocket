ifneq ($(findstring android,$(TARGET)),)
LIBRARIES += -L$(LOCAL_LIBPATH)/lib/$(TARGET) -Wl,--whole-archive -lrocket -Wl,--no-whole-archive
else
LIBRARIES += -L$(LOCAL_LIBPATH)/lib/$(TARGET) -lrocket
endif
LIBRARY_FILES += $(LOCAL_LIBPATH)/lib/$(TARGET)/librocket.a
INCLUDES += -I$(LOCAL_LIBPATH)/include
