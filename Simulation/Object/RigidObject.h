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
	RigidObject();
	~RigidObject();


	// 仿真步骤相关
	virtual bool createObjectFromFile(std::string filePath);
	virtual bool createRenderableObject(RenderableObject* rdFactory);
	virtual void timeStep(float time);
	virtual void post2Render();


	// 碰撞检测相关
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder);

protected:
	ObjLoader *m_loader;
};

#endif // !INSTRUMENT_H_

