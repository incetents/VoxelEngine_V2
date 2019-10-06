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
				union Data
				{
					unsigned int _input;
					unsigned char _outputs[4];
				} data;
				// Input
				data._input = input;
				// Output
				output1 = data._outputs[0];
				output2 = data._outputs[1];
				output3 = data._outputs[2];
				output4 = data._outputs[3];
			}
			uint32_t uchars_to_uint(unsigned char* inputs)
			{
				union Data
				{
					uint32_t _output;
					unsigned char _inputs[4];
				} data;
				// Input
				for (int i = 0; i < 4; i++)
					data._inputs[i] = inputs[i];
				// Output
				return data._output;
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