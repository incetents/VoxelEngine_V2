// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class RenderTexture : public BaseTexture
	{
	public:
		RenderTexture(const RenderTexture&) = delete;
		RenderTexture(
			int Width, int Height,
			Format_Type FormatType = Format_Type::RGBA8
		);
	};
}