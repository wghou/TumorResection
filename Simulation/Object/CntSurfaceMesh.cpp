#include"CntSurfaceMesh.h"

#include"FilamentWinApp/RenderableObject.h"
#include"ObjLoader/TBN.h"


CntSurfaceMesh::CntSurfaceMesh(int numV, int numF, std::string meshName) : SurfaceMesh(numV, numF, meshName)
{

}


CntSurfaceMesh::~CntSurfaceMesh()
{
	if (m_Tets) delete m_Tets;
}


void CntSurfaceMesh::initSurfaceMesh(float* mVt, int numTet, int tetIdxOffset, uint16_t* mTet)
{
	m_TetIdxOffset = tetIdxOffset;
	memcpy(mVertices, mVt, sizeof(float)*numVertices * 3);
	m_numTets = numTet;
	m_Tets = new uint16_t[m_numTets * 4];
	memcpy(m_Tets, mTet, sizeof(uint16_t)*m_numTets * 4);

	// init faces
	for (int i = 0; i < m_numTets; i++) {
		mFaces[i * 12 + 0] = mTet[i * 4 + 0];
		mFaces[i * 12 + 1] = mTet[i * 4 + 1];
		mFaces[i * 12 + 2] = mTet[i * 4 + 2];

		mFaces[i * 12 + 3] = mTet[i * 4 + 0];
		mFaces[i * 12 + 4] = mTet[i * 4 + 2];
		mFaces[i * 12 + 5] = mTet[i * 4 + 3];

		mFaces[i * 12 + 6] = mTet[i * 4 + 0];
		mFaces[i * 12 + 7] = mTet[i * 4 + 3];
		mFaces[i * 12 + 8] = mTet[i * 4 + 1];

		mFaces[i * 12 + 9] = mTet[i * 4 + 1];
		mFaces[i * 12 + 10] = mTet[i * 4 + 3];
		mFaces[i * 12 + 11] = mTet[i * 4 + 2];
	}

	// update the TBN
	TBN::buildVns(numFaces, mFaces, numVertices, mVertices, mNormals);
	TBN::updateTBNs(numVertices, mNormals, mTBNs);
}


void CntSurfaceMesh::updateTetIndex(int srcVtNum, float*srcVtPtr)
{
	//m_srcVertNum = srcVtNum;
	//m_srcVertPtr = srcVtPtr;

	for (int i = 0; i < numVertices; i++) {

		for (int j = 0; j < srcVtNum; j++) {
			float len = (mVertices[i * 3 + 0] - srcVtPtr[j * 3 + 0])*(mVertices[i * 3 + 0] - srcVtPtr[j * 3 + 0])
				+ (mVertices[i * 3 + 1] - srcVtPtr[j * 3 + 1])*(mVertices[i * 3 + 1] - srcVtPtr[j * 3 + 1])
				+ (mVertices[i * 3 + 2] - srcVtPtr[j * 3 + 2])*(mVertices[i * 3 + 2] - srcVtPtr[j * 3 + 2]);

			if (len < 0.0001) {
				m_VertCpy[i]=j;

				break;
			}
		}
	}
	return;
}


void CntSurfaceMesh::rendering(RenderableObject* rdFactory)
{
	for each(auto cpy in m_VertCpy) {
		uint16_t fv_to = cpy.first;
		uint16_t fv_from = cpy.second;
		mVertices[fv_to * 3 + 0] = m_srcVertPtr[fv_from * 3 + 0];
		mVertices[fv_to * 3 + 1] = m_srcVertPtr[fv_from * 3 + 1];
		mVertices[fv_to * 3 + 2] = m_srcVertPtr[fv_from * 3 + 2];
	}

	// update the TBN
	TBN::buildVns(numFaces, mFaces, numVertices, mVertices, mNormals);
	TBN::updateTBNs(numVertices, mNormals, mTBNs);

	if (rdFactory == nullptr) return;
	// update the vertexBuffer
	rdFactory->updateVertexBuffer(m_meshName);
}