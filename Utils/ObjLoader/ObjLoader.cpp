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
		//�ҵ��ַ������׸������ڷָ�������ĸ��
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

		//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
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
			// �ڵ�����
			float temp1, temp2, temp3;
			str_stream >> temp1 >> temp2 >> temp3;
			tVertices.push_back(Vertex{ temp1, temp2, temp3 });
		}
		else if (type_str == "vt")
		{
			// ��������
			float temp1, temp2;
			str_stream >> temp1 >> temp2;
			tVTs.push_back(TxtCord{ temp1,temp2 });
		}
		else if (type_str == "vn")
		{
			// �ڵ㷨����
			float temp1, temp2, temp3;
			str_stream >> temp1 >> temp2 >> temp3;
			tVNs.push_back(Normal{ temp1,temp2,temp3 });
		}
		else if (type_str == "f")
		{
			// ��������Ƭ
			string temp1[3];
			str_stream >> temp1[0] >> temp1[1] >> temp1[2];

			// ��ʽΪ��index//index index//index index//index
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

	// �Ż�����
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

		// �Ѿ��Աȳ�����ģ�����
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

		// Ψһ�Զ��㣬��Ҫ�����ж��Ƿ���Ҫ�½�����
		if (fv.index == i) {
			if (tagV[fv.v] == 0) {
				// �������㵽 sVertices ��
				mVertices[fv.v * 3 + 0] = tVertices[fv.v].x;
				mVertices[fv.v * 3 + 1] = tVertices[fv.v].y;
				mVertices[fv.v * 3 + 2] = tVertices[fv.v].z;
				mUVs[fv.v * 2 + 0] = tVTs[fv.vt].x;
				mUVs[fv.v * 2 + 1] = tVTs[fv.vt].y;
				mNormals[fv.v * 3 + 0] = tVNs[fv.vn].u;
				mNormals[fv.v * 3 + 1] = tVNs[fv.vn].v;
				mNormals[fv.v * 3 + 2] = tVNs[fv.vn].w;

				tagV[fv.v] = 1;

				// д�붥���ŵ� sFaces ��
				mFaces[i] = tFaces[fv.index].v;
			}
			// ��Ҫ�½�����
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
		// ������ظ��ԵĶ��㣬ֻ��Ҫ��������ȷ�ı�ţ�д�� sFaces �м���
		else {
			mFaces[i] = tFaces[fv.index].v;
		}
	}

	delete tagV;

	// ���� TBNs
	TBN::updateTBNs(numVertices, mNormals, mTBNs);

	return;
}
