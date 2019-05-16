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

	// ���� Tet �����еĽڵ������ʵ���Ͼ��ǲ����α����Ľڵ����
	// ��Ϊ��������������ͼ�Ĺ�ϵ�����Բ����α����Ľڵ����������ڵ㲻̫��Ӧ
	// ��ʵ��Ӧ�ø� sofa һ����������һ�� surface mesh��surface mesh ���α�ڵ�֮����һ��ӳ���ϵ
	// ��������ʵ�������ܶ�ġ�
	// �������Ľ���ʱ��ɣ��� surface mesh ��������
	int getTetVertNum() { return tVertices.size() / 3; }

	// ��������ϵ�Լ����
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
	std::vector<uint16_t> mVertCpys;	// ���ƵĽڵ��� pairs 

	int numVertices = 0;
	int numFaces = 0;
	int numTets = 0;
	int numVerticesObj = 0;
};
#endif // !ELEMENT_LOADER_H

