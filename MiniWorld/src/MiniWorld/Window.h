#pragma once

#include "mwpch.h"

#include "MiniWorld/Core.h"
#include "MiniWorld/Events/Event.h"

namespace MiniWorld {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "MiniWorld Engine",
			        unsigned int width = 1280,
			        unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class MINIWORLD_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		// DirectX11 accessors
		virtual ID3D11Device* GetDevice() const = 0;
		virtual ID3D11DeviceContext* GetDeviceContext() const = 0;

		// Frame rendering
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}
