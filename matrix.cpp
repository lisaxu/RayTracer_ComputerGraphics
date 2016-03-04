#include "matrix.h"
using namespace std;

void Matrix::multiply(const matrix_t transformation){
	matrix_t original;
	for(size_t i = 0; i < 4; i++)
		for(size_t j = 0; j < 4; j++)
			original[i][j] = matrix[i][j];

	for(size_t i = 0; i < 4; i++)
		for(size_t j = 0; j < 4; j++){
			matrix[i][j] = 0;
			for(size_t k = 0; k < 4; k++){
				matrix[i][j] += transformation[i][k] * original[k][j]; 
			}
		}
}


void Matrix::scale(double x, double y, double z){
	matrix_t scale = {{x,0,0,0}, {0,y,0,0}, {0,0,z,0}, {0,0,0,1}};
	multiply(scale);
}

void Matrix::translate(double x, double y, double z){
	matrix_t trans = {{1,0,0,x}, {0,1,0,y}, {0,0,1,z}, {0,0,0,1}};
	multiply(trans);
}

void transpose(const matrix_t a, matrix_t result){
	for(size_t i = 0; i < 4; i++){
		for(size_t j = 0; j < 4; j++){
			result[i][j] = a[j][i];
		}
	}
}
int findSmallest(const double w[4]){
	int smallestIndex;
	if(abs(w[0]) > abs(w[1])){
		smallestIndex = 1;
	}else{
		smallestIndex = 0;
	}
	
	if(abs(w[smallestIndex]) > abs(w[2])){
		smallestIndex = 2;
	}
	return smallestIndex;
}
void Matrix::rotate(double x, double y, double z, double theta){
	//normalize axis of rotation to get w
	double w[4] = {x,y,z,0};
	normalize(w);
	
	//m
	double m[4] = {w[0],w[1],w[2],0};
	m[findSmallest(m)] = 1;
	normalize(m);
	

	//U = W cross M
	double u[4];
	crossProduct(w,m,u);
	normalize(u);
	
	//V = W cross U
	double v[4];
	crossProduct(w,u,v);
	matrix_t moveToZ = {{u[0],u[1],u[2],0}, {v[0],v[1],v[2],0}, {w[0],w[1],w[2],0}, {0,0,0,1}};
	matrix_t rotateAboutZ = {{cosine(theta), -1 * sine(theta), 0, 0},
							 {sine(theta), cosine(theta), 0, 0},
							 {0,0,1,0}, {0,0,0,1}};
	
	matrix_t moveBack;
	transpose(moveToZ, moveBack);

	//compose together
	multiply(moveToZ);
	multiply(rotateAboutZ);
	multiply(moveBack);
}





