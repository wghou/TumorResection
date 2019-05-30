// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// ObjectBase is the basic interface of a simulated object, such as rigid body, instrument,
// or soft object, such as brain tissue.
// It also contains functions for simulation steps, collision detection, and information about
// pos of object in the world coordinate system.
#ifndef OBJECTBASE_H_
#define OBJECTBASE_H_

#include<memory>
#include<vector>
#include<stdint.h>

#include"Math\MyMath.h"

class GenericCollision;
class MyCollision;
class CollisionRecorder;
class RenderableObject;
class SurfaceMesh;

class ObjectBase
{
	// ��Ԫ�����ʵ��ı��� / �����У�
	// 
	friend MyCollision;

public:
	ObjectBase() {};
	virtual ~ObjectBase() {};


	// ���沽�����
	virtual bool createRenderableObject(RenderableObject* rdFactory, std::string objName) { return true; };
	virtual void timeStep(float time) {};
	virtual void post2Render() {};


	// ��ײ������
	GenericCollision* getCollisionObject() { return m_collision; }
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder) {};

	SurfaceMesh* getMesh() { return m_mesh; }
	std::string getObjectName() { return m_objName; }

protected:
	bool initialized = false;	// �Ƿ��ʼ��
	std::string m_objName = "ObjectBase";
	SurfaceMesh* m_mesh = nullptr;				// ��������
	RenderableObject* m_rdFactory = nullptr;

	GenericCollision* m_collision = nullptr;	// ��ײ����
	bool isPerformCollisionDetection = true;	// �Ƿ������ײ���
};


#endif // !OBJECTBASE_H_

