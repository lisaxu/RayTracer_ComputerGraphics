#ifndef MATRIX_H
#define MATRIX_H

#include "util.h"


using namespace std;
class Matrix{   
public:
	Matrix() = default;
	Matrix(const Matrix&) = default;
    Matrix& operator=(const Matrix&) = default;
	~Matrix()= default;
	
	void multiply(const matrix_t transformation);
	void scale(double x, double y, double z);
	void translate(double x, double y, double z);
	void rotate(double x, double y, double z, double theta);
	bool isIdentity();
	matrix_t& getMatrix(){return matrix;}  //WHY?
	void print();
	
	matrix_t matrix = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
};


#endif