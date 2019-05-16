// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 

#include"SimulationSceneC.h"

#include"Object\Object.h"


SimulationSceneC::SimulationSceneC()
{

}

SimulationSceneC::~SimulationSceneC()
{

}

void SimulationSceneC::cleanUp()
{
	// 1 - 交互前处理
	for (vector<InteractionBase*>::iterator it = m_interactions.begin(); it != m_interactions.end(); it++) {
		delete *it;
	}

	// 2 - 软组织形变 / 刚体位移
	for (vector<ObjectBase*>::iterator it = m_RgdObjects.begin(); it != m_RgdObjects.end(); it++) {
		delete *it;
	}

	for (vector<ObjectBase*>::iterator it = m_SftObjects.begin(); it != m_SftObjects.end(); it++) {
		delete *it;
	}
}

void SimulationSceneC::stepSimulation(float deltaTime)
{
	// 1 - 交互前处理
	for (vector<InteractionBase*>::const_iterator it = m_interactions.begin(); it != m_interactions.end(); it++) {
		(*it)->processBeforeTimeStep();
	}


	// 2 - 软组织形变 / 刚体位移
	for (vector<ObjectBase*>::const_iterator it = m_RgdObjects.begin(); it != m_RgdObjects.end(); it++) {
		(*it)->timeStep(deltaTime);
	}
	for (vector<ObjectBase*>::const_iterator it = m_SftObjects.begin(); it != m_SftObjects.end(); it++) {
		(*it)->timeStep(deltaTime);
	}


	// 3 - 交互后处理
	for (vector<InteractionBase*>::const_iterator it = m_interactions.begin(); it != m_interactions.end(); it++) {
		(*it)->processAfterTimeStep();
	}
}

void SimulationSceneC::post2RenderBuffer()
{
	for (vector<ObjectBase*>::const_iterator it = m_RgdObjects.begin(); it != m_RgdObjects.end(); it++) {
		(*it)->post2Render();
	}

	for (vector<ObjectBase*>::const_iterator it = m_SftObjects.begin(); it != m_SftObjects.end(); it++) {
		(*it)->post2Render();
	}
}