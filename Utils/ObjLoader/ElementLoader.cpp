#include "ElementLoader.h"
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>

#include"TBN.h"


template<class T> __forceinline
void Swap(T &a, T &b)
{
	T c = a; a = b; b = c;
}

int QuickSort_Partition(int a[], int l, int r)
{
	int pivot[4], i, j, t[4];
	pivot[0] = a[l * 4 + 0];
	pivot[1] = a[l * 4 + 1];
	pivot[2] = a[l * 4 + 2];
	pivot[3] = a[l * 4 + 3];
	i = l; j = r + 1;

	while (1)
	{
		do ++i; while ((a[i * 4 + 0] < pivot[0] || a[i * 4 + 0] == pivot[0] && a[i * 4 + 1] < pivot[1] || a[i * 4 + 0] == pivot[0] && a[i * 4 + 1] == pivot[1] && a[i * 4 + 2] <= pivot[2]) && i <= r);
		do --j; while (a[j * 4 + 0] > pivot[0] || a[j * 4 + 0] == pivot[0] && a[j * 4 + 1] > pivot[1] || a[j * 4 + 0] == pivot[0] && a[j * 4 + 1] == pivot[1] && a[j * 4 + 2] > pivot[2]);
		if (i >= j) break;
		//Swap i and j
		t[0] = a[i * 4 + 0];
		t[1] = a[i * 4 + 1];
		t[2] = a[i * 4 + 2];
		t[3] = a[i * 4 + 3];
		a[i * 4 + 0] = a[j * 4 + 0];
		a[i * 4 + 1] = a[j * 4 + 1];
		a[i * 4 + 2] = a[j * 4 + 2];
		a[i * 4 + 3] = a[j * 4 + 3];
		a[j * 4 + 0] = t[0];
		a[j * 4 + 1] = t[1];
		a[j * 4 + 2] = t[2];
		a[j * 4 + 3] = t[3];
	}
	//Swap l and j
	t[0] = a[l * 4 + 0];
	t[1] = a[l * 4 + 1];
	t[2] = a[l * 4 + 2];
	t[3] = a[l * 4 + 3];
	a[l * 4 + 0] = a[j * 4 + 0];
	a[l * 4 + 1] = a[j * 4 + 1];
	a[l * 4 + 2] = a[j * 4 + 2];
	a[l * 4 + 3] = a[j * 4 + 3];
	a[j * 4 + 0] = t[0];
	a[j * 4 + 1] = t[1];
	a[j * 4 + 2] = t[2];
	a[j * 4 + 3] = t[3];
	return j;
}

void QuickSort(int a[], int l, int r)
{
	if (l < r)
	{
		int j = QuickSort_Partition(a, l, r);
		QuickSort(a, l, j - 1);
		QuickSort(a, j + 1, r);
	}
}

void ElementLoader::loadElement(const string fileName)
{
	/////////  Node File //////////
	string nodeFile = fileName + ".node";
	cout << "Loading " << nodeFile << endl;

	ifstream fileStream1;
	fileStream1.open(nodeFile.c_str());
	if (fileStream1.fail())
	{
		std::cerr << "Failed to open file: " << nodeFile << endl;
		return;
	}

	string line_stream;
	if (!getline(fileStream1, line_stream)) {
		cerr << "there no node in the file." << endl;
		return;
	}

	stringstream num_stream(line_stream);
	int numV = 0;
	num_stream >> numV;

	for (int i = 0; i < numV; i++)
	{
		if (!getline(fileStream1, line_stream)) {
			cerr << "error" << endl;
			break;
		}

		float temp, x, y, z;
		stringstream str_stream(line_stream);
		str_stream >> temp >> x >> y >> z;
		tVertices.push_back({ x, y, z });
		mVertices.push_back(x); mVertices.push_back(y); mVertices.push_back(z);
	}

	fileStream1.close();


	///////// Ele File  //////////
	string eleFile = fileName + ".ele";
	cout << "Loading " << eleFile << endl;

	ifstream fileStream2;
	fileStream2.open(eleFile.c_str());
	if (fileStream2.fail())
	{
		std::cerr << "Failed to open file: " << eleFile << endl;
		return;
	}

	if (!getline(fileStream2, line_stream)) {
		cerr << "there no node in the file." << endl;
		return;
	}

	stringstream num2_stream(line_stream);
	int numT = 0;
	num2_stream >> numT;

	for (int i = 0; i < numT; i++)
	{
		if (!getline(fileStream2, line_stream)) {
			cerr << "error" << endl;
			break;
		}

		uint16_t temp, v0, v1, v2, v3;
		stringstream str_stream(line_stream);
		str_stream >> temp >> v0 >> v1 >> v2 >> v3;
		mTets.push_back({ v0, v1, v2, v3 });
	}

	fileStream2.close();

	////////  生成表面网格  ////////
	Build_Boundary_Triangles();

	////////  计算法向量  ////////
	TBN::buildVns(mFaces.size() / 3, mFaces.data(), mVertices.size() / 3, mVertices.data(), mNormals.data());

	////////  初始化纹理坐标  ////////
	mUVs.resize(mVertices.size() / 3 * 2, 0);
}

