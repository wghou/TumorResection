// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// RigidObject is 
#ifndef RIGIDOBJECT_H_
#define RIGIDOBJECT_H_

#include"Object\ObjectBase.h"


class OBJLoader;
class SurfaceMeshBase;

class RigidObject : public ObjectBase
{

public:
	RigidObject(char* fileName);
	~RigidObject();


	// ���沽�����
	virtual void timeStep(float time);


	// ��ײ������
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder);


private:
	OBJLoader *m_loader = nullptr;				// OBJ ģ��
};

#endif // !INSTRUMENT_H_

