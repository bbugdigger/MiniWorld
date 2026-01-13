#pragma once

#ifdef MW_PLATFORM_WINDOWS

extern MiniWorld::Application* MiniWorld::CreateApplication();

int main(int argc, char** argv)
{
	MiniWorld::Log::Init();
	MW_CORE_WARN("Initialized Log!");
	int a = 5;
	MW_INFO("Hello! Var={0}", a);

	auto app = MiniWorld::CreateApplication();
	app->Run();
	delete app;
}

#endif
