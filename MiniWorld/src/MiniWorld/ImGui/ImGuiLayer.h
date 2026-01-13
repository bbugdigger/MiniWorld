#pragma once

#include "MiniWorld/Layer.h"

#include "MiniWorld/Events/ApplicationEvent.h"
#include "MiniWorld/Events/KeyEvent.h"
#include "MiniWorld/Events/MouseEvent.h"

namespace MiniWorld {

	class MINIWORLD_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}
