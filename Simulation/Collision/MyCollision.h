// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 
#ifndef MYCOLLISION_H_
#define MYCOLLISION_H_

#include"GenericCollision.h"

#include<vector>

class GLUquadric;

class MyCollision : public GenericCollision
{
public:
	MyCollision(ObjectBase* obj_self);
	~MyCollision();

	// 计算 parentObjet 与 col_obj 之间的彭转关系，结果记录在 recorder 中
	virtual bool computeCollision(GenericCollision* col_obj, CollisionRecorder* recorder);


	float* getXPtr() { return X_ptr; }
	const std::vector<uint16_t> & getXIndex() { return X_index; }
	float getRadius() { return radius; }

private:
	float* X_ptr = nullptr;		// x的坐标地址
	std::vector<uint16_t> X_index;	// 需要参与碰撞检测的节点的编号
	float radius = 0.03f;			// 节点的大小

	// AABB
	float X_max[3];		// 层次包围盒
	float X_min[3];		// 

	// for render
	GLUquadric* m_quadric = nullptr;
};

#endif // !MYCOLLISION_H_

