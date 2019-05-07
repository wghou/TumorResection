// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// Drill is 
#ifndef KNIFE_H_
#define KNIFE_H_

#include"Object\RigidObject.h"


class InteractionTraction;
class PhantomDevice;


class Knife : public RigidObject
{
	friend class InteractionTraction;

public:
	Knife(char* fileName = "../assets/models/knife");
	~Knife();


	// ���沽�����
	virtual void timeStep(float time);


private:

	// �������ֱ�
	PhantomDevice *m_phDevice;
};

#endif // !INSTRUMENT_H_

