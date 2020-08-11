#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "AGameObject.h"

class GameObjectManager
{
public:
	typedef std::string String;
	typedef std::vector<AGameObject*> List;
	typedef std::unordered_map<String, AGameObject*> HashTable;

	enum PrimitiveType {
		CUBE,
		PLANE,
		SPHERE
	};

	static GameObjectManager* getInstance();
	static void initialize();
	static void destroy();

	AGameObject* findObjectByName(string name);
	List getAllObjects();
	int activeObjects();
	void updateAll();
	void renderAll(int viewportWidth, int viewportHeight, VertexShader* vertexShader, PixelShader* pixelShader);
	void addObject(AGameObject* gameObject);
	void createObject(PrimitiveType type, void* shaderByteCode, size_t sizeShader);
	void deleteObject(AGameObject* gameObject);
	void deleteObjectByName(string name);
	void setSelectedObject(string name);
	void setSelectedObject(AGameObject* gameObject);
	AGameObject* getSelectedObject();

private:
	GameObjectManager();
	~GameObjectManager();
	GameObjectManager(GameObjectManager const&) {};             // copy constructor is private
	GameObjectManager& operator=(GameObjectManager const&) {};  // assignment operator is private*/
	static GameObjectManager* sharedInstance;

	HashTable gameObjectMap;
	List gameObjectList;

	AGameObject* selectedObject = NULL;
};

