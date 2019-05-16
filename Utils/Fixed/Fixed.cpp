// Fixed.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<sstream>
#include<fstream>
#include"ObjLoader/ObjLoader.h"

int main()
{
	ObjLoader loader;
	loader.loadObj("../../Samples/assets/models/pulse/brain/brain.obj");

	uint16_t numV = loader.getNumVertices();
	float* Vptr = loader.getVertices();

	std::vector<uint16_t> fxd;
	std::vector<uint16_t> cst;

	for (uint16_t i = 0; i < numV; i++) {
		if (Vptr[i * 3 + 0] > -3.5) {
			cst.push_back(i);
		}

		float len = sqrtf(Vptr[i * 3 + 1] * Vptr[i * 3 + 1] + Vptr[i * 3 + 2] * Vptr[i * 3 + 2]);

		if (len > 17.9) fxd.push_back(i);
	}

	std::cout << "Fixed vertices number: " << fxd.size() << "  Constraint Vertices number: " << cst.size() << std::endl;


	//////////////
	string fxdFile = "../../Samples/assets/models/pulse/brain/brain.fxd";
	ofstream fileStream;
	fileStream.open(fxdFile.c_str(), ios::trunc);
	if (fileStream.fail()) {
		return -1;
	}

	fileStream << fxd.size() << std::endl;

	for (int i = 0; i < fxd.size(); i++) {
		fileStream << fxd[i] << std::endl;
	}
	fileStream.close();


	//////////////
	string cstFile = "../../Samples/assets/models/pulse/brain/brain.cst";
	fileStream.open(cstFile.c_str(), ios::trunc);
	if (fileStream.fail()) {
		return -1;
	}

	fileStream << cst.size() << std::endl;

	for (int i = 0; i < cst.size(); i++) {
		fileStream << cst[i] << std::endl;
	}
	fileStream.close();

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
