// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <Xinput.h>

#include "../utilities/singleton.h"
#include "../math/Vector2.h"

namespace Vxl
{
	enum ControllerIndex
	{
		Joy1 = 0,
		Joy2,
		Joy3,
		Joy4,
		TOTAL
	};

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
		XGamePad** m_GamePads;
	public:
		XGamePadManager()
		{
			m_GamePads = new XGamePad*[(int)ControllerIndex::TOTAL];

			for (int i = 0; i < 4; i++)
				m_GamePads[i] = new XGamePad(i);
		}
		XGamePad& GetGamePad(ControllerIndex index)
		{
			return *m_GamePads[(int)index];
		}
		void Update()
		{
			for (int i = 0; i < 4; i++)
				m_GamePads[i]->Update();
		}

	}SingletonInstance(XGamePadManager);

}

#define GamePad1 XGamePadManager.GetGamePad(ControllerIndex::Joy1)
#define GamePad2 XGamePadManager.GetGamePad(ControllerIndex::Joy2)
#define GamePad3 XGamePadManager.GetGamePad(ControllerIndex::Joy3)
#define GamePad4 XGamePadManager.GetGamePad(ControllerIndex::Joy4)
