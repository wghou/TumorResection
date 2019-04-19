// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 
#ifndef COLLISIONBASIC_H_
#define COLLISIONBASIC_H_

class ObjectBase;

// 碰撞对象的类型 - 未定义 - 点 - 三角形网格
enum CollisionType
{
	COL_NOT_DEFINED,	// undefined
	COL_POINT,			// 点碰撞
	COL_TRIANGLE		// 三角面片碰撞
};

enum ColProcessType
{
	COL_PROCESS_FOLLOW,	// 碰撞拉扯
	COL_PROCESS_PRESS,	// 碰撞按压
	COL_PROCESS_STOP	// 结束本次碰撞
};


// 碰撞结果记录
// 记录碰撞双方的信息
// Tips: 定义一个指针类型时，可以只预声明而不去定义，只要不用解引
class CollisionRecorder
{
public:
	bool isCollided = false;		// 是否发生了碰撞
	bool isProcessed = false;		// 是否完成了碰撞结果的处理

	ColProcessType processType;

	ObjectBase* obj_self = nullptr;	// 碰撞对象 - 自身
	int col_X_index_self = -1;		// 节点编号
	float col_X_self[3];			// 节点位置
	float col_VN_self[3];			// 节点法向量
	float col_Vel_self[3];			// 节点速度

	ObjectBase* obj_1 = nullptr;
	int col_X_index_1 = -1;
	float col_X_1[3];
	float col_VN_1[3];
	float col_Vel_1[3];

	float length;
};

#endif // !COLLISIONBASIC_H_

