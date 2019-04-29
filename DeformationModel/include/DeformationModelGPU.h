// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// DeformationModelGPU is based on descent method.
#ifndef DEFORMATIONMODELGPU_H_
#define DEFORMATIONMODELGPU_H_

#include"DeformationModelBase.h"

template <class TYPE>
class CUDA_HYPER_TET_MESH;

class SoftObjectGPU;

class DeformationModelGPU : public DeformationModelBase
{
	friend SoftObjectGPU;

public:
	DeformationModelGPU();
	~DeformationModelGPU();

	// ��ʼ������
	virtual void Initialize(DfModel_Config & config);

	// ִ�з����������
	virtual void timeStep(float time);

	void Reset_More_Fixed(int select_v, float dir[]);
	void SetExternalForce(float* externalForce);

	virtual void Scale(float s);
	virtual void Translate(float x, float y, float z);
	virtual void Rotate_X(float angle);
	virtual void Rotate_Y(float angle);
	virtual void Rotate_Z(float angle);


public:
	virtual int getNodeNumber();
	virtual float* getX();
	virtual float* getVel();
	virtual float* getVN();				// ��ȡ���㷨����
	virtual int getTriNumber();
	virtual uint16_t* getTriIndex();
	virtual uint16_t* getTet();				// ��ȡ�����嵥Ԫ�񶥵���
	virtual int getTetNumber();			// ��ȡ�����嵥Ԫ�񶥵����


private:
	CUDA_HYPER_TET_MESH<float> * m_model = nullptr;		// �α�ģ��
	int selectVertex = -1;
	float vertexDir[3] = { 0,0,0 };
};


#endif // !DEFORMATIONMODELGPU_H_
