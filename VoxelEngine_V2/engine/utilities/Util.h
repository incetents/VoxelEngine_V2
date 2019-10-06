// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Macros.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>

namespace Vxl
{
	template<typename Type>
	class _Vector4;

	class Color4F;

	namespace Util
	{
		// Vector merging
		template<typename Type>
		void MergeVectors(std::vector<Type>& dest, const std::vector<Type>& src)
		{
			dest.insert(dest.end(), src.begin(), src.end());
		}

		// Vector find
		template<typename Type>
		bool IsInVector(const std::vector<Type>& vec, Type data)
		{
			return (std::find(vec.begin(), vec.end(), data) != vec.end());
		}
		// Vector count
		template<typename Type>
		int CountInVector(const std::vector<Type>& vec, Type data)
		{
			return std::count(vec.begin(), vec.end(), data);
		}
		// Vector removal
		template<typename Type>
		void RemoveFromVector(std::vector<Type>& vec, Type data)
		{
			if (data == nullptr || vec.size() == 0)
				return;

			vec.erase(std::remove(vec.begin(), vec.end(), data));
		}

		// Converting Raw Data
		namespace Conversion
		{
			// Breaks uint into 4 uchars
			void	 uint_to_uchars(uint32_t input, unsigned char& output1, unsigned char& output2, unsigned char& output3, unsigned char& output4);
			// Breaks uint into 4 uchars that are converted into [0,1] floats
			_Vector4<float>  uint_to_vec4(uint32_t input);
			// Breaks uint into 4 uchars that are converted into [0,1] floats
			Color4F  uint_to_color4(uint32_t input);
			// Combines 4 uchars into 1 unsigned int
			uint32_t uchars_to_uint(unsigned char* inputs);
		}



		// Aspect Ratio //
		static const uint32_t m_commonAspectRatiosCount = 4;
		static float m_commonAspectRatios[m_commonAspectRatiosCount] =
		{
			16.f / 9.f,
			16.f / 10.f,
			4.f  / 3.f,
			1.f  / 1.f
		};
		static const char* m_commonAspectRatiosStr[m_commonAspectRatiosCount] =
		{
			"16/9",
			"16/10",
			"4/3",
			"1/1"
		};

		// Resolution //
		static const uint32_t m_commonResolutionsCount = 25;
		static std::pair<uint32_t, uint32_t> m_commonResolutions[m_commonResolutionsCount] =
		{
			// 16:9
			{ 1024, 576 },
			{ 1152, 648 },
			{ 1280, 720 },
			{ 1366, 768 },
			{ 1600, 900 },
			{ 1920, 1080 },
			{ 2560, 1440 },
			{ 3840, 2160 },
			{ 7680, 4320 },
			// 16:10
			{ 1280, 800	 },
			{ 1440, 900	 },
			{ 1680, 1050 },
			{ 1920, 1200 },
			{ 2560, 1600 },
			// 4:3
			{ 640, 480	 },
			{ 800, 600	 },
			{ 960, 720	 },
			{ 1024, 768	 },
			{ 1280, 960	 },
			{ 1400, 1050 },
			{ 1440, 1080 },
			{ 1600, 1200 },
			{ 1856, 1392 },
			{ 1920, 1440 },
			{ 2048, 1536 }
		};
		static const char* m_commonResolutionsStr[m_commonResolutionsCount] =
		{
			// 16:9
			"1024, 576",
			"1152, 648",
			"1280, 720",
			"1366, 768",
			"1600, 900",
			"1920, 1080",
			"2560, 1440",
			"3840, 2160",
			"7680, 4320",
			// 16:10
			"1280, 800",
			"1440, 900",
			"1680, 1050",
			"1920, 1200",
			"2560, 1600",
			// 4:3
			"640, 480",
			"800, 600",
			"960, 720",
			"1024, 768",
			"1280, 960",
			"1400, 1050",
			"1440, 1080",
			"1600, 1200",
			"1856, 1392",
			"1920, 1440",
			"2048, 1536"
		};

	}
}