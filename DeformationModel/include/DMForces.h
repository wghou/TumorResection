#ifndef DMFORCES_H_
#define DMFORCES_H_

#include<list>
#include<vector>
#include<math.h>
#include<iostream>

class DeformationModelGPU;

class DMForces
{
public:
	~DMForces();

	void intiDMForces(int nodeNum, std::vector<uint16_t> &ndA, std::vector<uint16_t> &ndB, DeformationModelGPU *mdA, DeformationModelGPU *mdB);
	void calcuateForces(float k, float damp);
	void disconnectSpring(float mul);

	float* getForcesModelA() { return forcesModelA; }
	float* getForcesModelB() { return forcesModelB; }

	DeformationModelGPU* getModelA() { return modelA; }
	DeformationModelGPU* getModelB() { return modelB; }
	std::list<uint16_t> & getNodesA() { return nodesA; }
	std::list<uint16_t> & getNodesB() { return nodesB; }


private:
	int nodeNumber = 0;			// 模型中连接点的个数
	std::list<uint16_t> nodesA;		// 模型A中的连接点编号
	std::list<uint16_t> nodesB;		// 模型B中的连接点编号
	std::list<float> len0;		// 连接点之间的初始长度

	DeformationModelGPU *modelA;	// 模型A指针
	DeformationModelGPU *modelB;	// 模型B指针

	float* forcesModelA = nullptr;	// 作用于模型A上的外力
	float* forcesModelB = nullptr;	// 作用于模型B上的外力
};

#endif // !DMFORCES_H_

