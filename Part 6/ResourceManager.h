#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <stdlib.h>

class Resource;
class AResourceManager
{
public:
	typedef std::wstring String;
	typedef std::unordered_map<String, Resource*> HashTable;

	static AResourceManager* getInstance();
	static void initialize();
	static void destroy();

	Resource* createResourceFromFile(const wchar_t* filePath);

protected:
	virtual Resource* convertResourceFromFile(const wchar_t* filePath) = 0;

private:
	AResourceManager();
	~AResourceManager();
	AResourceManager(AResourceManager const&) {};             // copy constructor is private
	AResourceManager& operator=(AResourceManager const&) {};  // assignment operator is private*/
	static AResourceManager* sharedInstance;

	HashTable resourceMap;
};

