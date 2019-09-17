// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Util.h"

#include "../math/Vector.h"
#include "../math/Color.h"

namespace Vxl
{
	namespace Util
	{
		const AspectRatioInfo AspectRatioInfo::Variations[] =
		{
			AspectRatioInfo(16.f, 9.f),
			AspectRatioInfo(16.f, 10.f),
			AspectRatioInfo(4.f, 3.f),
			AspectRatioInfo(1.f, 1.f),
		};
		const unsigned int AspectRatioInfo::VariationsCount = 4;

		const ResolutionInfo ResolutionInfo::Variations[] =
		{
			// 16:9
			ResolutionInfo(1024, 576),
			ResolutionInfo(1152, 648),
			ResolutionInfo(1280, 720), // HD
			ResolutionInfo(1366, 768),
			ResolutionInfo(1600, 900),
			ResolutionInfo(1920, 1080),// FHD
			ResolutionInfo(2560, 1440),
			ResolutionInfo(3840, 2160),// 4k
			ResolutionInfo(7680, 4320),// 8k
			// 16:10
			ResolutionInfo(1280, 800),
			ResolutionInfo(1440, 900),
			ResolutionInfo(1680, 1050),
			ResolutionInfo(1920, 1200),
			ResolutionInfo(2560, 1600),
			// 4:3
			ResolutionInfo(640, 480),
			ResolutionInfo(800, 600),
			ResolutionInfo(960, 720),
			ResolutionInfo(1024, 768),
			ResolutionInfo(1280, 960),
			ResolutionInfo(1400, 1050),
			ResolutionInfo(1440, 1080),
			ResolutionInfo(1600, 1200),
			ResolutionInfo(1856, 1392),
			ResolutionInfo(1920, 1440),
			ResolutionInfo(2048, 1536)
		};
		const unsigned int ResolutionInfo::VariationsCount = 25;

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