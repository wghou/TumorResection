// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 
#ifndef COLLISIONBASIC_H_
#define COLLISIONBASIC_H_

class ObjectBase;

// ��ײ��������� - δ���� - �� - ����������
enum CollisionType
{
	COL_NOT_DEFINED,	// undefined
	COL_POINT,			// ����ײ
	COL_TRIANGLE		// ������Ƭ��ײ
};

enum ColProcessType
{
	COL_PROCESS_FOLLOW,	// ��ײ����
	COL_PROCESS_PRESS,	// ��ײ��ѹ
	COL_PROCESS_STOP	// ����������ײ
};


// ��ײ�����¼
// ��¼��ײ˫������Ϣ
// Tips: ����һ��ָ������ʱ������ֻԤ��������ȥ���壬ֻҪ���ý���
class CollisionRecorder
{
public:
	bool isCollided = false;		// �Ƿ�������ײ
	bool isProcessed = false;		// �Ƿ��������ײ����Ĵ���

	ColProcessType processType;

	ObjectBase* obj_self = nullptr;	// ��ײ���� - ����
	int col_X_index_self = -1;		// �ڵ���
	float col_X_self[3];			// �ڵ�λ��
	float col_VN_self[3];			// �ڵ㷨����
	float col_Vel_self[3];			// �ڵ��ٶ�

	ObjectBase* obj_1 = nullptr;
	int col_X_index_1 = -1;
	float col_X_1[3];
	float col_VN_1[3];
	float col_Vel_1[3];

	float length;
};

#endif // !COLLISIONBASIC_H_

