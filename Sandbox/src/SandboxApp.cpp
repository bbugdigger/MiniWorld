#include <MiniWorld.h>

#include "imgui/imgui.h"

class ExampleLayer : public MiniWorld::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (MiniWorld::Input::IsKeyPressed(MW_KEY_TAB))
			MW_TRACE("Tab key is pressed (poll)!");
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(MiniWorld::Event& event) override
	{
		if (event.GetEventType() == MiniWorld::EventType::KeyPressed)
		{
			MiniWorld::KeyPressedEvent& e = (MiniWorld::KeyPressedEvent&)event;
			if (e.GetKeyCode() == MW_KEY_TAB)
				MW_TRACE("Tab key is pressed (event)!");
			MW_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox : public MiniWorld::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

MiniWorld::Application* MiniWorld::CreateApplication()
{
	return new Sandbox();
}
