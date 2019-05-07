// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// RigidObject is 
#ifndef RIGIDOBJECT_H_
#define RIGIDOBJECT_H_

#include"Object\ObjectBase.h"
#include<string>


class SurfaceMeshBase;

class RigidObject : public ObjectBase
{

public:
	RigidObject(std::string filePath);
	~RigidObject();


	// ���沽�����
	virtual bool createRenderableObject(RenderableObject* rdFactory, std::string objName);
	virtual void timeStep(float time);


	// ��ײ������
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder);

};

#endif // !INSTRUMENT_H_

