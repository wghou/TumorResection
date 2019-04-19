// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 碰撞检测是依附于 RigidObject 以及 SoftObject 中的
// 自身不保留数据了吧
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

	// 碰撞检测，计算 parentObject 与 col_obj 之间的碰撞关系，结果保存在 recorder 中
	// 碰撞类 GenericCollision 本身不存储数据，碰撞计算过程中的所有数据均来源于 parentObject 以及 col_obj
	virtual bool computeCollision(GenericCollision* col_obj, CollisionRecorder* recorder) { return false; }

	// 更新 AABB 层次包围盒，暂时没有用到
	virtual void updateAABB() {};


	// 碰撞类 GenericCollision 类所从属于的 Object 对象
	ObjectBase* getParentObject() { return parentObject; }

	// 碰撞检测对象的类型
	CollisionType getColType() { return col_type; }


protected:
	ObjectBase* parentObject = nullptr;		// 所从属的 object 类

	CollisionType col_type;					// 碰撞对象的类型
};


#endif // !GENERICCOLLISION_H_

