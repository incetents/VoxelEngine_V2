// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include <unordered_map>
#include <map>
#include <assert.h>
#include <chrono>

#include "../utilities/singleton.h"

#define HISTOGRAM_SIZE 50
#define GPUTIMER_CAPTURES 15
#define CPUTIMER_CAPTURES 15

namespace Vxl
{
	// ~~~ //
	static class Time : public Singleton<class Time>
	{
		friend class TimeController;
	private:
		UINT m_frames = 0;
		double m_lastTime = 0;
		double m_time = 0;
		double m_deltaTime = 0;
		double m_totalDeltaTime = 0;
		double m_limitFPS = 1.0 / 60.0;
		double m_targetFPS = 60.0;
		double m_currentFPS = 0.0;
		float m_histogramFPS[HISTOGRAM_SIZE];
	public:

		Time()
		{
			memset(m_histogramFPS, 0, HISTOGRAM_SIZE * sizeof(float));
		}

		inline void SetTargetFPS(double FPS)
		{
			assert(FPS > 0.0); // FPS cannot be zero or smaller

			m_targetFPS = FPS;
			m_limitFPS = 1.0 / FPS;
		}

		inline double GetDeltaTime(void) const
		{
			return m_deltaTime;
		}
		inline double GetFPS(void) const
		{
			return m_currentFPS;
		}
		inline float* GetFPSHistogram(void)
		{
			return m_histogramFPS;
		}
		inline UINT GetFPSHistogramSize(void) const
		{
			return HISTOGRAM_SIZE;
		}
		inline double GetTime()
		{
			return m_time;
		}
		inline UINT GetFrameCount(void) const
		{
			return m_frames;
		}

	} SingletonInstance(Time);

	// ~~~ //
	static class TimeController : public Singleton<class TimeController>
	{
	public:
		void StartFrame();
		void EndFrame()
		{
			Time.m_frames++;
		}
		inline double GetTotalDeltaTime() const
		{
			return Time.m_totalDeltaTime;
		}
		inline void DecreaseTotalDeltaTime()
		{
			Time.m_totalDeltaTime -= 1.0;
		}

	} SingletonInstance(TimeController);

	// ~~~ //
	class Clock
	{
	private:
		double m_startTime;
		double m_endTime;
		double m_time;
	public:
		Clock(double time)
			: m_time(time)
		{
			Reset();
		}
		double GetTimeLeft(void) const;
		bool HasFinished(void) const;
		inline void AddTime(double MoreTime)
		{
			m_endTime += MoreTime;
		}
		void Reset(void);
		void Reset(double NewTime)
		{
			m_time = NewTime;
			Reset();
		}

	};

	// ~~~ //
	class CPUTimer
	{
		friend class Performance;
		friend class RenderManager;
	private:

		static std::map<std::string, CPUTimer*> m_timers;

		bool m_inUse = false;
		long long	m_elapsedTime;
		double		m_elapsedTime_MS[GPUTIMER_CAPTURES] = { 0 };
		UINT		m_elapsedTime_MS_index = 0;
		double		m_elapsedTime_MS_average;
		std::chrono::steady_clock::time_point m_start;

		void Begin();
		void End();

		void Update();

	public:
		static void	StartTimer(const std::string& name);
		static void	EndTimer(const std::string& name);

		static double GetElapsedTime_MS(const std::string& name);
	};

	// ~~~ //
	class GPUTimer
	{
		friend class Performance;
		friend class RenderManager;
	private:

		static std::map<std::string, GPUTimer*> m_timers;
		static bool	m_TimerBeingUsed;

		UINT		m_ID;
		uint64_t	m_elapsedTime = 0;
		double		m_elapsedTime_MS[GPUTIMER_CAPTURES] = { 0 };
		UINT		m_elapsedTime_MS_index = 0;
		double		m_elapsedTime_MS_average;

		static void DestroyTimers();

		GPUTimer();
		~GPUTimer();

		void Begin();
		static void End();

		void Update();

	public:

		inline UINT		GetID(void) const
		{
			return m_ID;
		}

		static void		StartTimer(const std::string& name);
		static void		EndTimer();
		static double	GetElapsedTime_MS(const std::string& name);

	};
}