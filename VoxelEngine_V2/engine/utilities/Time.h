// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <vector>
#include <Windows.h>
#include "../utilities/singleton.h"
#include <GLFW/glfw3.h>

namespace Vxl
{
#define HISTOGRAM_SIZE 50
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
		inline double GetTimeLeft(void) const
		{
			return max(m_endTime - glfwGetTime(), 0);
		}
		inline bool HasFinished(void) const
		{
			return ((m_endTime - glfwGetTime()) <= 0);
		}
		inline void AddTime(double MoreTime)
		{
			m_endTime += MoreTime;
		}
		void Reset(void)
		{
			m_startTime = glfwGetTime();
			m_endTime = m_startTime + m_time;
		}
		void Reset(double NewTime)
		{
			m_time = NewTime;
			Reset();
		}
		
	};

	// ~~~ //
	static class TimeController : public Singleton<class TimeController>
	{
	public:
		void StartFrame()
		{
			Time.m_time = glfwGetTime();
			Time.m_deltaTime = (Time.m_time - Time.m_lastTime) / Time.m_limitFPS;
			Time.m_totalDeltaTime += Time.m_deltaTime;
			Time.m_lastTime = Time.m_time;
			Time.m_currentFPS = (Time.m_deltaTime * Time.m_targetFPS);

			Time.m_histogramFPS[HISTOGRAM_SIZE - 1] = (float)Time.m_currentFPS;
			memcpy(Time.m_histogramFPS, Time.m_histogramFPS + 1, (HISTOGRAM_SIZE - 1) * sizeof(float));

		}
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
}