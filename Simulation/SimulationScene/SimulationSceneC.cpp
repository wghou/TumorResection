// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 

#include"SimulationSceneC.h"

#include"Object\Object.h"
#include"Collision\MyCollision.h"


SimulationSceneC::SimulationSceneC()
{

}

SimulationSceneC::~SimulationSceneC()
{

}

void SimulationSceneC::stepSimulation(float deltaTime)
{
	// 1 - ����ǰ����
	for (vector<InteractionBase*>::iterator it = m_interactions.begin(); it != m_interactions.end(); it++)
	{
		(*it)->processBeforeTimeStep();
	}


	// 2 - ����֯�α� / ����λ��
	for (vector<ObjectBase*>::iterator it = m_Objects.begin(); it != m_Objects.end(); it++)
	{
		(*it)->timeStep(deltaTime);
	}


	// 3 - ��������
	for (vector<InteractionBase*>::iterator it = m_interactions.begin(); it != m_interactions.end(); it++)
	{
		(*it)->processAfterTimeStep();
	}
}