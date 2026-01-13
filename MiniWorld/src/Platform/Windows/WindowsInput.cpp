#include "mwpch.h"
#include "WindowsInput.h"

#include "MiniWorld/Application.h"

namespace MiniWorld {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		// Use GetAsyncKeyState for Windows
		return (GetAsyncKeyState(keycode) & 0x8000) != 0;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		int vkButton;
		switch (button)
		{
		case 0: vkButton = VK_LBUTTON; break;
		case 1: vkButton = VK_RBUTTON; break;
		case 2: vkButton = VK_MBUTTON; break;
		default: return false;
		}
		return (GetAsyncKeyState(vkButton) & 0x8000) != 0;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		POINT point;
		GetCursorPos(&point);
		
		HWND hwnd = static_cast<HWND>(Application::Get().GetWindow().GetNativeWindow());
		ScreenToClient(hwnd, &point);

		return { static_cast<float>(point.x), static_cast<float>(point.y) };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return y;
	}

}
