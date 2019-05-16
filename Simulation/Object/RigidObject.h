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

class ObjLoader;
class SurfaceMeshBase;

class RigidObject : public ObjectBase
{

public:
	RigidObject(std::string filePath);
	~RigidObject();


	// 仿真步骤相关
	virtual bool createRenderableObject(RenderableObject* rdFactory, std::string objName);
	virtual void timeStep(float time);
	virtual void post2Render();


	// 碰撞检测相关
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder);

protected:
	ObjLoader *m_loader;
};

#endif // !INSTRUMENT_H_

