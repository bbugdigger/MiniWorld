#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace MiniWorld {

	class MINIWORLD_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define MW_CORE_TRACE(...)    ::MiniWorld::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MW_CORE_INFO(...)     ::MiniWorld::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MW_CORE_WARN(...)     ::MiniWorld::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MW_CORE_ERROR(...)    ::MiniWorld::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MW_CORE_FATAL(...)    ::MiniWorld::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define MW_TRACE(...)	      ::MiniWorld::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MW_INFO(...)	      ::MiniWorld::Log::GetClientLogger()->info(__VA_ARGS__)
#define MW_WARN(...)	      ::MiniWorld::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MW_ERROR(...)	      ::MiniWorld::Log::GetClientLogger()->error(__VA_ARGS__)
#define MW_FATAL(...)	      ::MiniWorld::Log::GetClientLogger()->fatal(__VA_ARGS__)
