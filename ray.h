#include "util.h"
#include "vector3D.h"
#include "model.h"
#include "polygon.h"
#include "sphere.h"
#ifndef RAY_H
#define RAY_H

//this class is not thread safe
using namespace std;


class Ray{   
public:
	Ray()= default;
	Ray(const Vector3D& point, const Vector3D& orient){
		start = point;
		direction = orient;
	}
	Ray(const Ray&) = default;
    Ray& operator=(const Ray&) = default;
	~Ray()= default;
	
	void printRay(ostream& out = cout) const;
	bool getFirstHit(const unique_ptr<Model>* models, size_t numModels, size_t counter);
	bool hitBlockPlane(const unique_ptr<Model>* models, size_t numModels, double max);
	
	bool hitPolygon(const Polygon* thisPolygon);
	bool hitCloserPolygon(const Polygon* thisPolygon);
	
	
	bool intersectPlane(const Polygon* thisPolygon);
	bool intersectCloserPlane(const Polygon* thisPolygon, double& tGet);	
	bool intersectBlockPlane(const Polygon* thisPolygon, double max);
	
	bool isInsidePolygon(const Polygon* thisPolygon) const;
	bool hitSphereNF(const Sphere& thisSphere) const;
	
	
	Vector3D start;
	Vector3D direction;
	Vector3D iPoint;
	double tValue = numeric_limits<double>::max();
	size_t model_index, polygon_index;
}; 

ostream& operator<<(ostream& out, const Ray& r);
bool hitSphere(const Sphere& thisSphere, const Vector3D& pixcelVector);
bool isInsidePolygonfcn(const Polygon* thisPolygon, const Vector3D& itsPt); //this does not change any t, iPoint in the class

class ColorRay: public Ray {
public:
	 
	ColorRay()= default;
	ColorRay(const Vector3D& point, const Vector3D& orient){
		start = point;
		direction = orient;
	}
	
	Vector3D color;
};

#endif
