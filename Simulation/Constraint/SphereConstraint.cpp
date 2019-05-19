#include"Constraint/SphereConstraint.h"
#include"Object/SftBrain.h"
#include"Object/DfSurfaceMesh.h"
#include"DeformationModelGPU.h"

SphereConstraint::SphereConstraint(SftBrain* objA)
{
	m_objA = objA;
}


SphereConstraint::~SphereConstraint()
{

}


void SphereConstraint::processBeforeTimeStep() 
{
	// vertex
	//float *Xptr = dynamic_cast<SftBrain*>(m_objA)->getDeformationModel()->getX();

	//float *Fptr = dynamic_cast<SftBrain*>(m_objA)->getDeformationModel()->getExternalForce();
	//float p = 0.2;
	//for each (auto index in cstVertices)
	//{
	//	// ��ȡԼ��������
	//	int cvrIndex0 = Xptr[index * 3 + 0] * ratio;
	//	int cvrIndex1 = Xptr[index * 3 + 1] * ratio;

	//	float _x = Xptr[index * 3 + 0];
	//	if (_x < 0) continue;
	//	//Fptr[index * 3 + 0] = _x*_x*1050;
	//	//Xptr[index * 3 + 0] = 0;
	//	continue;


	//	// ����Լ�����ж��Ƿ���Ҫʩ��Լ��
	//	//if (cvr[(cvrIndex0 + 50) * 100 + (cvrIndex1 + 50)] == 0) continue;

	//	float x = Xptr[index * 3 + 0] - center[0];
	//	float y = Xptr[index * 3 + 1] - center[1];
	//	float z = Xptr[index * 3 + 2] - center[2];

	//	float len = sqrt(x * x + y * y + z * z);

	//	// ��Լ��������
	//	if (len < radius) continue;

	//	// ʩ��Լ��
	//	float mv_len = len - radius;

	//	//Xptr[index * 3 + 0] += x * mv_len / radius;
	//	//Xptr[index * 3 + 1] += y * mv_len / radius;
	//	//Xptr[index * 3 + 2] += z * mv_len / radius;

	//	//Fptr[index * 3 + 0] = x * mv_len / radius * p;
	//	//Fptr[index * 3 + 1] = y * mv_len / radius * p;
	//	//Fptr[index * 3 + 2] = z * mv_len / radius * p;
	//}
}


void SphereConstraint::processAfterTimeStep() 
{
	// vertex
	//float *Xptr = dynamic_cast<SftBrain*>(m_objA)->getDeformationModel()->getX();

	//float *Fptr = dynamic_cast<SftBrain*>(m_objA)->getDeformationModel()->getExternalForce();
	//float p = 2;

	//for each (auto index in cstVertices)
	//{
	//	// ��ȡԼ��������
	//	int cvrIndex0 = Xptr[index * 3 + 0] * ratio;
	//	int cvrIndex1 = Xptr[index * 3 + 1] * ratio;

	//	// ����Լ�����ж��Ƿ���Ҫʩ��Լ��
	//	if (cvr[(cvrIndex0 + 50) * 100 + (cvrIndex1 + 50)] == 0) continue;

	//	float x = Xptr[index * 3 + 0] - center[0];
	//	float y = Xptr[index * 3 + 1] - center[1];
	//	float z = Xptr[index * 3 + 2] - center[2];

	//	float len = sqrt(x * x + y * y + z * z);

	//	// ��Լ��������
	//	if (len < radius) continue;

	//	// ʩ��Լ��
	//	float mv_len = len - radius;

	//	//Xptr[index * 3 + 0] += x * mv_len / radius;
	//	//Xptr[index * 3 + 1] += y * mv_len / radius;
	//	//Xptr[index * 3 + 2] += z * mv_len / radius;

	//	//Fptr[index * 3 + 0] = x * mv_len / radius * p;
	//	//Fptr[index * 3 + 1] = y * mv_len / radius * p;
	//	//Fptr[index * 3 + 2] = z * mv_len / radius * p;
	//}
}