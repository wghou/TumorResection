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

	// ���� parentObjet �� col_obj ֮�����ת��ϵ�������¼�� recorder ��
	virtual bool computeCollision(GenericCollision* col_obj, CollisionRecorder* recorder);


	float* getXPtr() { return X_ptr; }
	const std::vector<uint16_t> & getXIndex() { return X_index; }
	float getRadius() { return radius; }

private:
	float* X_ptr = nullptr;		// x�������ַ
	std::vector<uint16_t> X_index;	// ��Ҫ������ײ���Ľڵ�ı��
	float radius = 0.03f;			// �ڵ�Ĵ�С

	// AABB
	float X_max[3];		// ��ΰ�Χ��
	float X_min[3];		// 

	// for render
	GLUquadric* m_quadric = nullptr;
};

#endif // !MYCOLLISION_H_

