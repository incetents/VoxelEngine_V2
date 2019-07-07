// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Macros that are used to enable/disable portions of code before compilation //


// [x] whether or not glUtil::setGLName should actually call glObjectLabel(...)
#ifdef _DEBUG
//#define GLOBAL_USE_GLNAMES // conflicts with GLOBAL_ERROR_CALLBACK
#endif

// [x] whether glUtil.cpp OpenglErrorCallback is used
#ifdef _DEBUG
#define GLOBAL_ERROR_CALLBACK // conflicts with GLOBAL_USE_GLNAMES
#endif

// [x] whether GPUTimer::StartTimer and GPUTimer::EndTimer actually do anything
#define GLOBAL_GPU_TIMERS

// [x] whether any of the ImGui libraries are used
#define GLOBAL_IMGUI

// ~~~~~~~~~~~~~~~~~~~~~~~~~//
// Macros that do Misc work //

#include "../utilities/logger.h"

void AssertFail(const std::string& message, const std::string& title);

// General Macros
#define __STRINGIFY(x) #x

// Assert Macros
#ifdef _DEBUG
// ~~~ //
#define VXL_ASSERT(expression, message) if(!(expression)) { Logger.error(message);\
AssertFail(#message ## "\n\nFunction: " + std::string(__FUNCSIG__) + "\nLine: " + std::to_string(__LINE__) + "\n\nFile:\n" + std::string(__FILE__), "Assert Failed"); }
#define VXL_RETURN_ON_FAIL(expression) if(!(expression)) { return; }
//
#else
// ~~~ //
#define VXL_ASSERT(expression, message) __noop;
#define VXL_RETURN_ON_FAIL(expression) __noop;
//
#endif