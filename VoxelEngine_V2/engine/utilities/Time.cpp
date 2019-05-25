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

	// Static Variables
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
#ifdef GLOBAL_GPU_TIMERS
		if (m_queryingState == QueryState::READY)
		{
			m_queryingState = QueryState::START;

			glBeginQuery(GL_TIME_ELAPSED, m_ID);
			m_TimerBeingUsed = true;
		}
#endif
	}

	void GPUTimer::End()
	{
#ifdef GLOBAL_GPU_TIMERS
		if (m_queryingState == QueryState::START)
		{
			m_queryingState = QueryState::END;

			glEndQuery(GL_TIME_ELAPSED);
			m_TimerBeingUsed = false;
		}
#endif
	}

	void GPUTimer::Update()
	{
#ifdef GLOBAL_GPU_TIMERS
		if (m_queryingState != QueryState::END)
			return;

		glGetQueryObjectui64v(m_ID, GL_QUERY_RESULT, &m_elapsedTime);
		m_elapsedTime_MS[m_elapsedTime_MS_index] = (double)m_elapsedTime / 1000000.0;

		m_elapsedTime_MS_index++;
		m_elapsedTime_MS_index %= GPUTIMER_CAPTURES;

		m_elapsedTime_MS_average = 0;
		for (auto ms : m_elapsedTime_MS)
			m_elapsedTime_MS_average += ms;
		m_elapsedTime_MS_average /= (double)GPUTIMER_CAPTURES;

		m_queryingState = QueryState::READY;
#endif
	}

	void GPUTimer::StartTimer(const std::string& name)
	{
#ifdef GLOBAL_GPU_TIMERS
		// Create timer if doesn't exist
		if (m_timers.find(name) == m_timers.end())
			m_timers[name] = new GPUTimer();

		if (m_TimerBeingUsed)
			EndTimer(name);

		m_timers[name]->Begin();
#endif
	}

	void GPUTimer::EndTimer(const std::string& name)
	{
#ifdef GLOBAL_GPU_TIMERS
		// Create timer if doesn't exist
		if (m_timers.find(name) == m_timers.end())
			m_timers[name] = new GPUTimer();

		m_timers[name]->End();
#endif
	}

	double GPUTimer::GetElapsedTime_MS(const std::string& name)
	{
#ifdef GLOBAL_GPU_TIMERS
		if (m_timers.find(name) != m_timers.end())
			return m_timers[name]->m_elapsedTime_MS_average;
		else
#endif
			return 0;
	}

	
}