#pragma once
//Class for storing of frame ticks
#include <iostream>
#include <chrono>
#include <ctime>

class Window;

class EngineTime
{
public:
	static void initialize();
	static double getDeltaTime();

private:
	EngineTime();
	~EngineTime();
	EngineTime(EngineTime const&) {};             // copy constructor is private
	EngineTime& operator=(EngineTime const&) {};  // assignment operator is private*/

	static EngineTime* sharedInstance;

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;

	double deltaTime = 0.0;

	static void LogFrameStart();
	static void LogFrameEnd();

	friend class Window;
};

