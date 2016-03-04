#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <memory>
#include <vector>
#include <limits>
#include <cmath>
#include <cfloat>
#include <stdexcept>
#include <array>
#include <algorithm> 

using namespace std;
typedef double matrix_t[4][4]; //4*4  matrix, for transformation
typedef double vector_t[4]; //(x,y,z,w), in 3D homogeneous coordinates
typedef double vec[3];
typedef double RGB[3];

void checkFailure(istream& istr, const string& msg);
void printM(const matrix_t matrix);
void printVector(const vector_t v);
void normalize(vector_t a);
double getNormSquared(const vector_t a); 
void crossProduct(const vector_t a, const vector_t b, vector_t c);
double dotProduct(const vector_t a, const vector_t b);
void getVector(const vector_t from, const vector_t to, vector_t c);
bool isZero(double number);
void scaleVector(double scale, vec src, vec after);
void scaleVector(double scale, vec after);
void addVector(vec sum, vec a);
void minusVector(vec sum, vec a);

double cosine(double angleIndegree);
double sine(double angleIndegree);




#endif
