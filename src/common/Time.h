#pragma once

#include <chrono>

inline long long GetTimeInNanosecondsSinceEpoch()
{
	using namespace std::chrono;
	time_point<system_clock> currentTime = system_clock::now();
	duration<long long, std::nano> timeSinceEpoch = currentTime.time_since_epoch();
	return timeSinceEpoch.count();
}

inline double getElapsedTimeInSeconds(long long startTime)
{
	return (GetTimeInNanosecondsSinceEpoch() - startTime) / 1000000000.0;
}
