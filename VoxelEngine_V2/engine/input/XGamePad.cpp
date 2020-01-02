// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "XGamePad.h"

#include "../utilities/Util.h"

#pragma comment(lib, "Xinput9_1_0.lib")

#define MAX_VAL 32767

namespace Vxl
{
	XGamePad::XGamePad(UINT ID)
		: m_id(ID)
	{
		VXL_ASSERT(ID < 4, "GamePad ID is too large, only supports IDs 0 to 3");
		memset(&m_state, 0, sizeof(XINPUT_STATE));
	}

	bool XGamePad::IsConnected()
	{
		memset(&m_state, 0, sizeof(XINPUT_STATE));
		return XInputGetState(m_id, &m_state) == ERROR_SUCCESS;
	}
	void XGamePad::SetVibration(float left, float right)
	{
		XINPUT_VIBRATION vibration = {};
		vibration.wLeftMotorSpeed = static_cast<WORD>(left * MAX_VAL);
		vibration.wRightMotorSpeed = static_cast<WORD>(right * MAX_VAL);

		XInputSetState(m_id, &vibration);
	}

	bool XGamePad::GetButton(Buttons button) const
	{
		return (ButtonMemory_Now & static_cast<WORD>(button)) != 0;
	}
	bool XGamePad::GetButtonDown(Buttons button) const
	{
		return (ButtonMemory_Prev & static_cast<WORD>(button)) == 0 && (ButtonMemory_Now & static_cast<WORD>(button)) != 0;
	}
	bool XGamePad::GetButtonUp(Buttons button) const
	{
		return (ButtonMemory_Prev & static_cast<WORD>(button)) != 0 && (ButtonMemory_Now & static_cast<WORD>(button)) == 0;
	}

	float XGamePad::GetLeftShoulder() const // [0, 1]
	{
		return m_leftShoulder;
	}
	float XGamePad::GetRightShoulder() const // [0, 1]
	{
		return m_rightShoulder;
	}

	Vector2 XGamePad::GetLeftAnalog() const
	{
		return m_leftAnalog;
	}
	Vector2 XGamePad::GetRightAnalog() const
	{
		return m_rightAnalog;
	}
	Vector2 XGamePad::GetLeftAnalogNormalized() const
	{
		return m_leftAnalogNormalized;
	}
	Vector2 XGamePad::GetRightAnalogNormalized() const
	{
		return m_rightAnalogNormalized;
	}

	float XGamePad::GetLeftAnalogMagnitude() const
	{
		return m_leftAnalogMagnitude;
	}
	float XGamePad::GetRightAnalogMagnitude() const
	{
		return m_rightAnalogMagnitude;
	}

	void XGamePad::Update()
	{
		memset(&m_state, 0, sizeof(XINPUT_STATE));
		XInputGetState(m_id, &m_state);

		// Buttons
		ButtonMemory_Prev = ButtonMemory_Now;
		ButtonMemory_Now = m_state.Gamepad.wButtons;

		// Controls
		if (m_state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			m_leftShoulder = static_cast<float>(m_state.Gamepad.bLeftTrigger) / 255.0f;
		else
			m_leftShoulder = 0.0f;

		if (m_state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			m_rightShoulder = static_cast<float>(m_state.Gamepad.bRightTrigger) / 255.0f;
		else
			m_rightShoulder = 0.0f;

		m_leftAnalog.x = static_cast<float>(m_state.Gamepad.sThumbLX);
		m_leftAnalog.y = static_cast<float>(m_state.Gamepad.sThumbLY);
		m_leftAnalogMagnitude = m_leftAnalog.Length();

		m_rightAnalog.x = static_cast<float>(m_state.Gamepad.sThumbRX);
		m_rightAnalog.y = static_cast<float>(m_state.Gamepad.sThumbRY);
		m_rightAnalogMagnitude = m_rightAnalog.Length();

		if (m_leftAnalogMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_leftAnalog /= MAX_VAL;
			m_leftAnalogMagnitude /= MAX_VAL;
			m_leftAnalogNormalized = m_leftAnalog.Normalize();
		}
		else
		{
			m_leftAnalog = Vector2::ZERO;
			m_leftAnalogMagnitude = 0.0f;
			m_leftAnalogNormalized = Vector2::ZERO;
		}

		if (m_rightAnalogMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_rightAnalog /= MAX_VAL;
			m_rightAnalogMagnitude /= MAX_VAL;
			m_rightAnalogNormalized = m_leftAnalog.Normalize();
		}
		else
		{
			m_rightAnalog = Vector2::ZERO;
			m_rightAnalogMagnitude = 0.0f;
			m_rightAnalogNormalized = Vector2::ZERO;
		}


	}
}
