// Copyright 2018 BJTU.
// License: Private
// Author: Wenguo Hou
// This is a framework for neurosurgical sululation.
// 
// 问题：近能检测两个物体的碰撞，多了，就会导致 m_colRecorder 覆盖

#include"MyCollision.h"

#include<typeinfo>
#include<algorithm>
#include<math.h>

#include"logger\include\Logger.h"
#include"Object\Drill.h"
#include"Object/Object.h"
#include"Object\SoftObjectGPU.h"
#include"Object\SurfaceMesh.h"
#include"Object/DfSurfaceMesh.h"
#include"Object/CntSurfaceMesh.h"
#include"DeformationModelGPU.h"

MyCollision::MyCollision(ObjectBase* obj_self)
{
	parentObject = obj_self;

	radius = 0.03f;
}

MyCollision::~MyCollision()
{
	
}


bool MyCollision::computeCollision(GenericCollision* col_obj, CollisionRecorder* recorder)
{
	if (typeid(*parentObject) == typeid(Drill) && 
		(typeid(*col_obj->getParentObject()) == typeid(SoftObjectGPU) || (typeid(*col_obj->getParentObject()) == typeid(SftBrainTumor)))){

		std::vector<SurfaceMesh*>& m_mesh = col_obj->getParentObject()->getMesh();

		for each(auto mesh in m_mesh) {

			// do not perform collision detection on CntSurfaceMesh
			if (typeid(*mesh) == typeid(CntSurfaceMesh)) {
				continue;
			}

			float* x_ptr = mesh->getVertices();
			uint16_t index_offset = 0;
			if (typeid(*mesh) == typeid(DfSurfaceMesh)) {
				index_offset = dynamic_cast<DfSurfaceMesh*>(mesh)->getIndexOffset();
			}
			uint16_t* face_ptr = mesh->getFaces();
			float x_radius = dynamic_cast<MyCollision*>(col_obj)->getRadius();

			for (int i = 0; i < mesh->getNumFaces() * 3; i++) {
				uint16_t index = face_ptr[i];

				double x = parentObject->getLocalPos().x - x_ptr[index * 3 + 0];
				double y = parentObject->getLocalPos().y - x_ptr[index * 3 + 1];
				double z = parentObject->getLocalPos().z - x_ptr[index * 3 + 2];

				double len = sqrt(x*x + y * y + z * z);

				if (len < x_radius + radius)
				{
					/***********************************************************/
					// 记录碰撞信息
					recorder->isCollided = true;
					recorder->isProcessed = false;

					// 自身 A
					recorder->obj_self = this->parentObject;
					recorder->col_X_index_self = 0;
					recorder->col_X_self[0] = parentObject->getLocalPos().x;
					recorder->col_X_self[1] = parentObject->getLocalPos().y;
					recorder->col_X_self[2] = parentObject->getLocalPos().z;
					recorder->col_VN_self[0] = -x;
					recorder->col_VN_self[1] = -y;
					recorder->col_VN_self[2] = -z;

					// 碰撞对象 B
					recorder->obj_1 = col_obj->getParentObject();
					recorder->col_X_index_1 = index + index_offset;
					recorder->col_X_1[0] = x_ptr[index * 3 + 0];
					recorder->col_X_1[1] = x_ptr[index * 3 + 1];
					recorder->col_X_1[2] = x_ptr[index * 3 + 2];
					recorder->col_VN_1[0] = x;
					recorder->col_VN_1[1] = y;
					recorder->col_VN_1[2] = z;

					// 碰撞点之间的距离
					recorder->length = len;


					Logger::getMainLogger().log(Logger::Debug, "Detected collision..  vertex index: " + std::to_string(index));
					return true;
				}
			}
		}
	}
	else if (typeid(*parentObject) == typeid(Knife) && (typeid(*col_obj->getParentObject()) == typeid(SftBrainTumor))) {
		std::vector<SurfaceMesh*>& m_mesh = col_obj->getParentObject()->getMesh();

		for each(auto mesh in m_mesh) {

			// do not perform collision detection on CntSurfaceMesh
			if (typeid(*mesh) != typeid(CntSurfaceMesh)) {
				continue;
			}

			float* x_ptr = dynamic_cast<CntSurfaceMesh*>(mesh)->getSrcVertPtr();
			int tetNum = dynamic_cast<CntSurfaceMesh*>(mesh)->getTetNum();
			uint16_t* tet_ptr = dynamic_cast<CntSurfaceMesh*>(mesh)->getTets();
			uint16_t* rmTets = dynamic_cast<CntSurfaceMesh*>(mesh)->getRmTets();
			std::map<uint16_t, uint16_t> &vt_cpy = dynamic_cast<CntSurfaceMesh*>(mesh)->getIndexCpyMap();

			for (int i = 0; i < tetNum; i++) {

				// the tet has been removed
				if (rmTets[i] != 0) continue;


				uint16_t p0 = vt_cpy[tet_ptr[i * 4 + 0]];
				uint16_t p1 = vt_cpy[tet_ptr[i * 4 + 1]];
				uint16_t p2 = vt_cpy[tet_ptr[i * 4 + 2]];
				uint16_t p3 = vt_cpy[tet_ptr[i * 4 + 3]];

				float avg_x = (x_ptr[p0 * 3 + 0] + x_ptr[p1 * 3 + 0] + x_ptr[p2 * 3 + 0] + x_ptr[p3 * 3 + 0]) / 4;
				float avg_y = (x_ptr[p0 * 3 + 1] + x_ptr[p1 * 3 + 1] + x_ptr[p2 * 3 + 1] + x_ptr[p3 * 3 + 1]) / 4;
				float avg_z = (x_ptr[p0 * 3 + 2] + x_ptr[p1 * 3 + 2] + x_ptr[p2 * 3 + 2] + x_ptr[p3 * 3 + 2]) / 4;

				float len = (parentObject->getLocalPos().x - avg_x) * (parentObject->getLocalPos().x - avg_x) +
					(parentObject->getLocalPos().y - avg_y) * (parentObject->getLocalPos().y - avg_y) +
					(parentObject->getLocalPos().z - avg_z) * (parentObject->getLocalPos().z - avg_z);

				if (len < 0.05) {
					recorder->col_X_index_1 = i;
					//rmTets[i] = 1;
					//dynamic_cast<SftBrainTumor*>(col_obj->getParentObject())->getDeformationModel()->removeTet(i + tetIdxOffset);

					Logger::getMainLogger().log(Logger::Debug, "Detected collision.  tetrahedron index: " + std::to_string(i));
					return true;
				}
			}
		}
	}
	else {
		Logger::getMainLogger().log(Logger::Error, "Uknown typeid of object when perform collision");
	}

	return false;
}
