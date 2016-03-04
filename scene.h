#ifndef SCENE_H
#define SCENE_H
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <memory>
#include <vector>
#include <limits>
#include <time.h> 
#include "util.h"
#include "model.h"
#include "vector3D.h"
#include "ray.h"


using namespace std;

class LightSource{
public:
	LightSource(const Vector3D& rgb, const Vector3D& pos): RGB(rgb), position(pos){}
	Vector3D RGB, position;
	
};


class numGen{
public:
	numGen(){
		srand (time(NULL));
	}
	double getNumber(){
		return (double) rand() / (RAND_MAX) - 0.5;
	}
};

class Scene{   
public:
	
	Scene(size_t m): numModels(m){}
	//need copy contructor and equal operator
	~Scene(){	
		if(imagePlane != nullptr){
			for(size_t i = 0; i < rows; i++){
				delete imagePlane[i];
			}
			delete[] imagePlane;
		}
		if(models != nullptr){
			delete[] models;
		}
	}
	
	void setModels(unique_ptr<Model>* mptr){models = mptr;}
	void readInCamera(const string& fileName); //throws
	void readInMaterial(const string& fileName); //throws
	void createImagePlane();
	void rayTracing();
	Vector3D giveMeColor(Ray& ray, double product, size_t counter);
	void outputImage(const string& outputFile);
	
	void getPixelVec(Vector3D& pixcelVector, Vector3D& pixcelPoint, double r, double c);
	void getPixelVecRand(Vector3D& pixcelVector, Vector3D& pixcelPoint, double r, double c);
	void printCameraInfo();
	
	//string fileName;
	//string outputFile;
	Vector3D focalPoint, lookatPoint, vup;
	Vector3D nVec, vVec, uVec, dnTerm;
	double focalLength;
	int Umin,Umax,Vmin,Vmax;
	size_t numModels;
	size_t rows,cols;
	
	Vector3D** imagePlane = nullptr;
	unique_ptr<Model>* models = nullptr;
	
	vector<LightSource> lightSources;
	Vector3D ambient;
	numGen generator;
};

Vector3D getReflectionColor(const Vector3D& lightDirection, const Vector3D& surfaceNormal, const Polygon& poly, const LightSource& lightSrc, const Vector3D& viewingDirection);
Vector3D getReflectDir(const Vector3D& direction, const Vector3D& normal);


#endif 
