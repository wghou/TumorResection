// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// DeformationModelBase describe the basic interface for simulating deformable objects.
//
#ifndef DEFORMATIONMODELBASE_H_
#define DEFORMATIONMODELBASE_H_

#include<string>

class DfModel_Config 
{
public:
	std::string modelType;

	std::string fileName;

	bool indexDec = false;

	float scale = 1.0f;
	bool centralize = false;
	float trans[3] = { 0.0f, 0.0f, 0.0f };
	float rote[3] = { 0.0f, 0.0f, 0.0f };

	float fixedAxisUp[3] = { 0.0f, 0.0f, 0.0f };
	float fixedAxisBt[3] = { 0.0f, 0.0f, 0.0f };
};

class DeformationModelBase
{
public:
	virtual ~DeformationModelBase(){}

	virtual void Initialize(DfModel_Config &config) = 0;
	virtual void timeStep(float time) = 0;	// 仿真步骤

	virtual void Scale(float s);
	virtual void Translate(float x, float y, float z);
	virtual void Rotate_X(float angle) {};
	virtual void Rotate_Y(float angle) {};
	virtual void Rotate_Z(float angle) {};


protected:
	virtual int getNodeNumber() = 0;		// 网格节点的个数
	virtual float* getX() = 0;				// 网格节点的坐标
	virtual float* getVel() = 0;			// 网格节点的速度
	virtual float* getVN() = 0;				// 获取顶点法向量
	virtual int getTriNumber() = 0;			// 获取表面网格三角形面片顶点个数
	virtual uint16_t* getTriIndex() = 0;			// 获取表面网格三角形面片顶点编号
	virtual uint16_t* getTet() = 0;				// 获取四面体单元格顶点编号
	virtual int getTetNumber() = 0;			// 获取四面体单元格顶点个数
};

#endif // !DEFORMATIONMODELBASE_H_
