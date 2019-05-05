#include"DMForces.h"

#include<assert.h>

#include"DeformationModelGPU.h"

#include"Logger/include/Logger.h"

DMForces::~DMForces()
{
	if (forcesModelA) delete[] forcesModelA;
	if (forcesModelB) delete[] forcesModelB;
}

void DMForces::intiDMForces(int nodeNum, std::vector<uint16_t> &ndA, std::vector<uint16_t> &ndB, DeformationModelGPU *mdA, DeformationModelGPU *mdB)
{
	nodeNumber = nodeNum;


	if (ndA.size() != nodeNum || ndB.size() != nodeNum)
	{
		Logger::getMainLogger().log(Logger::Level::Error, "vector size incorrect.", "DNForces::initDMForces");
	}

	for (int i = 0; i < nodeNum; i++)
	{
		nodesA.push_back(ndA[i]);
		nodesB.push_back(ndB[i]);

		float len = sqrt((mdA->getX()[ndA[i] * 3 + 0] - mdB->getX()[ndB[i] * 3 + 0]) * (mdA->getX()[ndA[i] * 3 + 0] - mdB->getX()[ndB[i] * 3 + 0]) + 
			(mdA->getX()[ndA[i] * 3 + 1] - mdB->getX()[ndB[i] * 3 + 1]) * (mdA->getX()[ndA[i] * 3 + 1] - mdB->getX()[ndB[i] * 3 + 1]) + 
			(mdA->getX()[ndA[i] * 3 + 2] - mdB->getX()[ndB[i] * 3 + 2]) * (mdA->getX()[ndA[i] * 3 + 2] - mdB->getX()[ndB[i] * 3 + 2]));

		len0.push_back(len);
	}

	modelA = mdA; modelB = mdB;

	// ·ÖÅäÄÚ´æ¿Õ¼ä
	forcesModelA = new float[modelA->getNodeNumber() * 3];
	forcesModelB = new float[modelB->getNodeNumber() * 3];
}


void DMForces::calcuateForces(float k, float damp)
{
	memset(forcesModelA, 0, sizeof(float)*modelA->getNodeNumber() * 3);
	memset(forcesModelB, 0, sizeof(float)*modelB->getNodeNumber() * 3);

	std::list<uint16_t>::iterator itB = nodesB.begin();
	std::list<float>::iterator itLen = len0.begin();
	for (std::list<uint16_t>::iterator itA = nodesA.begin(); itA != nodesA.end(); itA++)
	{

		float distance[3];
		distance[0] = modelA->getX()[*itA * 3 + 0] - modelB->getX()[*itB * 3 + 0];
		distance[1] = modelA->getX()[*itA * 3 + 1] - modelB->getX()[*itB * 3 + 1];
		distance[2] = modelA->getX()[*itA * 3 + 2] - modelB->getX()[*itB * 3 + 2];

		float velocity[3];
		velocity[0] = modelA->getVel()[*itA * 3 + 0] - modelB->getVel()[*itB * 3 + 0];
		velocity[1] = modelA->getVel()[*itA * 3 + 1] - modelB->getVel()[*itB * 3 + 1];
		velocity[2] = modelA->getVel()[*itA * 3 + 2] - modelB->getVel()[*itB * 3 + 2];

		float len1 = sqrt(distance[0] * distance[0] + distance[1] * distance[1] + distance[2] * distance[2]);

		float force = k*(len1 - *itLen);

		forcesModelA[*itA * 3 + 0] = force*distance[0] / len1 + damp* modelA->getVel()[*itA * 3 + 0];
		forcesModelA[*itA * 3 + 1] = force*distance[1] / len1 + damp* modelA->getVel()[*itA * 3 + 1];
		forcesModelA[*itA * 3 + 2] = force*distance[2] / len1 + damp* modelA->getVel()[*itA * 3 + 2];

		forcesModelB[*itB * 3 + 0] = -force*distance[0] / len1 + damp*modelB->getVel()[*itB * 3 + 0];
		forcesModelB[*itB * 3 + 1] = -force*distance[1] / len1 + damp*modelB->getVel()[*itB * 3 + 1];
		forcesModelB[*itB * 3 + 2] = -force*distance[2] / len1 + damp*modelB->getVel()[*itB * 3 + 2];


		if (itB != nodesB.end()) itB++;
		if (itLen != len0.end()) itLen++;
	}
}

void DMForces::disconnectSpring(float mul)
{
	std::list<uint16_t>::iterator itB = nodesB.begin();
	std::list<float>::iterator itLen = len0.begin();
	for (std::list<uint16_t>::iterator itA = nodesA.begin(); itA != nodesA.end();)
	{

		float distance[3];
		distance[0] = modelA->getX()[*itA * 3 + 0] - modelB->getX()[*itB * 3 + 0];
		distance[1] = modelA->getX()[*itA * 3 + 1] - modelB->getX()[*itB * 3 + 1];
		distance[2] = modelA->getX()[*itA * 3 + 2] - modelB->getX()[*itB * 3 + 2];

		float len1 = sqrt(distance[0] * distance[0] + distance[1] * distance[1] + distance[2] * distance[2]);

		// É¾³ý
		if (len1 > *itLen * mul)
		{
			itB = nodesB.erase(itB);
			itA = nodesA.erase(itA);
			itLen = len0.erase(itLen);
		}
		else
		{
			itA++;
			itB++;
			itLen++;
		}
	}
}