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
	int getNumFaceVertices() { return numFaces; }
	vector<float> getVertices() { return mVertices; }
	vector<float> getUVs() { return mUVs; }
	vector<float> getTBNs() { return mTBNs; }
	vector<uint16_t> getFaces() { return mFaces; }

protected:
	void optimMesh();

private:
	// Êý¾Ý»º´æ
	vector<Vertex> tVertices;
	vector<TxtCord> tVTs;
	vector<Normal> tVNs;
	vector<FaceVertex> tFaces;


	vector<float> mVertices;
	vector<float> mUVs;
	vector<float> mNormals;
	vector<float> mTBNs;
	vector<uint16_t> mFaces;


	int numVertices = 0;
	int numFaces = 0;
};

#endif // !OBJLOADER_H
