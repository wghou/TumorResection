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


	// 仿真步骤相关
	virtual void timeStep(float time);


	// 碰撞检测相关
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder);


private:
	OBJLoader *m_loader = nullptr;				// OBJ 模型
};

#endif // !INSTRUMENT_H_

