// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Macros.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

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
		class AspectRatioInfo
		{
		public:
			static const AspectRatioInfo Variations[];
			static const unsigned int VariationsCount;

			const float AspectRatio = 0;
			const std::string StrFormat = "";

			AspectRatioInfo() { }
			AspectRatioInfo(float Num, float Denom)
				: AspectRatio(Num / Denom),
				StrFormat(std::to_string(Num) + '|' + std::to_string(Denom))
			{}
		};

		// Resolution //
		class ResolutionInfo
		{
		public:
			static const ResolutionInfo Variations[];
			static const unsigned int VariationsCount;

			const unsigned int Width = 0;
			const unsigned int Height = 0;
			const std::string StrFormat = "";

			ResolutionInfo() {}
			ResolutionInfo(unsigned int W, unsigned int H)
				: Width(W), Height(H),
				StrFormat(std::to_string(W) + " x " + std::to_string(H))
			{}
		};
	}
}