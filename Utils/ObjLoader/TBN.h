#ifndef TBN_H
#define TBN_H
#include<vector>

using namespace std;
class TBN
{
public:
	static bool buildVns(int numFaces, uint16_t* faces, int numVertices, float* vertices, float* vn);
	static bool updateTBNs(int numVert, float* vn, float* tbn);
};
#endif // !TBN_H



