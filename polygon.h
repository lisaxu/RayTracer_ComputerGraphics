#ifndef POLYGON_H
#define POLYGON_H
#include "util.h"
#include "vector3D.h"
#include "sphere.h"

using namespace std;
class Polygon{   
public:
	Polygon(){}
	void initialize(size_t numVer){
		numVertices = numVer;
		numEdges = numVer;
		edges = new Vector3D[numEdges];
		vertices = new Vector3D[numVertices];
		Kd = Vector3D(0.5,0.5,0.5);
	}
	~Polygon(){	
		if(edges != nullptr){
			delete[] edges;
			delete[] vertices;
		}
	}
	
	size_t numVertices;
	size_t numEdges;
	
	void printPolygon(ostream& out) const;
	
	Sphere boundingSphere;
	Vector3D surfaceNormal; //a,b,c
	bool hasNoVolumn = true;
	double dTerm; //as in ax+by+cz+d=0 to represent the plane
	Vector3D* edges = nullptr; //array of vectors(edges)
	Vector3D* vertices = nullptr;//array of nodes
	
	//material property
	Vector3D Kd; //three by three diagnonal matrix, diagonals are Kr, Kg, Kb
	double Ks = 0; //how specular it is, %light reflected, mirror = 1
	double alpha = 1; //-1 to mark that this polygon is not assigned a property
						//phong constant, how shiny the object is, mirror = 200
	double translucency = 0;//percent translucency, opaque=0, window=1
};

ostream& operator<<(ostream& out, const Polygon& p);

#endif
