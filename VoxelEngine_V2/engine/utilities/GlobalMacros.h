// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

// Macros that are used to enable/disable portions of code before compilation


// [x] whether or not glUtil::setGLName should actually call glObjectLabel(...)
//#define GLOBAL_USE_GLNAMES // conflicts with GLOBAL_ERROR_CALLBACK

// [x] whether glUtil.cpp OpenglErrorCallback is used
#define GLOBAL_ERROR_CALLBACK // conflicts with GLOBAL_USE_GLNAMES

// [x] whether GPUTimer::StartTimer and GPUTimer::EndTimer actually do anything
#define GLOBAL_GPU_TIMERS

// [x] whether any of the ImGui libraries are used
#define GLOBAL_IMGUI