void ElementLoader::Build_Boundary_Triangles()
{
	int tet_number = mTets.size();
	if (tet_number == 0) {
		cerr << "There is no tetradedron." << endl;
		return;
	}

	int *temp_T = new int[tet_number * 4 * 4];
	for (int i = 0; i < tet_number; i++)
	{
		temp_T[i * 16 + 0] = mTets[i].v0;
		temp_T[i * 16 + 1] = mTets[i].v1;
		temp_T[i * 16 + 2] = mTets[i].v2;
		temp_T[i * 16 + 3] = 1;

		temp_T[i * 16 + 4] = mTets[i].v0;
		temp_T[i * 16 + 5] = mTets[i].v2;
		temp_T[i * 16 + 6] = mTets[i].v3;
		temp_T[i * 16 + 7] = 1;

		temp_T[i * 16 + 8] = mTets[i].v0;
		temp_T[i * 16 + 9] = mTets[i].v3;
		temp_T[i * 16 + 10] = mTets[i].v1;
		temp_T[i * 16 + 11] = 1;

		temp_T[i * 16 + 12] = mTets[i].v1;
		temp_T[i * 16 + 13] = mTets[i].v3;
		temp_T[i * 16 + 14] = mTets[i].v2;
		temp_T[i * 16 + 15] = 1;
	}

	for (int i = 0; i < tet_number * 4; i++)
	{
		if (temp_T[i * 4 + 1] < temp_T[i * 4 + 0])
		{
			Swap(temp_T[i * 4 + 0], temp_T[i * 4 + 1]);
			temp_T[i * 4 + 3] = (temp_T[i * 4 + 3] + 1) % 2;
		}
		if (temp_T[i * 4 + 2] < temp_T[i * 4 + 0])
		{
			Swap(temp_T[i * 4 + 0], temp_T[i * 4 + 2]);
			temp_T[i * 4 + 3] = (temp_T[i * 4 + 3] + 1) % 2;
		}
		if (temp_T[i * 4 + 2] < temp_T[i * 4 + 1])
		{
			Swap(temp_T[i * 4 + 1], temp_T[i * 4 + 2]);
			temp_T[i * 4 + 3] = (temp_T[i * 4 + 3] + 1) % 2;
		}
	}

	QuickSort(temp_T, 0, tet_number * 4 - 1);

	for (int i = 0; i < tet_number * 4; i++)
	{
		if (i != tet_number * 4 - 1 && temp_T[i * 4 + 0] == temp_T[i * 4 + 4] && temp_T[i * 4 + 1] == temp_T[i * 4 + 5] && temp_T[i * 4 + 2] == temp_T[i * 4 + 6])
		{
			i++;
			continue;
		}

		if (temp_T[i * 4 + 3] == 1)
		{
			mFaces.push_back(temp_T[i * 4 + 0]);
			mFaces.push_back(temp_T[i * 4 + 1]);
			mFaces.push_back(temp_T[i * 4 + 2]);
		}
		else
		{
			mFaces.push_back(temp_T[i * 4 + 1]);
			mFaces.push_back(temp_T[i * 4 + 0]);
			mFaces.push_back(temp_T[i * 4 + 2]);
		}
	}

	delete[]temp_T;
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


void ElementLoader::loadTextureCoord(const string fileName)
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

	tVTs.clear(); tVNs.clear(); tFaces.clear();
	while (getline(fileStream, line_stream))
	{
		stringstream str_stream(line_stream);
		string type_str;
		str_stream >> type_str;

		if (type_str == "v")
		{
			// 节点坐标
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

	// 默认已经载入四面体网格信息了
	int numVertices = countV - tVertices.size() + mVertices.size() / 3;
	int numFaces = tFaces.size();
	int *tagV = new int[numVertices];
	memset(tagV, 0, numVertices * sizeof(int));
	mUVs.resize(numVertices * 2, 0);
	mNormals.resize(numVertices * 3, 0);
	mFaces.resize(numFaces, 0);

	int newIndex = tVertices.size();
	for (int i = 0; i < tFaces.size(); i++)
	{
		FaceVertex &fv = tFaces[i];

		// 唯一性顶点，则要继续判断是否需要新建顶点
		if (fv.index == i) {
			if (tagV[fv.v] == 0) {
				// 拷贝顶点到 sVertices 中
				//mVertices[fv.v * 3 + 0] = tVertices[fv.v].x;
				//mVertices[fv.v * 3 + 1] = tVertices[fv.v].y;
				//mVertices[fv.v * 3 + 2] = tVertices[fv.v].z;
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
				mVertices.push_back(tVertices[fv.v].x);
				mVertices.push_back(tVertices[fv.v].y);
				mVertices.push_back(tVertices[fv.v].z);
				//mVertices[newIndex * 3 + 0] = tVertices[fv.v].x;
				//mVertices[newIndex * 3 + 1] = tVertices[fv.v].y;
				//mVertices[newIndex * 3 + 2] = tVertices[fv.v].z;
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

	// 生成法向量 Tangent biTangent Normal
	TBN::updateTBNs(mNormals, mTBNs);
}
