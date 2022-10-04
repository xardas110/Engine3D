#pragma once
#include <unordered_map>
#include <iostream>
#include <string>
#include <intrin.h>
#include <chrono>

namespace Profiler
{
	struct Stats
	{
		struct Avg
		{	
			size_t counter{0};
			double totalMs{0.0};
			size_t totalCycles{0};

			size_t GetAvgCycles() const { return totalCycles / counter; }
			double GetAvgMs() const { return totalMs / (double)counter;}
						
		} avg;
		size_t timeNs {0};
		double timeMs {0.0};
		size_t cycles {0};
	};

	static std::unordered_map<std::string, Stats> functionProfiler;
}

#define STARTPROFILE(funcName) \
const auto start = std::chrono::high_resolution_clock::now(); \
const auto cycleStart = __rdtsc(); \

#define ENDPROFILE(funcName) \
const auto cycleEnd = __rdtsc(); \
const auto end = std::chrono::high_resolution_clock::now(); \
\
const auto cycles = cycleEnd - cycleStart; \
const auto duration = end - start; \
\
const auto timeNs = duration.count(); \
const auto timeMs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration).count(); \
\
\
auto it = Profiler::functionProfiler.find(#funcName);\
\
if (it != Profiler::functionProfiler.end())\
{\
	it->second.cycles = cycles;\
	it->second.timeMs = timeMs;\
	it->second.timeNs = timeNs;\
	\
	it->second.avg.counter++;\
	it->second.avg.totalMs += timeMs;\
	it->second.avg.totalCycles += cycles;\
}\
else\
{\
	Profiler::Stats stats;\
	stats.cycles = cycles;\
	stats.timeMs = timeMs;\
	stats.timeNs = timeNs;\
	\
	stats.avg.counter += 1;\
	stats.avg.totalMs += timeMs;\
	stats.avg.totalCycles += cycles;\
	\
	Profiler::functionProfiler[#funcName] = stats;\
}

#define PRINTPROFILE(funcName) \
	auto it2 = Profiler::functionProfiler.find(#funcName);\
	if (it2 != Profiler::functionProfiler.end())\
{\
	std::cout << "Profiling function(" << #funcName << ")" << std::endl; \
	std::cout << "Current timeMS: " << it2->second.timeMs << std::endl; \
	std::cout << "Current Cycles : " << it2->second.cycles << std::endl; \
	std::cout << "Avg timeMS: " << it2->second.avg.GetAvgMs() << std::endl; \
	std::cout << "Avg cycles: " << it2->second.avg.GetAvgCycles() << std::endl; \
	std::cout << "Total Ms: " << it2->second.avg.totalMs << std::endl << std::endl; \
}