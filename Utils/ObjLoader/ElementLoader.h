#ifndef ELEMENT_LOADER_H
#define ELEMENT_LOADER_H

#include <string>
#include <vector>
#include"Vertex.h"

using namespace std;
class ElementLoader
{
public:
	~ElementLoader();

	void loadElement(const string fileName);

	int getNumVertices() { return numVertices; }
	int getNumFaceVertices() { return numFaces; }
	int getNumTets() { return numTets; }
	float* getVertices() { return mVertices; }
	float* getUVs() { return mUVs; }
	float* getTBNs() { return mTBNs; }
	uint16_t* getFaces() { return mFaces; }
	uint16_t* getTets() { return mTets; }

private:
	vector<float> tVertices;
	vector<float> tVTs;
	vector<float> tVNs;
	vector<FaceVertex> tFaces;
	

	float* mVertices = nullptr;
	float* mUVs = nullptr;
	float* mNormals = nullptr;
	float* mTBNs = nullptr;
	uint16_t* mFaces = nullptr;
	uint16_t* mTets = nullptr;

	int numVertices = 0;
	int numFaces = 0;
	int numTets = 0;

};
#endif // !ELEMENT_LOADER_H

