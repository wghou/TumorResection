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

#include"Math\MyMath.h"
#include<memory>
#include<vector>
#include<stdint.h>

class GenericCollision;
class MyCollision;
class CollisionRecorder;
class RenderableObject;

struct Mesh
{
	int numVertices = 0;
	int numFaces = 0;
	int numTets = 0;

	float* mVertices = nullptr;
	float* mNormals = nullptr;
	float* mTBNs = nullptr;
	float* mUV0 = nullptr;
	float* mUV1 = nullptr;

	uint16_t* mFaces = nullptr;
	uint16_t* mTets = nullptr;

	MyMatrix4f m_localOriant;		// 对象体的旋转矩阵
};


class ObjectBase
{
	// 友元：访问到的变量/函数有：
	// 
	friend MyCollision;

public:
	ObjectBase() {};
	virtual ~ObjectBase() {};


	// 仿真步骤相关
	virtual bool createRenderableObject(RenderableObject* rdFactory, std::string objName) { return true; };
	virtual void timeStep(float time) {};


	// 碰撞检测相关
	GenericCollision* getCollisionObject() { return m_collision; }
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder) {};

	Vector3f getLocalPos() { return Vector3f{ m_mesh.m_localOriant.m[3][0],m_mesh.m_localOriant.m[3][1], m_mesh.m_localOriant.m[3][2]}; }
	MyMatrix4f getLocalOriant() { return m_mesh.m_localOriant; }
	Mesh getMesh() { return m_mesh; }
	std::string getObjectName() { return m_objName; }

protected:
	bool initialized = false;	// 是否初始化
	std::string m_objName = "ObjectBase";
	Mesh m_mesh;				// 网格数据
	RenderableObject* m_rdFactory = nullptr;
	std::string m_mtlPath;		// 纹理存储路径

	GenericCollision* m_collision = nullptr;	// 碰撞对象
	bool isPerformCollisionDetection = true;	// 是否参与碰撞检测
};


#endif // !OBJECTBASE_H_

