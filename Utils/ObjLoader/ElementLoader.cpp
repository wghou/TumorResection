#include "ElementLoader.h"
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>
#include<memory>

//#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"

#include"TBN.h"
#include"Logger/include/Logger.h"


ElementLoader::~ElementLoader()
{
	if (mVertices)	delete[] mVertices;
	if (mUVs)		delete[] mUVs;
	if (mNormals)	delete[] mNormals;
	if (mTBNs)		delete[] mTBNs;
	if (mFaces)		delete[] mFaces;
	if (mTets)		delete[] mTets;
}


void ElementLoader::loadElement(const string fileName)
{
	/////////  Node File //////////
	string nodeFile = fileName + ".node";
	Logger::getMainLogger().log(Logger::Level::Info, "Loading " + nodeFile, "ElementLoader::loadElement");

	ifstream fileStream1;
	fileStream1.open(nodeFile.c_str());
	if (fileStream1.fail())
	{
		Logger::getMainLogger().log(Logger::Level::Error, "Failed to open file: " + nodeFile, "ElementLoader::loadElement");
		return;
	}

	string line_stream;
	if (!getline(fileStream1, line_stream)) {
		Logger::getMainLogger().log(Logger::Level::Error, "There no node in the file " + nodeFile, "ElementLoader::loadElement");
		return;
	}

	stringstream num_stream(line_stream);
	num_stream >> numVertices;
	if (numVertices <= 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "The number of vertices seems wrong: ", "ElementLoader::loadElement");
		return;
	}

	for (int i = 0; i < numVertices; i++)
	{
		if (!getline(fileStream1, line_stream)) {
			Logger::getMainLogger().log(Logger::Level::Error, "Error occurs when reading line from the file " + nodeFile, "ElementLoader::loadElement");
			break;
		}

		float temp, x, y, z;
		stringstream str_stream(line_stream);
		str_stream >> temp >> x >> y >> z;
		tVertices.push_back(x);
		tVertices.push_back(y);
		tVertices.push_back(z);
	}

	fileStream1.close();


	///////// Ele File  //////////
	string eleFile = fileName + ".ele";
	Logger::getMainLogger().log(Logger::Level::Info, "Loading file: " + eleFile, "ElementLoader::loadElement");

	ifstream fileStream2;
	fileStream2.open(eleFile.c_str());
	if (fileStream2.fail())
	{
		Logger::getMainLogger().log(Logger::Level::Error, "Failed to open file: " + eleFile, "ElementLoader::loadElement");
		return;
	}

	if (!getline(fileStream2, line_stream)) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no node in file " + eleFile, "ElementLoader::loadElement");
		return;
	}

	stringstream num2_stream(line_stream);
	num2_stream >> numTets;
	if (numTets <= 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is something wrong with the tet number in " + eleFile, "ElementLoader::loadElement");
		return;
	}

	mTets = new uint16_t[numTets * 4];
	for (int i = 0; i < numTets; i++)
	{
		if (!getline(fileStream2, line_stream)) {
			Logger::getMainLogger().log(Logger::Level::Error, "Error occurs when reading line from the file " + eleFile, "ElementLoader::loadElement");
			break;
		}

		uint16_t temp, v0, v1, v2, v3;
		stringstream str_stream(line_stream);
		str_stream >> temp >> v0 >> v1 >> v2 >> v3;
		mTets[i * 4 + 0] = v0;
		mTets[i * 4 + 1] = v1;
		mTets[i * 4 + 2] = v2;
		mTets[i * 4 + 3] = v3;
	}

	fileStream2.close();


	///////// obj File  //////////
	string objFile = fileName + ".obj";

	Logger::getMainLogger().log(Logger::Level::Info, "Loading file: " + objFile, "ElementLoader::loadElement");

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFile.c_str());
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}
	if (!ret) {
		Logger::getMainLogger().log(Logger::Level::Error, "Error occurs when loading file" + objFile + "with tinyobj", "ElementLoader::loadElement");
		return;
	}

	tVTs.swap(attrib.texcoords);
	if (tVTs.size() == 0) tVTs.resize(2, 0);
	tVNs.resize(3, 0);
	
	if (shapes.size() != 1) {
		Logger::getMainLogger().log(Logger::Level::Error, "Something wrong with the shapes.size.", "ElementLoader::loadElement");
	}
	// loop over faces(ploygon)
	size_t index_offset = 0;
	for (size_t f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++) {
		int fv = shapes[0].mesh.num_face_vertices[f];

		if (fv != 3) {
			Logger::getMainLogger().log(Logger::Level::Error, "Error with fv", "ElementLoader::loadElement");
			break;
		}

		uint16_t tpV[3];
		// Loop over vertices in the face
		for (size_t v = 0; v < fv; v++) {
			tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];

			int v_index = idx.vertex_index;
			int t_index = idx.texcoord_index;
			int n_index = 0;
			if (v_index < 0) v_index = 0;
			if (t_index < 0) t_index = 0;
			tFaces.push_back({ (uint16_t)(index_offset + v), (uint16_t)v_index, (uint16_t)t_index, (uint16_t)n_index });
		}
		index_offset += fv;
	}

	numFaces = shapes[0].mesh.num_face_vertices.size();



	///////// 优化网格  //////////
	if (tVertices.size() == 0 || tFaces.size() == 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no mesh in the file", "ElementLoader::loadElement");
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
	numVertices = numVertices + countV - attrib.vertices.size() / 3;
	mVertices = new float[numVertices * 3];
	for (int i = 0; i < tVertices.size(); i++) {
		mVertices[i] = tVertices[i];
	}

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
				//mNormals[fv.v * 3 + 0] = tVNs[fv.vn * 3 + 0];
				//mNormals[fv.v * 3 + 1] = tVNs[fv.vn * 3 + 1];
				//mNormals[fv.v * 3 + 2] = tVNs[fv.vn * 3 + 2];

				tagV[fv.v] = 1;

				// 写入顶点编号到 sFaces 中
				mFaces[i] = tFaces[fv.index].v;
			}
			// 需要新建顶点
			else {
				if (newIndex >= numVertices) {
					Logger::getMainLogger().log(Logger::Level::Error, "Error occur when create new vertex.", "ElementLoader::loadElement");
					break;
				}

				mVertices[newIndex * 3 + 0] = tVertices[fv.v * 3 + 0];
				mVertices[newIndex * 3 + 1] = tVertices[fv.v * 3 + 1];
				mVertices[newIndex * 3 + 2] = tVertices[fv.v * 3 + 2];
				mUVs[newIndex * 2 + 0] = tVTs[fv.vt * 2 + 0];
				mUVs[newIndex * 2 + 1] = tVTs[fv.vt * 2 + 1];
				//mNormals[newIndex * 3 + 0] = tVNs[fv.vn * 3 + 0];
				//mNormals[newIndex * 3 + 1] = tVNs[fv.vn * 3 + 1];
				//mNormals[newIndex * 3 + 2] = tVNs[fv.vn * 3 + 2];

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

	// update normals
	TBN::buildVns(numFaces, mFaces, numVertices, mVertices, mNormals);

	// 生成 TBNs
	TBN::updateTBNs(numVertices, mNormals, mTBNs);

	return;
}

void ElementLoader::scale(float s)
{
	for (int i = 0; i < numVertices * 3; i++) {
		mVertices[i] *= s;
	}
}

void ElementLoader::translate(float _x, float _y, float _z)
{
	if (numVertices <= 0) return;

	for (int i = 0; i < numVertices; i++) {
		mVertices[3 * i + 0] += _x;
		mVertices[3 * i + 1] += _y;
		mVertices[3 * i + 2] += _z;
	}
}