#include "ray.h"
using namespace std;

ostream& operator<<(ostream& out, const Ray& r){
	r.printRay(out);
	return out;
}

void Ray::printRay(ostream& out) const{
	out << "Ray starts from " << start << " in the direction of " << direction<<", tValue:"<< tValue << endl;
}

bool Ray::intersectPlane(const Polygon* thisPolygon){
	if(thisPolygon->hasNoVolumn == true){
		return false;
	}
	//inspect Norm[dot]PixelVector, 
	double Nu = dotProduct(thisPolygon->surfaceNormal, direction);
	if(Nu == 0){
		return false;// dot==0, ray is parallel to this polygon
	}
	//t = (- d - N.L )/ N.u
	double t = ( (-1) * thisPolygon->dTerm - dotProduct(thisPolygon->surfaceNormal, start)) / Nu;
	if(t < 0){
		return false; // ray hit polygon behind the camera...
	}
	//tValue = t; not needed to keep track of t in this case
	iPoint = start + direction*t;
	return true;
}

bool Ray::intersectCloserPlane(const Polygon* thisPolygon, double& tGet){
	if(thisPolygon->hasNoVolumn == true){
		return false;
	}
	//inspect Norm[dot]PixelVector, 
	double Nu = dotProduct(thisPolygon->surfaceNormal, direction);
	if(Nu == 0){
		return false;// dot==0, ray is parallel to this polygon
	}
	//t = (- d - N.L )/ N.u
	double t = ( (-1) * thisPolygon->dTerm - dotProduct(thisPolygon->surfaceNormal, start)) / Nu;
	//cout << "t: " << t <<endl;
	if(t < 0.000001){
		return false; // ray hit polygon behind the camera...
	}
	if(t >= tValue){ //this is not the first plane this ray hit
		return false;  //it does not intersect a closer plane
	}
	tGet = t;   //store t somewhere and see if it aculally is inside the polygon
	//iPoint = start + direction*t;
	return true;	
}


bool Ray::hitCloserPolygon(const Polygon* thisPolygon){
	//if(!hitSphere(thisPolygon->boundingSphere, direction)){
		//cout << "skipped" << endl;
	//	return false;
	//}
	double currentT;
	if(!intersectCloserPlane(thisPolygon, currentT)){
		//cout << "does not intersectCloserPlane" << endl;
		return false;
	}
	
	Vector3D itsPt = start + direction*currentT;
	if(!isInsidePolygonfcn(thisPolygon, itsPt)){
		//cout << "is not inside polygon" << endl;
		return false;
	}
	//now we can safely update t, since it's smaller and is inside
	tValue = currentT;
	iPoint = itsPt;
	return true;
}

bool Ray::getFirstHit(const unique_ptr<Model>* models, size_t numModels, size_t counter){
	tValue = numeric_limits<double>::max();
	bool intersect = false;
	for(size_t modelIter = 0;  modelIter < numModels; modelIter++){	
		//hit sphere test
		if(counter == 1){
			if(!hitSphere(models[modelIter]->bigSphere, direction)){
				//cout << "model skipped"<< endl;
				continue;
			}
		}else{
			if(!hitSphereNF(models[modelIter]->bigSphere)){
				continue;
			}
		}
		
		// hit the big sphere, loop through each polygon
		for(size_t polyIter = 0; polyIter < models[modelIter]->numFaces; polyIter++){			
			Polygon* thisPolygon = &(models[modelIter]->polygons[polyIter]);	
			if(hitCloserPolygon(thisPolygon)){
				intersect = true;
				//record which polygon this ray first hit
				model_index = modelIter;
				polygon_index = polyIter;
				//cout << "update to: " << polygon_index << endl;
			}
		}
	}
	return intersect;
}

