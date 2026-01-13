#pragma once

#include "MiniWorld/Window.h"

namespace MiniWorld {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const override { return m_Handle; }

		// DirectX11 accessors
		virtual ID3D11Device* GetDevice() const override { return m_Device; }
		virtual ID3D11DeviceContext* GetDeviceContext() const override { return m_DeviceContext; }

		// Frame rendering
		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		bool CreateDeviceD3D();
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();
	private:
		HWND m_Handle = nullptr;
		WNDCLASSEX m_WindowClass = {};

		// DirectX 11 resources
		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;
		IDXGISwapChain* m_SwapChain = nullptr;
		ID3D11RenderTargetView* m_RenderTargetView = nullptr;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			unsigned int ResizeWidth = 0;
			unsigned int ResizeHeight = 0;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
