#include "vector3D.h"
using namespace std;



void Vector3D::printVector(ostream& out) const{
	out << "[";
	for(double element: content){
		out << element << "," ;
	}
	out << "]" << flush;
} 

ostream& operator<<(ostream& out, const Vector3D& v){
	v.printVector(out);
	return out;
}

Vector3D Vector3D::operator+(const Vector3D & rhs) const{
	Vector3D result;
	for(size_t i = 0; i < 3; i++){
		result.content[i] = content[i] + rhs.content[i];
	}
	return result;	
}
Vector3D Vector3D::operator-(const Vector3D & rhs) const{
	Vector3D result;
	for(size_t i = 0; i < 3; i++){
		result.content[i] = content[i] - rhs.content[i];
	}
	return result;
}

Vector3D Vector3D::operator*(double scale) const{
	Vector3D result;
	for(size_t i = 0; i < 3; i++){
		result.content[i] = content[i] * scale;
	}
	return result;
}

Vector3D Vector3D::operator/(double scale) const{
	Vector3D result;
	for(size_t i = 0; i < 3; i++){
		result.content[i] = content[i] / scale;
	}
	return result;
}

const Vector3D & Vector3D::operator+=(const Vector3D & rhs ){
	for(size_t i = 0; i < 3; i++){
		content[i] += rhs.content[i];
	}
	return *this;
}
const Vector3D & Vector3D::operator-=(const Vector3D & rhs ){
	for(size_t i = 0; i < 3; i++){
		content[i] -= rhs.content[i];
	}
	return *this;
}
const Vector3D & Vector3D::operator*=(double scale){
	for(size_t i = 0; i < 3; i++){
		content[i] *= scale;
	}
	return *this;
}

const Vector3D & Vector3D::operator/=(double scale){
	for(size_t i = 0; i < 3; i++){
		content[i] /= scale;
	}
	return *this;
}

void Vector3D::normalize(){
	double norm = getNormSquared();
	norm =  sqrt(norm);
	content[0] /= norm;
	content[1] /= norm;
	content[2] /= norm;
}

double Vector3D::getNormSquared() const{
	return (pow(content[0],2) + pow(content[1],2) + pow(content[2],2));
}

Vector3D Vector3D::crossProduct(const Vector3D & rhs) const{
	Vector3D result;
	result.content[0] = content[1]* rhs.content[2] - content[2]* rhs.content[1];
	result.content[1] = content[2]* rhs.content[0] - content[0]* rhs.content[2];
	result.content[2] = content[0]* rhs.content[1] - content[1]* rhs.content[0];
	return result;
}

Vector3D crossProduct(const Vector3D & a, const Vector3D & b){
	return a.crossProduct(b);
}

double Vector3D::dotProduct(const Vector3D & rhs) const{
	return (content[0]*rhs.content[0] + content[1]*rhs.content[1] + content[2]*rhs.content[2]);
}

double dotProduct(const Vector3D & a, const Vector3D & b){
	return a.dotProduct(b);
}

Vector3D diagonalMultiply(const Vector3D & a, const Vector3D & b){
	Vector3D result;
	for(size_t i = 0; i < 3; i++){
		result.content[i] = a.content[i] * b.content[i];
	}
	return result;
}

Vector3D getVector(const Vector3D & from, const Vector3D & to){
	Vector3D result;
	for(size_t i = 0; i < 3; i++){
		result.content[i] = to.content[i] - from.content[i];
	}
	return result;
}

double getNormSquared(const Vector3D& v) {
	return (pow(v.content[0],2) + pow(v.content[1],2) + pow(v.content[2],2));
}

double getDistanceSquared(const Vector3D & a, const Vector3D & b){
	Vector3D v = getVector(a,b);
	return getNormSquared(v);
}