//------------------------------blocking---------------------------------------------
bool Ray::intersectBlockPlane(const Polygon* thisPolygon, double max){
	if(thisPolygon->hasNoVolumn == true){
		return false;
	}
	//inspect Norm[dot]PixelVector, 
	double Nu = dotProduct(thisPolygon->surfaceNormal, direction);
	if(Nu == 0){
		return false;// dot==0, ray is parallel to this polygon
	}
	//t = (- d - N.L )/ N.u
	double t = ( (-1) * thisPolygon->dTerm - dotProduct(thisPolygon->surfaceNormal, start)) / Nu;
	if(t < 0.00001){
		return false; // ray hit polygon behind this polygon...
	}
	if(t > max){ //ray hit behind the light src
		return false;  //it does not intersect a closer plane
	}
	tValue = t;   
	iPoint = start + direction*t;
	return true;	
}

//once you find one blocking polygon, break.
bool Ray::hitBlockPlane(const unique_ptr<Model>* models, size_t numModels, double max){
	bool intersect = false;
	for(size_t modelIter = 0;  modelIter < numModels; modelIter++){	
		if(!hitSphereNF(models[modelIter]->bigSphere)){ 
			continue;
		}
		for(size_t polyIter = 0; polyIter < models[modelIter]->numFaces; polyIter++){			
			Polygon* thisPolygon = &(models[modelIter]->polygons[polyIter]);	
			if(!intersectBlockPlane(thisPolygon, max)){
				continue;
			}
			if(!isInsidePolygon(thisPolygon)){
				continue;
			}
			intersect = true;
			break;
		}
		
		if(intersect) break;
	}
	return intersect;
}

//-------------------------------------------end blocking-----------------------------------------------------

//------------below:----currently not using--------------------------------
bool Ray::isInsidePolygon(const Polygon* thisPolygon) const{	
	for(size_t i = 0; i < thisPolygon->numEdges; i++){
		Vector3D ep = getVector(thisPolygon->vertices[i], iPoint);
		Vector3D Np = crossProduct(thisPolygon->edges[i], ep);
		if(dotProduct(thisPolygon->surfaceNormal,Np) < -0.000000001){
			return false;
		}
	}
	return true;
}

bool isInsidePolygonfcn(const Polygon* thisPolygon, const Vector3D& itsPt){	
	for(size_t i = 0; i < thisPolygon->numEdges; i++){
		Vector3D ep = getVector(thisPolygon->vertices[i], itsPt);
		Vector3D Np = crossProduct(thisPolygon->edges[i], ep);
		if(dotProduct(thisPolygon->surfaceNormal,Np) < -0.000000001){
			return false;
		}
	}
	return true;
}


//true if hit this polygon
bool Ray::hitPolygon(const Polygon* thisPolygon){
	if(!hitSphere(thisPolygon->boundingSphere, direction)){
		cout << "skipped" << endl;
		cout << thisPolygon->boundingSphere << endl;
		return false;
	}
		
	if(!intersectPlane(thisPolygon)){
		cout << "not intersectPlane" << endl;
		return false;
	}
	if(!isInsidePolygon(thisPolygon))
		return false;
	return true;
}


bool hitSphere(const Sphere& thisSphere, const Vector3D& pixcelVector){
	double v = thisSphere.EO.dotProduct(pixcelVector);
	//d^2 = r^2 - c^2 + v^2
	double dSq = thisSphere.RC + v * v;
	//cout << "dsq" << dSq << endl;
	if(dSq < -0.00000001){
		return false;
	}
	return true;
}

bool Ray::hitSphereNF(const Sphere& thisSphere) const{
	Vector3D EO = getVector(start, thisSphere.center);
	double v = EO.dotProduct(direction);
	//double distanceSq = EO.getNormSquared(); //c^2
	
	//d^2 = r^2 - c^2 + v^2
	double dSq = thisSphere.diameterSq - EO.getNormSquared() + v * v;
	//cout << "dsq" << dSq << endl;
	if(dSq < -0.00000001){
		return false;
	}
	return true;
}






