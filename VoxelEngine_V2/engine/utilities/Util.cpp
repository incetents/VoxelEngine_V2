// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Util.h"

#include "../math/Vector.h"
#include "../math/Color.h"

namespace Vxl
{
	namespace Util
	{
		namespace Conversion
		{
			void uint_to_uchars(uint32_t input, unsigned char& output1, unsigned char& output2, unsigned char& output3, unsigned char& output4)
			{
				output1 = (input) & 0xFF;
				output2 = (input >> 8) & 0xFF;
				output3 = (input >> 16) & 0xFF;
				output4 = (input >> 24) & 0xFF;
			}
			uint32_t uchars_to_uint(unsigned char* inputs)
			{
				uint32_t output;

				output = output << 8 | static_cast<unsigned char>(inputs[3]);
				output = output << 8 | static_cast<unsigned char>(inputs[2]);
				output = output << 8 | static_cast<unsigned char>(inputs[1]);
				output = output << 8 | static_cast<unsigned char>(inputs[0]);

				return output;
			}
			Vector4 uint_to_vec4(uint32_t input)
			{
				unsigned char cx, cy, cz, cw;
				uint_to_uchars(input, cx, cy, cz, cw);
				return Vector4(
					(float)cx / 255.0f,
					(float)cy / 255.0f,
					(float)cz / 255.0f,
					(float)cw / 255.0f
				);
			}
			Color4F uint_to_color4(uint32_t input)
			{
				unsigned char cx, cy, cz, cw;
				uint_to_uchars(input, cx, cy, cz, cw);
				return Color4F(
					(float)cx / 255.0f,
					(float)cy / 255.0f,
					(float)cz / 255.0f,
					(float)cw / 255.0f
				);
			}
		}
	}
}