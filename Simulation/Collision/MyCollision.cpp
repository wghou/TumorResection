// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// ���⣺���ܼ�������������ײ�����ˣ��ͻᵼ�� m_colRecorder ����

#include"MyCollision.h"

#include<typeinfo>
#include<algorithm>
#include<math.h>

#include"logger\include\Logger.h"
#include"Object\Drill.h"
#include"Object\SoftObjectGPU.h"
#include"Object/SurfaceMesh.h"

MyCollision::MyCollision(ObjectBase* obj_self)
{
	parentObject = obj_self;

	if (typeid(*obj_self) == typeid(Drill))
	{
		col_type = COL_POINT;
	}
	else if (typeid(*obj_self) == typeid(SoftObjectGPU))
	{
		col_type = COL_TRIANGLE;

		X_ptr = dynamic_cast<SoftObjectGPU*>(obj_self)->getMesh()->getVertices();
		
		X_index.clear();
		int triangleNum = dynamic_cast<SoftObjectGPU*>(obj_self)->getMesh()->getNumFaces();
		uint16_t *T = dynamic_cast<SoftObjectGPU*>(obj_self)->getMesh()->getFaces();
		for (int i = 0; i < triangleNum * 3; i++)
		{
			uint16_t v = T[i];
			std::vector<uint16_t>::iterator it;
			it = std::find(X_index.begin(), X_index.end(), v);
			if (it == X_index.end()) X_index.push_back(v);
		}
	}
	else if (typeid(*obj_self) == typeid(ObjectBase))
	{
		Logger::getMainLogger().log(Logger::Error, "Error when construct a new MyCollision object.", "MyCollision::MyCollision");
	}
	else {
		Logger::getMainLogger().log(Logger::Error, "Error when construct a new MyCollision object: unknown object type", "MyCollision::MyCollision");
	}

	radius = 0.03f;
}

MyCollision::~MyCollision()
{
	
}


bool MyCollision::computeCollision(GenericCollision* col_obj, CollisionRecorder* recorder)
{
	if (col_type == COL_POINT && col_obj->getColType() == COL_POINT)
	{
		// ���߾��� = ��ײ���� A ����λ�� - ��ײ���� B λ��
		double x = parentObject->getMesh()->getLocalPos().x - col_obj->getParentObject()->getMesh()->getLocalPos().x;
		double y = parentObject->getMesh()->getLocalPos().y - col_obj->getParentObject()->getMesh()->getLocalPos().y;
		double z = parentObject->getMesh()->getLocalPos().z - col_obj->getParentObject()->getMesh()->getLocalPos().z;

		double len = sqrt(x*x + y*y + z*z);

		// �ж��Ƿ�������ײ - �����֮��ľ���
		if (len < dynamic_cast<MyCollision*>(col_obj)->getRadius() + radius)
		{
			/***********************************************************/
			// ��¼��ײ���
			recorder->isCollided = true;

			// ���� A
			recorder->obj_self = this->parentObject;
			recorder->col_X_index_self = 0;
			recorder->col_X_self[0] = parentObject->getMesh()->getLocalPos().x;
			recorder->col_X_self[1] = parentObject->getMesh()->getLocalPos().y;
			recorder->col_X_self[2] = parentObject->getMesh()->getLocalPos().z;
			recorder->col_VN_self[0] = -x;
			recorder->col_VN_self[1] = -y;
			recorder->col_VN_self[2] = -z;

			// ��ײ���� B
			recorder->obj_1 = col_obj->getParentObject();
			recorder->col_X_index_1 = 0;
			recorder->col_X_1[0] = col_obj->getParentObject()->getMesh()->getLocalPos().x;
			recorder->col_X_1[1] = col_obj->getParentObject()->getMesh()->getLocalPos().y;
			recorder->col_X_1[2] = col_obj->getParentObject()->getMesh()->getLocalPos().z;
			recorder->col_VN_1[0] = x;
			recorder->col_VN_1[1] = y;
			recorder->col_VN_1[2] = z;

			// ��ײ��֮��ľ���
			recorder->length = len;

			return true;
		}
	}
	else if (col_type == COL_POINT && col_obj->getColType() == COL_TRIANGLE)
	{
		float* x_ptr = dynamic_cast<MyCollision*>(col_obj)->getXPtr();
		const std::vector<uint16_t> &x_index = dynamic_cast<MyCollision*>(col_obj)->getXIndex();
		float x_radius = dynamic_cast<MyCollision*>(col_obj)->getRadius();

		for (std::vector<uint16_t>::const_iterator it = x_index.begin(); it != x_index.end(); it++)
		{
			double x = parentObject->getMesh()->getLocalPos().x - x_ptr[(*it) * 3 + 0];
			double y = parentObject->getMesh()->getLocalPos().y - x_ptr[(*it) * 3 + 1];
			double z = parentObject->getMesh()->getLocalPos().z - x_ptr[(*it) * 3 + 2];

			double len = sqrt(x*x + y*y + z*z);

			if (len < x_radius + radius)
			{
				/***********************************************************/
				// ��¼��ײ��Ϣ
				recorder->isCollided = true;
				recorder->isProcessed = false;

				// ���� A
				recorder->obj_self = this->parentObject;
				recorder->col_X_index_self = 0;
				recorder->col_X_self[0] = parentObject->getMesh()->getLocalPos().x;
				recorder->col_X_self[1] = parentObject->getMesh()->getLocalPos().y;
				recorder->col_X_self[2] = parentObject->getMesh()->getLocalPos().z;
				recorder->col_VN_self[0] = -x;
				recorder->col_VN_self[1] = -y;
				recorder->col_VN_self[2] = -z;

				// ��ײ���� B
				recorder->obj_1 = col_obj->getParentObject();
				recorder->col_X_index_1 = (*it);
				recorder->col_X_1[0] = x_ptr[(*it) * 3 + 0];
				recorder->col_X_1[1] = x_ptr[(*it) * 3 + 1];
				recorder->col_X_1[2] = x_ptr[(*it) * 3 + 2];
				recorder->col_VN_1[0] = x;
				recorder->col_VN_1[1] = y;
				recorder->col_VN_1[2] = z;

				// ��ײ��֮��ľ���
				recorder->length = len;


				Logger::getMainLogger().log(Logger::Debug, "Detected collision..  vertex index: " + std::to_string(*it));
				return true;
			}
		}
	}
	else if (col_type == COL_TRIANGLE && col_obj->getColType() == COL_POINT)
	{
		return false;
	}
	else if (col_type == COL_TRIANGLE && col_obj->getColType() == COL_TRIANGLE)
	{
		return false;
	}
	else
	{
		return false;
	}

	return false;
}
