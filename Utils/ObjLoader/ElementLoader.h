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
	int getNumFaces() { return numFaces; }
	int getNumTets() { return numTets; }
	float* getVertices() { return mVertices; }
	float* getUVs() { return mUVs; }
	float* getTBNs() { return mTBNs; }
	uint16_t* getFaces() { return mFaces; }
	uint16_t* getTets() { return mTets; }
	std::vector<uint16_t> &getFixed() { return mFixed; }
	std::vector<uint16_t> &getVertCpys() { return mVertCpys; }

	// 返回 Tet 网格中的节点个数，实际上就是参与形变计算的节点个数
	// 因为表面网格纹理贴图的关系，所以参与形变计算的节点跟表面网格节点不太对应
	// 其实，应该跟 sofa 一样，独立出一个 surface mesh，surface mesh 与形变节点之间有一个映射关系
	// 这样做其实会清晰很多的。
	// 后面做改进的时候吧，将 surface mesh 独立出来
	int getTetVertNum() { return tVertices.size() / 3; }

	// 计算表面上的约束点
	//vector<uint16_t> &calCstVert(float alitude);
	//void calculateFixed(float radius);

	void scale(float s = 1);
	void translate(float _x, float _y, float _z);

private:
	void loadObjFile(std::string objFile);
	void loadNodeFile(std::string nodeFile);
	void loadEleFile(std::string eleFile);
	void loadFxdFile(std::string fxdFile);
	void loadCstFile(std::string cstFile);
	void optimizeMesh();

private:
	vector<float> tVertices;
	vector<float> tVTs;
	vector<float> tVNs;
	vector<FaceVertex> tFaces;

	// constraint vertex
	vector<uint16_t> tCstVert;
	

	float* mVertices = nullptr;
	float* mUVs = nullptr;
	float* mNormals = nullptr;
	float* mTBNs = nullptr;
	uint16_t* mFaces = nullptr;
	uint16_t* mTets = nullptr;
	std::vector<uint16_t> mFixed;
	std::vector<uint16_t> mVertCpys;	// 复制的节点编号 pairs 

	int numVertices = 0;
	int numFaces = 0;
	int numTets = 0;
	int numVerticesObj = 0;
};
#endif // !ELEMENT_LOADER_H

