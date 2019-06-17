// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Macros.h"

void AssertFail(const std::string& message, const std::string& title)
{
	MessageBox(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
}