#include "sphere.h"
using namespace std;

ostream& operator<<(ostream& out, const Sphere& r){
	r.printSphere(out);
	return out;
}

void Sphere::printSphere(ostream& out) const{
	out << "Sphere: center at " << center << " with diameter^2 of " << diameterSq  << ", EO is " << EO << ", distanceSq to foccal point is " << distanceSq << endl;
}

void Sphere::setSphere(const Vector3D& ctr, double dia, const Vector3D& focal){
	center = ctr;
	diameterSq = pow(dia,2);
	EO = getVector(focal, center);
	distanceSq = EO.getNormSquared();
	RC = diameterSq - distanceSq;
}

void Sphere::setSphereSq(const Vector3D& ctr, double diaSq, const Vector3D& focal){
	center = ctr;
	diameterSq = diaSq;
	EO = getVector(focal, center);
	distanceSq = EO.getNormSquared();
	RC = diameterSq - distanceSq;
}
