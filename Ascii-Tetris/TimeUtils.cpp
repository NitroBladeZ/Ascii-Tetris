#include "TimeUtils.h"
#include <thread>

using namespace std;


auto TimeUtils::getElapsedTime(steady_clock::time_point startTime, steady_clock::time_point endTime) {
	return duration_cast<milliseconds>(endTime - startTime).count();
}

void TimeUtils::enforceFrameRate(steady_clock::time_point startTime, steady_clock::time_point endTime) {
	const auto elapsedMilliseconds = getElapsedTime(startTime, endTime);
	this_thread::sleep_for(milliseconds(targetFrameTime - elapsedMilliseconds));
}

