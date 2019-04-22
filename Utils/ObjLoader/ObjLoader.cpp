#include"OBJLoader.h"
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"

#include "TBN.h"

using namespace std;

ObjLoader::~ObjLoader()
{
	if (mVertices)	delete[] mVertices;
	if (mUVs)		delete[] mUVs;
	if (mNormals)	delete[] mNormals;
	if (mTBNs)		delete[] mTBNs;
	if (mFaces)		delete[] mFaces;
}


void ObjLoader::loadObj(const std::string fileName)
{
	cout << "Loading " << fileName << endl;

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str());
	if (!err.empty()) {
		cerr << err << endl;
	}
	if (!ret) {
		cerr << "Error." << endl;
		return;
	}

	tVertices.swap(attrib.vertices);
	tVTs.swap(attrib.texcoords);
	tVNs.swap(attrib.normals);
	if (tVertices.size() == 0) tVertices.resize(3, 0);
	if (tVTs.size() == 0) tVTs.resize(2, 0);
	if (tVNs.size() == 0) tVNs.resize(3, 0);
	
	if (shapes.size() != 1) {
		cout << "error shpes.size" << endl;
	}
	// loop over faces(ploygon)
	size_t index_offset = 0;
	for (size_t f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++) {
		int fv = shapes[0].mesh.num_face_vertices[f];

		if (fv != 3) {
			cerr << "error fv" << endl;
			break;
		}

		uint16_t tpV[3];
		// Loop over vertices in the face
		for (size_t v = 0; v < fv; v++) {
			tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];

			int v_index = idx.vertex_index;
			int t_index = idx.texcoord_index;
			int n_index = idx.normal_index;
			if (v_index < 0) v_index = 0;
			if (t_index < 0) t_index = 0;
			if (n_index < 0) n_index = 0;
			tFaces.push_back({ (uint16_t)(index_offset + v), (uint16_t)v_index, (uint16_t)t_index, (uint16_t)n_index });
		}
		index_offset += fv;
	}

	numFaces = shapes[0].mesh.num_face_vertices.size();

	// 优化网格
	optimMesh();

	return;
}

void ObjLoader::optimMesh()
{
	if (tVertices.size() == 0 || tFaces.size() == 0) {
		cerr << "There is no mesh." << endl;
		return;
	}

	// optimization
	// identify the vertex exculsively
	int countV = tFaces.size();
	for (int i = 0; i < tFaces.size(); i++)
	{
		FaceVertex &fv = tFaces[i];

		// 已经对比出结果的，跳过
		if (fv.index != i) continue;

		for (int j = i + 1; j < tFaces.size(); j++)
		{
			FaceVertex &fv_cp = tFaces[j];

			if (fv == fv_cp) {
				fv_cp.index = fv.index;
				countV--;
			}
		}
	}


	// allocate new copy for mesh information
	numVertices = countV;
	mVertices = new float[numVertices * 3];
	int *tagV = new int[numVertices];
	memset(tagV, 0, numVertices * sizeof(int));
	mUVs = new float[numVertices * 2];
	memset(mUVs, 0, numVertices * 2 * sizeof(float));
	mNormals = new float[numVertices * 3];
	memset(mNormals, 0, numVertices * 3 * sizeof(float));
	mTBNs = new float[numVertices * 4];
	memset(mTBNs, 0, numVertices * 4 * sizeof(float));
	mFaces = new uint16_t[tFaces.size()];
	memset(mFaces, 0, tFaces.size() * sizeof(uint16_t));

	int newIndex = tVertices.size() / 3;
	for (int i = 0; i < tFaces.size(); i++)
	{
		FaceVertex &fv = tFaces[i];

		// 唯一性顶点，则要继续判断是否需要新建顶点
		if (fv.index == i) {
			if (tagV[fv.v] == 0) {
				// 拷贝顶点到 sVertices 中
				mVertices[fv.v * 3 + 0] = tVertices[fv.v * 3 + 0];
				mVertices[fv.v * 3 + 1] = tVertices[fv.v * 3 + 1];
				mVertices[fv.v * 3 + 2] = tVertices[fv.v * 3 + 2];
				mUVs[fv.v * 2 + 0] = tVTs[fv.vt * 2 + 0];
				mUVs[fv.v * 2 + 1] = tVTs[fv.vt * 2 + 1];
				mNormals[fv.v * 3 + 0] = tVNs[fv.vn * 3 + 0];
				mNormals[fv.v * 3 + 1] = tVNs[fv.vn * 3 + 1];
				mNormals[fv.v * 3 + 2] = tVNs[fv.vn * 3 + 2];

				tagV[fv.v] = 1;

				// 写入顶点编号到 sFaces 中
				mFaces[i] = tFaces[fv.index].v;
			}
			// 需要新建顶点
			else {
				if (newIndex >= numVertices) {
					cout << "err";
				}

				mVertices[newIndex * 3 + 0] = tVertices[fv.v * 3 + 0];
				mVertices[newIndex * 3 + 1] = tVertices[fv.v * 3 + 1];
				mVertices[newIndex * 3 + 2] = tVertices[fv.v * 3 + 2];
				mUVs[newIndex * 2 + 0] = tVTs[fv.vt * 2 + 0];
				mUVs[newIndex * 2 + 1] = tVTs[fv.vt * 2 + 1];
				mNormals[newIndex * 3 + 0] = tVNs[fv.vn * 3 + 0];
				mNormals[newIndex * 3 + 1] = tVNs[fv.vn * 3 + 1];
				mNormals[newIndex * 3 + 2] = tVNs[fv.vn * 3 + 2];

				fv.v = newIndex++;

				tagV[fv.v] = 1;

				mFaces[i] = fv.v;
			}
		}
		// 如果是重复性的顶点，只需要索引到正确的编号，写入 sFaces 中即可
		else {
			mFaces[i] = tFaces[fv.index].v;
		}
	}

	delete tagV;

	// 生成 TBNs
	TBN::updateTBNs(numVertices, mNormals, mTBNs);

	return;
}
