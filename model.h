#ifndef MODEL_H
#define MODEL_H
#include "matrix.h"
#include "polygon.h"
#include "vector3D.h"

using namespace std;
class Model{   
public:
	Model(){}
	Model(size_t v, size_t f, const Vector3D& foco): numVertices(v), numFaces(f), focalPoint(foco){
		vertices = new Vector3D[v];
		faces = new vector<unsigned int>[f];
		polygons = new Polygon[f];
	}
	//need copy contructor and equal operator
	~Model(){	
		delete[] vertices;
		delete[] faces;
		delete[] polygons;
	}
	
	void calculateMeanBounding();
	void calculateBoundingSphere();
	//void fillPolygonInformation();
	void multiply(const matrix_t transform);
	void printModelMatrix();
	void printModelInfo();
	
	Sphere bigSphere;
	size_t numVertices;
	size_t numFaces;
	//double meanVertex[3];
	Vector3D meanVertex;
	//double bound[6];//	bound[0] = minX;[1] = maxX;[2] = minY;[3] = maxY;[4] = minZ;[5] = maxZ;
	Vector3D minBound;
	Vector3D maxBound;
	Vector3D* vertices;
	vector<unsigned int>* faces;
	Polygon* polygons;
	Vector3D focalPoint;
};



#endif
