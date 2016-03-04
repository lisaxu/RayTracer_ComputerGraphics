#include "util.h"
#ifndef VECTOR3D_H
#define VECTOR3D_H

using namespace std;

 class Vector3D{   
public:
	Vector3D(){
		content[0] = 0;
		content[1] = 0;
		content[2] = 0;
	}
	Vector3D(double x, double y, double z){
		content[0] = x;
		content[1] = y;
		content[2] = z;
	}
	Vector3D(const Vector3D&) = default;
    Vector3D& operator=(const Vector3D&) = default;
	~Vector3D()= default;
	
	//arithmic operator
	void printVector(ostream& out = cout) const;
	Vector3D operator+(const Vector3D & rhs) const;
	Vector3D operator-(const Vector3D & rhs) const;
	Vector3D operator*(double scale) const;
	Vector3D operator/(double scale) const;
	const Vector3D & operator+=(const Vector3D & rhs );
	const Vector3D & operator-=(const Vector3D & rhs );
	const Vector3D & operator*=(double scale);
	const Vector3D & operator/=(double scale);
	
	//common vector operations
	void normalize();
	double getNormSquared() const;
	Vector3D crossProduct(const Vector3D & rhs) const;
	double dotProduct(const Vector3D & rhs) const;
	
	//index operator
	double& operator[] (int index){return content[index];}
	const double& operator[](int index) const{return content[index];}
	
	vec content;
}; 

Vector3D getVector(const Vector3D & from, const Vector3D & to);
Vector3D crossProduct(const Vector3D & a, const Vector3D & b);
Vector3D diagonalMultiply(const Vector3D & a, const Vector3D & b);
double dotProduct(const Vector3D & a, const Vector3D & b);
double getNormSquared(const Vector3D& v);
double getDistanceSquared(const Vector3D & a, const Vector3D & b);
ostream& operator<<(ostream& out, const Vector3D& v);
#endif
