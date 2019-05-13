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
#include<vector>

class DfModel_Config 
{
public:
	std::string modelType;
	int numVertex = 0;
	int tetVertNum = 0;
	float* mVertices = nullptr;
	int numTet = 0;
	uint16_t* mTets = nullptr;

	float fixedAxisUp[3] = { 0.0f, 0.0f, 0.0f };
	float fixedAxisBt[3] = { 0.0f, 0.0f, 0.0f };

	std::vector<uint16_t> fixedVertices;
};

class DeformationModelBase
{
public:
	virtual ~DeformationModelBase(){}

	virtual void Initialize(DfModel_Config &config) = 0;
	virtual void timeStep(float time) = 0;	// ���沽��

	virtual void Scale(float s) {};
	virtual void Translate(float x, float y, float z) {};
	virtual void Rotate_X(float angle) {};
	virtual void Rotate_Y(float angle) {};
	virtual void Rotate_Z(float angle) {};


protected:
	virtual int getNodeNumber() = 0;		// ����ڵ�ĸ���
	virtual float* getX() = 0;				// ����ڵ������
	virtual float* getVel() = 0;			// ����ڵ���ٶ�
	virtual uint16_t* getTet() = 0;				// ��ȡ�����嵥Ԫ�񶥵���
	virtual int getTetNumber() = 0;			// ��ȡ�����嵥Ԫ�񶥵����
};

#endif // !DEFORMATIONMODELBASE_H_
