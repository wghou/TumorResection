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

	void addObject(ObjectBase* object) { m_Objects.push_back(object); }
	void addInteractions(InteractionBase* interaction) { m_interactions.push_back(interaction); }

private:
	vector<ObjectBase*> m_Objects;				// 所有对象，软体/刚体，每个对象包含了各自的 render
	vector<InteractionBase*> m_interactions;	// 不同物体对象之间的交互
};


#endif