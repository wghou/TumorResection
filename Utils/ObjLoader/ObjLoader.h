#pragma once
#ifndef OBJLOADER_H
#define OBJLOADER_H

#include<string>
#include<vector>
#include"Vertex.h"

using namespace std;

class ObjLoader
{
public:
	~ObjLoader();

	void loadObj(const std::string fileName);


	int getNumVertices() { return numVertices; }
	int getNumFaces() { return numFaces; }
	float* getVertices() { return mVertices; }
	float* getUVs() { return mUVs; }
	float* getTBNs() { return mTBNs; }
	uint16_t* getFaces() { return mFaces; }

protected:
	void optimMesh();

private:
	// Êý¾Ý»º´æ
	vector<float> tVertices;
	vector<float> tVTs;
	vector<float> tVNs;
	vector<FaceVertex> tFaces;


	float* mVertices = nullptr;
	float* mUVs = nullptr;
	float* mNormals = nullptr;
	float* mTBNs = nullptr;
	uint16_t* mFaces = nullptr;


	int numVertices = 0;
	int numFaces = 0;
};

#endif // !OBJLOADER_H
