#ifndef SURFACE_MESH_H
#define SURFACE_MESH_H

#include"Math\MyMath.h"
#include<memory>
#include<vector>
#include<stdint.h>

class RenderableObject;

class SurfaceMesh
{
public:
	SurfaceMesh(int numV, int numF, std::string meshName);
	~SurfaceMesh();

	void initSurfaceMesh(float* mVt, uint16_t* mFs, float* mU0, std::string mtlPahth);

	bool createRenderableObject(RenderableObject* rdFactory);
	virtual void rendering(RenderableObject* rdFactory);
	
	Vector3f getLocalPos() { return Vector3f{m_localOriant.m[3][0], m_localOriant.m[3][1], m_localOriant.m[3][2]}; }
	MyMatrix4f getOriant() { return m_localOriant; }
	void setOriant(MyMatrix4f ori) { m_localOriant = ori; }

	int getNumVertices() { return numVertices; }
	float* getVertices() { return mVertices; }
	int getNumFaces() { return numFaces; }
	uint16_t* getFaces() { return mFaces; }

protected:
	int numVertices = 0;
	int numFaces = 0;

	float* mVertices = nullptr;
	float* mNormals = nullptr;
	float* mTBNs = nullptr;
	float* mUV0 = nullptr;
	float* mUV1 = nullptr;

	uint16_t* mFaces = nullptr;

	MyMatrix4f m_localOriant;		// 对象体的旋转矩阵

	std::string m_meshName = "SurfaceMesh";
	std::string m_mtlPath;			// 纹理存储路径
};
#endif // !SURFACE_MESH_H