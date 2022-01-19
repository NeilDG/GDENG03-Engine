#include "SceneWriter.h"
#include <iostream>
#include <fstream>
#include "GameObjectManager.h"

typedef std::fstream FileWriter;
SceneWriter::SceneWriter(String directory)
{
	this->directory = directory;
}

SceneWriter::~SceneWriter()
{
}

void SceneWriter::writeToFile()
{
	String fileDir = this->directory + ".iet";
	if (this->directory.find(".iet") != String::npos) {
		fileDir = this->directory;
	}
	
	FileWriter  sceneFile;
	sceneFile.open(fileDir, std::ios::out);

	std::cout << "Selected filename " << fileDir << "\n";

	GameObjectManager::List allObjects = GameObjectManager::getInstance()->getAllObjects();

	for (int i = 0; i < allObjects.size(); i++) {
		sceneFile << allObjects[i]->getName() << std::endl;
		Vector3D position = allObjects[i]->getLocalPosition();
		Vector3D rotation = allObjects[i]->getLocalRotationDegrees();
		Vector3D scale = allObjects[i]->getLocalScale();

		sceneFile << "Type: " << allObjects[i]->getObjectType() << std::endl;
		sceneFile << "Position: " << position.getX() << " "<< position.getY() << " " << position.getZ() << std::endl;
		sceneFile << "Rotation: " << rotation.getX() << " " << rotation.getY() << " " << rotation.getZ() << std::endl;
		sceneFile << "Scale: " << scale.getX() << " " << scale.getY() << " " << scale.getZ() << std::endl;
	}
	sceneFile.close();
}
