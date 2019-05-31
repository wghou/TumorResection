#include "DfSurfaceMesh.h"

#include"FilamentWinApp/RenderableObject.h"
#include"ObjLoader/TBN.h"

DfSurfaceMesh::DfSurfaceMesh(int numV, int numF, std::string meshName) : SurfaceMesh(numV, numF, meshName)
{

}


DfSurfaceMesh::~DfSurfaceMesh()
{
}

void DfSurfaceMesh::rendering(RenderableObject* rdFactory)
{
	// copy the vertex coordinate
	memcpy(mVertices, m_srcVertPtr + mVertIdxOffset * 3, sizeof(float)*m_srcVertNum * 3);
	for (int i = 0; i < m_VertCpy.size(); i = i + 2) {
		uint16_t newIndex = m_VertCpy[i];
		uint16_t fv_v = m_VertCpy[i + 1];
		mVertices[newIndex * 3 + 0] = m_srcVertPtr[(fv_v + mVertIdxOffset) * 3 + 0];
		mVertices[newIndex * 3 + 1] = m_srcVertPtr[(fv_v + mVertIdxOffset) * 3 + 1];
		mVertices[newIndex * 3 + 2] = m_srcVertPtr[(fv_v + mVertIdxOffset) * 3 + 2];
	}

	// update the TBN
	TBN::buildVns(numFaces, mFaces, numVertices, mVertices, mNormals);
	TBN::updateTBNs(numVertices, mNormals, mTBNs);

	if (rdFactory == nullptr) return;
	// update the vertexBuffer
	rdFactory->updateVertexBuffer(m_meshName);
}

void DfSurfaceMesh::setVertCpys(int srcVtNum, int offset, float* srcVtPtr, std::vector<uint16_t> &cpy)
{
	m_srcVertNum = srcVtNum;
	m_srcVertPtr = srcVtPtr;
	mVertIdxOffset = offset;
	m_VertCpy.assign(cpy.begin(), cpy.end());
}
