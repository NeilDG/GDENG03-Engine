#pragma once
class MyManager
{
public:
	static MyManager* getInstance();
	static void initialize();
	static void destroy();

	int getX();
	
private:
	static MyManager* sharedInstance;
	MyManager();
	~MyManager();
	MyManager(MyManager const&) {};             // copy constructor is private
	MyManager& operator=(MyManager const&) {};  // assignment operator is private*/

	int x = 100;
};

