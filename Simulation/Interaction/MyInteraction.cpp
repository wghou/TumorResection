#include"MyInteraction.h"

#include"DeformationModelGPU.h"
#include"DMForces.h"

#include"Object\SoftObjectGPU.h"


#include"Logger.h"

#include<vector>

MyInteraction::MyInteraction()
{
	m_dmForces = new DMForces();
}

MyInteraction::~MyInteraction()
{
	delete m_dmForces;
}

void MyInteraction::processBeforeTimeStep()
{
	imposeInteractionForces(1450, 1150);	// 2000000000
}

void MyInteraction::processAfterTimeStep()
{
	disconnectSpring(true, 7);
}

void MyInteraction::init(SoftObjectGPU *obA, SoftObjectGPU *obB, float distance)
{
	int number = 0; 
	std::vector<uint16_t> ndA, ndB;

	DeformationModelGPU* dmA = obA->getDeformationModel();
	DeformationModelGPU* dmB = obB->getDeformationModel();

	for (int i = 0; i < dmA->getNodeNumber(); i++)
	{
		for (int j = 0; j < dmB->getNodeNumber(); j++)
		{
			float xA = dmA->getX()[i * 3 + 0];
			float yA = dmA->getX()[i * 3 + 1];
			float zA = dmA->getX()[i * 3 + 2];

			float xB = dmB->getX()[j * 3 + 0];
			float yB = dmB->getX()[j * 3 + 1];
			float zB = dmB->getX()[j * 3 + 2];

			float len = sqrtf((xA - xB)*(xA - xB) + (yA - yB)*(yA - yB) + (zA - zB)*(zA - zB));

			if (len < distance)
			{
				ndA.push_back(i); ndB.push_back(j);
				break;
			}
		}
	}

	number = ndA.size();

	m_dmForces->intiDMForces(number, ndA, ndB, obA->getDeformationModel(), obB->getDeformationModel());
	
	m_objA = obA;
	m_objB = obB;
}

void MyInteraction::imposeInteractionForces(float k, float damp)
{
	m_dmForces->calcuateForces(k, damp);

	m_dmForces->getModelA()->SetExternalForce(m_dmForces->getForcesModelA());
	m_dmForces->getModelB()->SetExternalForce(m_dmForces->getForcesModelB());
}

void MyInteraction::disconnectSpring(bool st, float mul)
{
	if (st == false) return;

	// 伸展到 1.5 倍长度时，断开
	m_dmForces->disconnectSpring(mul);
}