#include "mwpch.h"
#include "WindowsWindow.h"

#include "MiniWorld/Events/ApplicationEvent.h"
#include "MiniWorld/Events/MouseEvent.h"
#include "MiniWorld/Events/KeyEvent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

namespace MiniWorld {
	
	static WindowsWindow* s_WindowInstance = nullptr;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		MW_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		s_WindowInstance = this;

		// Register window class
		m_WindowClass = {};
		m_WindowClass.cbSize = sizeof(WNDCLASSEX);
		m_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		m_WindowClass.lpfnWndProc = WindowsWindow::WndProc;
		m_WindowClass.hInstance = GetModuleHandle(nullptr);
		m_WindowClass.lpszClassName = L"MiniWorldWindowClass";
		m_WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		m_WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

		if (!RegisterClassEx(&m_WindowClass))
		{
			MW_CORE_ERROR("Failed to register window class!");
			MW_CORE_ASSERT(false, "Failed to register window class!");
		}

		// Convert title to wide string
		std::wstring wideTitle(props.Title.begin(), props.Title.end());

		// Calculate window size to get desired client area
		RECT windowRect = { 0, 0, (LONG)props.Width, (LONG)props.Height };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window
		m_Handle = CreateWindowExW(
			0,
			L"MiniWorldWindowClass",
			wideTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, nullptr,
			GetModuleHandle(nullptr),
			nullptr
		);

		if (!m_Handle)
		{
			MW_CORE_ERROR("Failed to create window!");
			MW_CORE_ASSERT(false, "Failed to create window!");
		}

		// Create D3D11 device and swap chain
		if (!CreateDeviceD3D())
		{
			CleanupDeviceD3D();
			UnregisterClassW(m_WindowClass.lpszClassName, m_WindowClass.hInstance);
			MW_CORE_ERROR("Failed to create D3D11 device!");
			MW_CORE_ASSERT(false, "Failed to create D3D11 device!");
		}

		ShowWindow(m_Handle, SW_SHOW);
		UpdateWindow(m_Handle);

		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		CleanupDeviceD3D();

		if (m_Handle)
		{
			DestroyWindow(m_Handle);
			m_Handle = nullptr;
		}

		if (m_WindowClass.lpszClassName)
		{
			UnregisterClassW(m_WindowClass.lpszClassName, m_WindowClass.hInstance);
			m_WindowClass = {};
		}

		s_WindowInstance = nullptr;
	}

	void WindowsWindow::OnUpdate()
	{
		// Process Windows messages
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Handle resize
		if (m_Data.ResizeWidth != 0 && m_Data.ResizeHeight != 0)
		{
			CleanupRenderTarget();
			m_SwapChain->ResizeBuffers(0, m_Data.ResizeWidth, m_Data.ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			m_Data.Width = m_Data.ResizeWidth;
			m_Data.Height = m_Data.ResizeHeight;
			m_Data.ResizeWidth = m_Data.ResizeHeight = 0;
			CreateRenderTarget();
		}
	}

	void WindowsWindow::BeginFrame()
	{
		// Clear with a color (magenta like original)
		const float clearColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
		m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, clearColor);
	}

	void WindowsWindow::EndFrame()
	{
		// Present with or without VSync
		m_SwapChain->Present(m_Data.VSync ? 1 : 0, 0);
	}

	bool WindowsWindow::CreateDeviceD3D()
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = m_Handle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

		UINT createDeviceFlags = 0;
#ifdef MW_DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevelArray,
			2,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&m_SwapChain,
			&m_Device,
			&featureLevel,
			&m_DeviceContext
		);

		if (FAILED(result))
		{
			MW_CORE_ERROR("D3D11CreateDeviceAndSwapChain failed with HRESULT: {0}", result);
			return false;
		}

		CreateRenderTarget();
		return true;
	}

	void WindowsWindow::CleanupDeviceD3D()
	{
		CleanupRenderTarget();

		if (m_SwapChain)
		{
			m_SwapChain->Release();
			m_SwapChain = nullptr;
		}
		if (m_DeviceContext)
		{
			m_DeviceContext->Release();
			m_DeviceContext = nullptr;
		}
		if (m_Device)
		{
			m_Device->Release();
			m_Device = nullptr;
		}
	}

	void WindowsWindow::CreateRenderTarget()
	{
		ID3D11Texture2D* backBuffer = nullptr;
		m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
		backBuffer->Release();
	}

	void WindowsWindow::CleanupRenderTarget()
	{
		if (m_RenderTargetView)
		{
			m_RenderTargetView->Release();
			m_RenderTargetView = nullptr;
		}
	}

	LRESULT WINAPI WindowsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (!s_WindowInstance || !s_WindowInstance->m_Data.EventCallback)
			return DefWindowProc(hWnd, msg, wParam, lParam);

		switch (msg)
		{
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
				return 0;
			s_WindowInstance->m_Data.ResizeWidth = static_cast<UINT>(LOWORD(lParam));
			s_WindowInstance->m_Data.ResizeHeight = static_cast<UINT>(HIWORD(lParam));
			{
				WindowResizeEvent event(s_WindowInstance->m_Data.ResizeWidth, s_WindowInstance->m_Data.ResizeHeight);
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
				return 0;
			break;

		case WM_CLOSE:
			{
				WindowCloseEvent event;
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			{
				int repeatCount = (lParam & 0x40000000) ? 1 : 0;
				KeyPressedEvent event(static_cast<int>(wParam), repeatCount);
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				KeyReleasedEvent event(static_cast<int>(wParam));
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_CHAR:
			{
				KeyTypedEvent event(static_cast<int>(wParam));
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			{
				int button = (msg == WM_LBUTTONDOWN) ? 0 : (msg == WM_RBUTTONDOWN) ? 1 : 2;
				MouseButtonPressedEvent event(button);
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			{
				int button = (msg == WM_LBUTTONUP) ? 0 : (msg == WM_RBUTTONUP) ? 1 : 2;
				MouseButtonReleasedEvent event(button);
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_MOUSEMOVE:
			{
				float x = static_cast<float>(LOWORD(lParam));
				float y = static_cast<float>(HIWORD(lParam));
				MouseMovedEvent event(x, y);
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;

		case WM_MOUSEWHEEL:
			{
				float yOffset = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
				MouseScrolledEvent event(0.0f, yOffset);
				s_WindowInstance->m_Data.EventCallback(event);
			}
			return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
