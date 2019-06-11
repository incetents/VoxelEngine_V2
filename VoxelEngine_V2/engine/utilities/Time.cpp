// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Time.h"

#include "../utilities/GlobalMacros.h"
#include "../opengl/glUtil.h"
#include <assert.h>
#include <GLFW/glfw3.h>

namespace Vxl
{
	void TimeController::StartFrame()
	{
		Time.m_time = glfwGetTime();
		Time.m_deltaTime = (Time.m_time - Time.m_lastTime) / Time.m_limitFPS;
		Time.m_totalDeltaTime += Time.m_deltaTime;
		Time.m_lastTime = Time.m_time;
		Time.m_currentFPS = (Time.m_deltaTime * Time.m_targetFPS);

		Time.m_histogramFPS[HISTOGRAM_SIZE - 1] = (float)Time.m_currentFPS;
		memcpy(Time.m_histogramFPS, Time.m_histogramFPS + 1, (HISTOGRAM_SIZE - 1) * sizeof(float));
	}

	double Clock::GetTimeLeft(void) const
	{
		return max(m_endTime - glfwGetTime(), 0);
	}
	bool Clock::HasFinished(void) const
	{
		return ((m_endTime - glfwGetTime()) <= 0);
	}
	void Clock::Reset(void)
	{
		m_startTime = glfwGetTime();
		m_endTime = m_startTime + m_time;
	}

	// CPU TIMERS //
	std::map<std::string, CPUTimer*> CPUTimer::m_timers;

	void CPUTimer::Begin()
	{
		assert(!m_inUse);

		m_start = std::chrono::steady_clock::now();
		m_inUse = true;
	}
	void CPUTimer::End()
	{
		assert(m_inUse);

		auto elapsedTime = std::chrono::steady_clock::now() - m_start;
		// Convert elapsed time into long long
		m_elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsedTime).count();
		m_inUse = false;
	}

	void CPUTimer::Update()
	{
		m_elapsedTime_MS[m_elapsedTime_MS_index] = (double)m_elapsedTime / 1000000.0;
		m_elapsedTime_MS_index++;
		m_elapsedTime_MS_index %= CPUTIMER_CAPTURES;

		m_elapsedTime_MS_average = 0;
		for (auto ms : m_elapsedTime_MS)
			m_elapsedTime_MS_average += ms;
		m_elapsedTime_MS_average /= (double)CPUTIMER_CAPTURES;
	}

	void CPUTimer::StartTimer(const std::string& name)
	{
		// Create timer if doesn't exist
		if (m_timers.find(name) == m_timers.end())
			m_timers[name] = new CPUTimer();

		m_timers[name]->Begin();
	}
	void CPUTimer::EndTimer(const std::string& name)
	{
		// Error if timer doesn't exist
		if (m_timers.find(name) == m_timers.end())
			assert(false);

		m_timers[name]->End();
	}

	double CPUTimer::GetElapsedTime_MS(const std::string& name)
	{
		// Error if timer doesn't exist
		if (m_timers.find(name) == m_timers.end())
			assert(false);

		// return time
		return m_timers[name]->m_elapsedTime_MS_average;
	}


	// GPU TIMERS //
	std::map<std::string, GPUTimer*> GPUTimer::m_timers;
	bool GPUTimer::m_TimerBeingUsed = false;

	void GPUTimer::DestroyTimers()
	{
#ifdef GLOBAL_GPU_TIMERS
		for (auto& timer : m_timers)
			delete timer.second;
		m_timers.clear();
#endif
	}

	GPUTimer::GPUTimer()
	{
#ifdef GLOBAL_GPU_TIMERS
		glGenQueries(1, &m_ID);
#endif
	}
	GPUTimer::~GPUTimer()
	{
#ifdef GLOBAL_GPU_TIMERS
		glDeleteQueries(1, &m_ID);
#endif
	}

	void GPUTimer::Begin()
	{
		glBeginQuery(GL_TIME_ELAPSED, m_ID);
	}
	void GPUTimer::End()
	{
		glEndQuery(GL_TIME_ELAPSED);
	}

	void GPUTimer::Update()
	{
#ifdef GLOBAL_GPU_TIMERS
		int ready = false;
		glGetQueryObjectiv(m_ID, GL_QUERY_RESULT_AVAILABLE, &ready);

		if (ready != 0)
		{
			glGetQueryObjectui64v(m_ID, GL_QUERY_RESULT, &m_elapsedTime);

			m_elapsedTime_MS[m_elapsedTime_MS_index] = (double)m_elapsedTime / 1000000.0;
			m_elapsedTime_MS_index++;
			m_elapsedTime_MS_index %= GPUTIMER_CAPTURES;

			m_elapsedTime_MS_average = 0;
			for (auto ms : m_elapsedTime_MS)
				m_elapsedTime_MS_average += ms;
			m_elapsedTime_MS_average /= (double)GPUTIMER_CAPTURES;
		}
#endif
	}

	void GPUTimer::StartTimer(const std::string& name)
	{
#ifdef GLOBAL_GPU_TIMERS
		// Must be open for usage
		if (m_TimerBeingUsed)
			assert(false);

		// Create timer if doesn't exist
		if (m_timers.find(name) == m_timers.end())
			m_timers[name] = new GPUTimer();

		m_timers[name]->Begin();
		m_TimerBeingUsed = true;
#endif
	}

	void GPUTimer::EndTimer()
	{
#ifdef GLOBAL_GPU_TIMERS
		// Must be open for usage
		if (!m_TimerBeingUsed)
			assert(false);

		GPUTimer::End();
		m_TimerBeingUsed = false;
#endif
	}

	double GPUTimer::GetElapsedTime_MS(const std::string& name)
	{
#ifdef GLOBAL_GPU_TIMERS
		if (m_timers.find(name) != m_timers.end())
			return m_timers[name]->m_elapsedTime_MS_average;
		else
#endif
			return 0.0;
	}

	
}