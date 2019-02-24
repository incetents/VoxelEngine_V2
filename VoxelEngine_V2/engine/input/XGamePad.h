// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <Xinput.h>

#include "../utilities/singleton.h"
#include "../math/Vector2.h"

namespace Vxl
{
	class XGamePad
	{
	private:
		XINPUT_STATE m_state;
		UINT m_id;

		float m_leftShoulder;
		float m_rightShoulder;

		Vector2 m_leftAnalog;
		Vector2 m_rightAnalog;
		Vector2 m_leftAnalogNormalized;
		Vector2 m_rightAnalogNormalized;

		float m_leftAnalogMagnitude;
		float m_rightAnalogMagnitude;

		WORD ButtonMemory_Prev;
		WORD ButtonMemory_Now;

	public:
		enum class Buttons
		{
			DPadUp			= 0x0001,
			DPadDown		= 0x0002,
			DPadLeft		= 0x0004,
			DPadRight		= 0x0008,
			Start			= 0x0010,
			Back			= 0x0020,
			LeftThumb		= 0x0040,
			RightThumb		= 0x0080,
			LeftShoulder	= 0x0100,
			RightShoulder	= 0x0200,
			A				= 0x1000,
			B				= 0x2000,
			X				= 0x4000,
			Y				= 0x8000
		};


		XGamePad() = delete;
		XGamePad(UINT ID);

		bool IsConnected();
		void SetVibration(float left, float right);

		bool GetButton(Buttons button) const;
		bool GetButtonDown(Buttons button) const;
		bool GetButtonUp(Buttons button) const;

		float GetLeftShoulder() const; // [0, 1]
		float GetRightShoulder() const; // [0, 1]

		Vector2 GetLeftAnalog() const;
		Vector2 GetRightAnalog() const;
		Vector2 GetLeftAnalogNormalized() const;
		Vector2 GetRightAnalogNormalized() const;

		float GetLeftAnalogMagnitude() const;
		float GetRightAnalogMagnitude() const;

		void Update();
	};

	static class XGamePadManager : public Singleton<class XGamePadManager>
	{
	private:
		// All Gamepad
		XGamePad** m_GamePads;
	public:
		XGamePadManager()
		{
			m_GamePads = new XGamePad*[4];

			for (int i = 0; i < 4; i++)
				m_GamePads[i] = new XGamePad(i);
		}
		void Update()
		{
			for (int i = 0; i < 4; i++)
				m_GamePads[i]->Update();
		}
		XGamePad& GetGamePad1(void) const
		{
			return *m_GamePads[0];
		}
		XGamePad& GetGamePad2(void) const
		{
			return *m_GamePads[1];
		}
		XGamePad& GetGamePad3(void) const
		{
			return *m_GamePads[2];
		}
		XGamePad& GetGamePad4(void) const
		{
			return *m_GamePads[3];
		}

	}SingletonInstance(XGamePadManager);

}

#define GamePad1 XGamePadManager.GetGamePad1()
#define GamePad2 XGamePadManager.GetGamePad2()
#define GamePad3 XGamePadManager.GetGamePad3()
#define GamePad4 XGamePadManager.GetGamePad4()
