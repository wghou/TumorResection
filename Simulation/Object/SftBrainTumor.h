#ifndef SFT_BRAIN_TUMOR_H
#define SFT_BRAIN_TUMOR_H

#include"Object/ObjectBase.h"

class MyCollision;
class ElementLoader;
class DeformationModelGPU;

class SftBrainTumor : public ObjectBase
{
	friend MyCollision;

public:
	SftBrainTumor(std::string filePath);
	~SftBrainTumor();

	// 仿真步骤相关
	virtual bool createRenderableObject(RenderableObject* rdFactory, std::string objName);
	virtual void timeStep(float time);
	virtual void post2Render();


	// 碰撞检测相关
	virtual void collisionDetection(ObjectBase* obj_other, CollisionRecorder* recorder);
	void setMoreFixed(float target[], int index) {
		dir[0] = target[0]; dir[1] = target[1]; dir[2] = target[2]; more_fixed = index;
	}

	DeformationModelGPU* getDeformationModel() { return m_deformationModel; }

public:
	float dir[3] = { 0,0,0 };

protected:
	ElementLoader *m_loader_brain;
	ElementLoader *m_loader_tumor;
	DeformationModelGPU *m_deformationModel = nullptr;		// 形变模型对象
	int more_fixed = -1;
};

#endif // !SFT_BRAIN_TUMOR_H

