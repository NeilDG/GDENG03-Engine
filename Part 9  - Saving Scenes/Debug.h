#pragma once
#include <string>
#include "ConsoleScreen.h"
#include <vector>

class Debug
{
public:
	typedef std::string String;
	typedef std::vector<String> PendingList;

	static void initialize();
	static void destroy();
	static void Log(String msg);
	static void assignConsole(ConsoleScreen* console);

private:
	Debug();
	~Debug();
	Debug(Debug const&) {};             // copy constructor is private
	Debug& operator=(Debug const&) {};  // assignment operator is private*/

	static Debug* sharedInstance;

	ConsoleScreen* console;
	PendingList pendingMessages;



};

