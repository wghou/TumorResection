// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// ��ײ����������� RigidObject �Լ� SoftObject �е�
// �������������˰�
#ifndef GENERICCOLLISION_H_
#define GENERICCOLLISION_H_

#include"CollisionBasic.h"

class ObjectBase;
class CollisionRecorder;

class GenericCollision
{
public:
	GenericCollision();
	virtual ~GenericCollision() {};

	// ��ײ��⣬���� parentObject �� col_obj ֮�����ײ��ϵ����������� recorder ��
	// ��ײ�� GenericCollision �����洢���ݣ���ײ��������е��������ݾ���Դ�� parentObject �Լ� col_obj
	virtual bool computeCollision(GenericCollision* col_obj, CollisionRecorder* recorder) { return false; }

	// ���� AABB ��ΰ�Χ�У���ʱû���õ�
	virtual void updateAABB() {};


	// ��ײ�� GenericCollision ���������ڵ� Object ����
	ObjectBase* getParentObject() { return parentObject; }

	// ��ײ�����������
	CollisionType getColType() { return col_type; }


protected:
	ObjectBase* parentObject = nullptr;		// �������� object ��

	CollisionType col_type;					// ��ײ���������
};


#endif // !GENERICCOLLISION_H_

