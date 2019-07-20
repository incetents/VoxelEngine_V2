// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

namespace Vxl
{
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

		// Data Conversion //
		class DataConversion
		{
		public:
			static void uint_to_uchars(unsigned int input, unsigned char& output1, unsigned char& output2, unsigned char& output3, unsigned char& output4)
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
			static void uchars_to_uint(unsigned char* inputs, unsigned int& output)
			{
				union Data
				{
					unsigned int _output;
					unsigned char _inputs[4];
				} data;
				// Input
				for(int i = 0; i < 4; i++)
					data._inputs[i] = inputs[i];
				// Output
				output = data._output;
			}

			// Breaks uint into 4 uchars that are converted into [0,1] floats
			static Vector4 uint_to_vec4(unsigned int input);
			static Color4F uint_to_color4(unsigned int input);
		};

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