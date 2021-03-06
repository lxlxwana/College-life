#include <iostream>
#include <fstream>
#include <vector>

#include "objReader.hpp"



objReader::objReader() {
    numOfVertex = -1;
	numofObject = -1;
	numofMaterial = -1;
}

objReader::~objReader() {

}

bool objReader::readMtlFile(const char *filename) {
	std::ifstream mtlFile(filename);
	
	if (!mtlFile.good()) {
		printf("The mtl file %s doesn't exist.\n", filename);
		return false;
	}

	std::string tmpName;
	tripleArrayDouble tmpTA;
	material *tmpMaterial = new material();
	int lineNum = 0;

	while (!mtlFile.eof()) {
		lineNum++;
		mtlFile >> tmpName;
		if (!tmpName.compare("newmtl")) {
			numofMaterial++;
			mtlFile >> tmpName;
			tmpMaterial = new material();
			materialList[tmpName] = tmpMaterial;
			tmpMaterial->name = tmpName;
		}
		else if (!tmpName.compare("illum")) {
			mtlFile >> tmpMaterial->illum;
		}
		else if (!tmpName.compare("Ka")) {
			mtlFile >> tmpTA[0] >> tmpTA[1] >> tmpTA[2];
			tmpMaterial->Ka = vec3d(tmpTA[0], tmpTA[1], tmpTA[2]);
		}
		else if (!tmpName.compare("Kd")) {
			mtlFile >> tmpTA[0] >> tmpTA[1] >> tmpTA[2];
			tmpMaterial->Kd = vec3d(tmpTA[0], tmpTA[1], tmpTA[2]);
		}
		else if (!tmpName.compare("Ks")) {
			mtlFile >> tmpTA[0] >> tmpTA[1] >> tmpTA[2];
			tmpMaterial->Ks = vec3d(tmpTA[0], tmpTA[1], tmpTA[2]);
		}
		else if (!tmpName.compare("Ns")) {
			mtlFile >> tmpMaterial->Ns;
		}
		else if (!tmpName.compare("Tr")) {
			mtlFile >> tmpMaterial->Tr;
		}
		else if (!tmpName.compare("Ni")) {
			mtlFile >> tmpMaterial->Ni;
		}
		else {
			printf("Error format in mtl file at line %d\n", lineNum);
			return false;
		}
	}

	printf("Read from %s success!\n", filename);
	printf("Material Number = %d\n", numofMaterial);

	return true;
}

bool objReader::readObjFile(const char *filename) {
    int lineNum = 0;

    std::ifstream objFile(filename);
    
    if (!objFile.good()) {
        printf("The obj file %s doesn't exist.\n", filename);
        return false;
    }

	vertexList.clear();

	std::string tmpName;
	tripleArrayDouble tmpTA;
	rootObject *tmpObject = NULL;



    while (!objFile.eof()) {
		lineNum++;
		objFile >> tmpName;
		// 材质文件名
		if (!tmpName.compare("mtllib")) {
			objFile >> tmpName;
			printf("mtl file name is %s\n", tmpName.c_str());
			if (!readMtlFile(tmpName.c_str()))
				return false;
		}
		// 可以认为usemtl以下直至下一个usemtl之间的f参数标识了一个object
		else if (!tmpName.compare("usemtl")) {
			tmpObject = new rootObject();
			tmpObject->type = objectType::MESH;
			objFile >> tmpName;
			tmpObject->mat = materialList[tmpName];
			objectList.push_back(tmpObject);
			numofObject++;
		}
		// vertex
		else if (!tmpName.compare("v")) {
			objFile >> tmpTA[0] >> tmpTA[1] >> tmpTA[2];
			vertexList.push_back(vec3d(tmpTA[0], tmpTA[1], tmpTA[2]));
			numOfVertex++;
		}
		// face
		else if (!tmpName.compare("f")) {
			std::vector<int> tmpVec;
			getline(objFile, tmpName);
			const char *tmpString = tmpName.c_str();
			int i;

			while (tmpString = strchr(tmpString, ' ')) {
				if (sscanf(tmpString, "%d", &i) != 1) {
					printf("Error face format in line %d\n", lineNum);
					return false;
				}
				i--; // .obj starts from 1 while array start from 0
				tmpVec.push_back(i);
				tmpString += strspn(tmpString, " ");
			}
			tmpObject->faceList.push_back(tmpVec);
		}
		// sphere
		else if (!tmpName.compare("sphere")) {
			tmpObject = new sphere();
			tmpObject->type = objectType::SPHERE;
			objFile >> tmpName;
			tmpObject->mat = materialList[tmpName];
			objFile >> tmpTA[0] >> tmpTA[1] >> tmpTA[2];
			((sphere *)tmpObject)->center = vec3d(tmpTA[0], tmpTA[1], tmpTA[2]);
			double tmpRadius;
			objFile >> tmpRadius;
			((sphere *)tmpObject)->radius = tmpRadius;
			objectList.push_back(tmpObject);
			numofObject++;
		}
		else {
			getline(objFile, tmpName);
		}

    }
    objFile.close();


	// 确定是否是光源
	for (int i = 0; i < objectList.size(); i++) {
		if (objectList[i]->mat->Ka.x > 1e-5)
			objectList[i]->mat->isLight = true;
		else
			objectList[i]->mat->isLight = false;
	}



    printf("Read from %s success!\n", filename);
    printf("Vertex Number = %d\n", numOfVertex);
    printf("Object Number = %d\n", numofObject);

    return true;
}