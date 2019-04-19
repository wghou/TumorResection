#ifndef ELEMENT_LOADER_H
#define ELEMENT_LOADER_H

#include <string>
#include <vector>
#include"Vertex.h"

using namespace std;
class ElementLoader
{
public:

	void loadElement(const string fileName);
	void loadTextureCoord(const string fileName);

protected:
	void Build_Boundary_Triangles();

private:
	vector<Vertex> tVertices;
	vector<TxtCord> tVTs;
	vector<Normal> tVNs;
	vector<FaceVertex> tFaces;
	

	vector<float> mVertices;
	vector<float> mUVs;
	vector<float> mNormals;
	vector<float> mTBNs;
	vector<uint16_t> mFaces;
	vector<Tetrahedron> mTets;

};
#endif // !ELEMENT_LOADER_H

