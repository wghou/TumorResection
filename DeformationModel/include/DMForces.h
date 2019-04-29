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
	int nodeNumber = 0;			// ģ�������ӵ�ĸ���
	std::list<uint16_t> nodesA;		// ģ��A�е����ӵ���
	std::list<uint16_t> nodesB;		// ģ��B�е����ӵ���
	std::list<float> len0;		// ���ӵ�֮��ĳ�ʼ����

	DeformationModelGPU *modelA;	// ģ��Aָ��
	DeformationModelGPU *modelB;	// ģ��Bָ��

	float* forcesModelA = nullptr;	// ������ģ��A�ϵ�����
	float* forcesModelB = nullptr;	// ������ģ��B�ϵ�����
};

#endif // !DMFORCES_H_

