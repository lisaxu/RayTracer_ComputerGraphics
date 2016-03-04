#include "util.h"
using namespace std;

void checkFailure(istream& istr, const string& msg){
	if(istr.fail()){
		if(istr.eof()){
			throw ios_base::failure("Input stream failed when extracting " + msg + ", EOF reached");
		}
		else{
			throw ios_base::failure("Input stream failed when extracting " + msg);
		}
	}
}

double getNormSquared(const vector_t a){
	return (pow(a[0],2) + pow(a[1],2) + pow(a[2],2));
}
void normalize(vector_t a){
	//cout << "before nornalize: " << x<< ", " << y << ", " << z << endl;
	double norm = getNormSquared(a);
	norm =  sqrt(norm);
	a[0] /= norm;
	a[1] /= norm;
	a[2] /= norm;
	//cout << "after nornalize: " << x<< ", " << y << ", " << z << endl;
}

void crossProduct(const vector_t a, const vector_t b, vector_t c){
	c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = a[2]*b[0] - a[0]*b[2];
	c[2] = a[0]*b[1] - a[1]*b[0];
}

double dotProduct(const vector_t a, const vector_t b){
	return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

double cosine(double angleIndegree){
	return cos(angleIndegree * M_PI / 180.0 );
}
double sine(double angleIndegree){
	return sin(angleIndegree * M_PI / 180.0 );
}

void getVector(const vector_t from, const vector_t to, vector_t c){
	c[0] = to[0] - from[0];
	c[1] = to[1] - from[1];
	c[2] = to[2] - from[2];
}
void printM(const matrix_t matrix) {
	cout << "printing matrix:" << endl;
	for(size_t i = 0; i < 4; i++){
		cout << "[ ";
		for(size_t j = 0; j < 4; j++){
			cout << matrix[i][j] << ", ";
		}
		cout << "]" << endl;
	}
}

void printVector(const vector_t v){
	cout << "printing vector:" << endl;
	cout << "[ " << v[0] << ", " << v[1] << ", " << v[2] << "]" << endl;
}

bool isZero(double number){
	return (number > 0 - DBL_EPSILON) && (number < 0 + DBL_EPSILON);
}

void scaleVector(double scale, vec src, vec after){
	after[0] = src[0]* scale;
	after[1] = src[1]* scale;
	after[2] = src[2]* scale;
}

void scaleVector(double scale, vec after){
	after[0] *= scale;
	after[1] *= scale;
	after[2] *= scale;
}

void addVector(vec sum, vec a){
	sum[0] += a[0];
	sum[1] += a[1];
	sum[2] += a[2];
}

void  minusVector(vec sum, vec a){
	sum[0] -= a[0];
	sum[1] -= a[1];
	sum[2] -= a[2];
}