// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Util.h"

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
}