LOCAL_PATH := $(call my-dir)/../GRT

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    /** \
                    /**/** \
                    /**/**/**

LOCAL_MODULE    := GRT

LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/ClassificationModules/*/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/ClassificationModules/**/**/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/ClusteringModules/**/*.cpp) \
				   $(wildcard $(LOCAL_PATH)/ClusteringModules/**/**/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/ContextModules/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/CoreAlgorithms/**/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/CoreModules/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/DataStructures/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/FeatureExtractionModules/**/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/PostProcessingModules/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/PreProcessingModules/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/RegressionModules/**/**/*.cpp) \
                   $(wildcard $(LOCAL_PATH)/Util/*.cpp)

LOCAL_SRC_FILES += grt_wrap.cxx

LOCAL_CPPFLAGS += -std=c++11

LOCAL_LDLIBS += -latomic
LOCAL_LDLIBS += -llog  

include $(BUILD_SHARED_LIBRARY)