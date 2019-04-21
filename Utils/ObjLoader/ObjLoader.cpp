#include"OBJLoader.h"
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>

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

vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}


void ObjLoader::loadObj(const std::string fileName)
{
	cout << "Loading " << fileName << endl;

	ifstream fileStream;
	fileStream.open(fileName.c_str());
	if (fileStream.fail())
	{
		std::cerr << "Failed to open file: " << fileName << endl;
		return;
	}

	string line_stream;
	uint16_t faceVertexindex = 0;

	while (getline(fileStream,line_stream))
	{
		stringstream str_stream(line_stream);
		string type_str;
		str_stream >> type_str;

		if (type_str == "v")
		{
			// 节点坐标
			float temp1, temp2, temp3;
			str_stream >> temp1 >> temp2 >> temp3;
			tVertices.push_back(Vertex{ temp1, temp2, temp3 });
		}
		else if (type_str == "vt")
		{
			// 纹理坐标
			float temp1, temp2;
			str_stream >> temp1 >> temp2;
			tVTs.push_back(TxtCord{ temp1,temp2 });
		}
		else if (type_str == "vn")
		{
			// 节点法向量
			float temp1, temp2, temp3;
			str_stream >> temp1 >> temp2 >> temp3;
			tVNs.push_back(Normal{ temp1,temp2,temp3 });
		}
		else if (type_str == "f")
		{
			// 三角形面片
			string temp1[3];
			str_stream >> temp1[0] >> temp1[1] >> temp1[2];

			// 格式为：index//index index//index index//index
			for (int i = 0; i < 3; i++)
			{
				vector<string> tpS = split(temp1[i], "/");
				if (tpS.size() != 3) {
					cerr << "error:" << endl;
					break;
				}

				uint16_t tpV[3];
				for (int j = 0; j < 3; j++)
				{
					tpV[j] = stoi(tpS[j]) - 1;
				}

				tFaces.push_back(FaceVertex{ faceVertexindex,tpV[0],tpV[1],tpV[2] });
				faceVertexindex++;
			}
		}
	}
	fileStream.close();

	numFaces = faceVertexindex / 3;

	// 优化网格
	optimMesh();

	return;
}

void ObjLoader::optimMesh()
{
	if (tVertices.size() == 0 || tVTs.size() == 0 || tVNs.size() == 0 || tFaces.size() == 0) {
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
	mNormals = new float[numVertices * 3];
	mTBNs = new float[numVertices * 4];
	mFaces = new uint16_t[tFaces.size()];

	int newIndex = tVertices.size();
	for (int i = 0; i < tFaces.size(); i++)
	{
		FaceVertex &fv = tFaces[i];

		// 唯一性顶点，则要继续判断是否需要新建顶点
		if (fv.index == i) {
			if (tagV[fv.v] == 0) {
				// 拷贝顶点到 sVertices 中
				mVertices[fv.v * 3 + 0] = tVertices[fv.v].x;
				mVertices[fv.v * 3 + 1] = tVertices[fv.v].y;
				mVertices[fv.v * 3 + 2] = tVertices[fv.v].z;
				mUVs[fv.v * 2 + 0] = tVTs[fv.vt].x;
				mUVs[fv.v * 2 + 1] = tVTs[fv.vt].y;
				mNormals[fv.v * 3 + 0] = tVNs[fv.vn].u;
				mNormals[fv.v * 3 + 1] = tVNs[fv.vn].v;
				mNormals[fv.v * 3 + 2] = tVNs[fv.vn].w;

				tagV[fv.v] = 1;

				// 写入顶点编号到 sFaces 中
				mFaces[i] = tFaces[fv.index].v;
			}
			// 需要新建顶点
			else {
				mVertices[newIndex * 3 + 0] = tVertices[fv.v].x;
				mVertices[newIndex * 3 + 1] = tVertices[fv.v].y;
				mVertices[newIndex * 3 + 2] = tVertices[fv.v].z;
				mUVs[newIndex * 2 + 0] = tVTs[fv.vt].x;
				mUVs[newIndex * 2 + 1] = tVTs[fv.vt].y;
				mNormals[newIndex * 3 + 0] = tVNs[fv.vn].u;
				mNormals[newIndex * 3 + 1] = tVNs[fv.vn].v;
				mNormals[newIndex * 3 + 2] = tVNs[fv.vn].w;

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
