#include "SurfaceMesh.h"

#include"FilamentWinApp/RenderableObject.h"
#include"ObjLoader/TBN.h"

#include"Logger/include/Logger.h"


SurfaceMesh::SurfaceMesh(int numV, int numF, std::string meshName)
{
	m_meshName = meshName;
	if (m_meshName == "") {
		Logger::getMainLogger().log(Logger::Warning, "The mesh name is null.", "");
	}

	numVertices = numV;
	numFaces = numF;

	if (numVertices <= 0 || numFaces <= 0) {
		Logger::getMainLogger().log(Logger::Level::Error, "There is no vertices in the meshLoader.", "SurfaceMesh::SurfaceMesh");
		return;
	}

	mVertices = new float[numVertices * 3];
	mNormals = new float[numVertices * 3];
	mTBNs = new float[numVertices * 4];
	//mUV0 = new float[numVertices * 2];
	//mUV1 = new float[numVertices * 2];

	mFaces = new uint16_t[numFaces * 3];
}


SurfaceMesh::~SurfaceMesh()
{
	if (mVertices) delete[] mVertices;
	if (mNormals) delete[] mNormals;
	if (mTBNs) delete[] mTBNs;
	if (mUV0) delete[] mUV0;
	if (mUV1) delete[] mUV1;

	if (mFaces) delete[] mFaces;
}

void SurfaceMesh::initSurfaceMesh(float* mVt, uint16_t* mFs, float* mU0, std::string mtlPahth)
{
	m_mtlPath = mtlPahth;

	if (numVertices <= 0 || numFaces <= 0) {
		return;
	}
	memcpy(mVertices, mVt, sizeof(float)*numVertices * 3);
	memcpy(mFaces, mFs, sizeof(uint16_t)*numFaces * 3);

	if (mU0 != nullptr) {
		mUV0 = new float[numVertices * 2];
		memcpy(mUV0, mU0, sizeof(float)*numVertices * 2);
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Info, "There is no UV0 in the meshLoader.");
	}

	TBN::buildVns(numFaces, mFaces, numVertices, mVertices, mNormals);
	TBN::updateTBNs(numVertices, mNormals, mTBNs);
}


bool SurfaceMesh::createRenderableObject(RenderableObject* rdFactory)
{
	if (rdFactory == nullptr) return false;

	// 导入纹理
	if (!m_mtlPath.empty()) {
		rdFactory->genMaterial(m_mtlPath);
	}
	else {
		Logger::getMainLogger().log(Logger::Level::Warning, "There is no material for object " + m_meshName, "SurfaceMesh::createRenderableObject");
	}

	// 生成渲染对象
	bool rlt = rdFactory->genRenderable(m_meshName, numVertices, mVertices,
		mTBNs, mUV0, numFaces, mFaces);
	return rlt;
}

void SurfaceMesh::rendering(RenderableObject* rdFactory)
{
	if (rdFactory == nullptr) return;

	rdFactory->updateObjectOriant(m_meshName, &m_localOriant.m[0][0]);
}
