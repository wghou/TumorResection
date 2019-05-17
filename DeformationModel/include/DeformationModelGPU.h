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

	// 初始化函数
	virtual void Initialize(DfModel_Config & config);

	// 执行仿真操作步骤
	virtual void timeStep(float time);

	void Reset_More_Fixed(int select_v, float dir[]);
	void SetExternalForce(float* externalForce);


public:
	virtual int getNodeNumber();
	virtual float* getX();
	virtual float* getVel();
	virtual uint16_t* getTet();				// 获取四面体单元格顶点编号
	virtual int getTetNumber();			// 获取四面体单元格顶点个数
	float * getExternalForce();


private:
	CUDA_HYPER_TET_MESH<float> * m_model = nullptr;		// 形变模型
	int selectVertex = -1;
	float vertexDir[3] = { 0,0,0 };
};


#endif // !DEFORMATIONMODELGPU_H_
