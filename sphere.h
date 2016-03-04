#include "util.h"
#include "vector3D.h"

#ifndef SPHERE_H
#define SPHERE_H


using namespace std;

class Sphere{   
public:
	Sphere()= default;
	Sphere(const Vector3D& ctr, double dia, const Vector3D& focal){
		setSphere(ctr, dia, focal);
	}
	
	Sphere(const Sphere&) = default;
    Sphere& operator=(const Sphere&) = default;
	~Sphere()= default;
	
	void printSphere(ostream& out = cout) const;
	void setSphere(const Vector3D& ctr, double dia, const Vector3D& focal);
	void setSphereSq(const Vector3D& ctr, double diaSq, const Vector3D& focal);
	
	Vector3D center; //just in case, no need of it in calculation
	Vector3D EO; //vector from focal point (E) to center
	double diameterSq; //r^2
	double distanceSq; //norm of EO, c^2
	double RC; //r^2 - c^2
	
	//point of intersection is Q = E + (v-d)U (smallest positive d)
}; 

ostream& operator<<(ostream& out, const Sphere& r);

#endif

