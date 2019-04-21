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
	
protected:
	void loadFaceVertices(const string fileName);

private:
	vector<Vertex> tVertices;
	vector<TxtCord> tVTs;
	vector<Normal> tVNs;
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

