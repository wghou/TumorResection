// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 
#ifndef SIMULATIONSCENEC_H_
#define SIMULATIONSCENEC_H_


class ObjectBase;
class InteractionBase;

#include<vector>
using namespace std;

class SimulationSceneC
{
public:
	SimulationSceneC();
	~SimulationSceneC();

	void stepSimulation(float deltaTime);	// 仿真步

	void addSoftObject(ObjectBase* object) { m_SftObjects.push_back(object); }
	void addRigidObject(ObjectBase* object) { m_RgdObjects.push_back(object); }
	void addInteractions(InteractionBase* interaction) { m_interactions.push_back(interaction); }
	void cleanUp();

private:
	vector<ObjectBase*> m_SftObjects;			// 软体对象
	vector<ObjectBase*> m_RgdObjects;			// 刚体对象
	vector<InteractionBase*> m_interactions;	// 不同物体对象之间的交互
};


#endif