#include "mwpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "MiniWorld/Application.h"

// Forward declare ImGui's WndProc handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace MiniWorld {

	// Static pointer to access ImGuiLayer from WndProc
	static ImGuiLayer* s_ImGuiLayer = nullptr;
	static WNDPROC s_OriginalWndProc = nullptr;

	static LRESULT CALLBACK ImGuiWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		return CallWindowProc(s_OriginalWndProc, hWnd, msg, wParam, lParam);
	}

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		s_ImGuiLayer = this;
	}

	ImGuiLayer::~ImGuiLayer()
	{
		s_ImGuiLayer = nullptr;
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		HWND hwnd = static_cast<HWND>(app.GetWindow().GetNativeWindow());
		ID3D11Device* device = app.GetWindow().GetDevice();
		ID3D11DeviceContext* context = app.GetWindow().GetDeviceContext();

		// Hook into the window procedure for ImGui input
		s_OriginalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)ImGuiWndProc);

		// Initialize ImGui Win32 and DX11 implementations
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device, context);
	}

	void ImGuiLayer::OnDetach()
	{
		// Restore original WndProc
		if (s_OriginalWndProc)
		{
			Application& app = Application::Get();
			HWND hwnd = static_cast<HWND>(app.GetWindow().GetNativeWindow());
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)s_OriginalWndProc);
			s_OriginalWndProc = nullptr;
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	
	void ImGuiLayer::Begin()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		// Show the ImGui demo window to demonstrate docking
		static bool show_demo = true;
		ImGui::ShowDemoWindow(&show_demo);

		// Simple debug window
		ImGui::Begin("MiniWorld Debug");
		ImGui::Text("Welcome to MiniWorld Engine!");
		ImGui::Text("Using DirectX11 for rendering");
		ImGui::Text("Docking and Viewports ENABLED");
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

}
