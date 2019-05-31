// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Drill is 
#ifndef DRILL_H_
#define DRILL_H_

#include"Object\RigidObject.h"

class InteractionTraction;
class PhantomDevice;


class Drill : public RigidObject
{
	friend class InteractionTraction;

public:
	Drill(std::string fileName = "../assets/models/drill/");
	~Drill();


	// ���沽�����
	virtual bool createRenderableObject(RenderableObject* rdFactory, std::string objName = "Drill");
	virtual void timeStep(float time);


private:

	// �������ֱ�
	PhantomDevice *m_phDevice;
};

#endif // !INSTRUMENT_H_

