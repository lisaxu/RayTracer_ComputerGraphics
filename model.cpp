#include "model.h"

using namespace std;

void Model::calculateMeanBounding(){
	double minX, minY, minZ, maxX, maxY, maxZ;
	double maxDistanceSq = -1;
	minX = maxX = vertices[0][0];
	minY = maxY = vertices[0][1];
	minZ = maxZ = vertices[0][2];
	
	
	for(size_t i = 0; i < numVertices; i++){	
		if(vertices[i][0] < minX){
			minX = vertices[i][0];
		}else if(vertices[i][0] > maxX){
			maxX = vertices[i][0];
		}
		if(vertices[i][1] < minY){
			minY = vertices[i][1];
		}else if(vertices[i][1] > maxY){
			maxY = vertices[i][1];
		}
		if(vertices[i][2] < minZ){
			minZ = vertices[i][2];
		}else if(vertices[i][2] > maxZ){
			maxZ = vertices[i][2];
		}
		double d = getDistanceSquared(meanVertex, vertices[i]);
		if( d > maxDistanceSq){
			maxDistanceSq = d;
		}
	}
	

	minBound = Vector3D(minX, minY, minZ);
	maxBound = Vector3D(maxX, maxY, maxZ);
	//double diameter = max(getNormSquared(getVector(meanVertex, minBound)), getNormSquared(getVector(meanVertex, maxBound)));
	bigSphere.setSphereSq(meanVertex, maxDistanceSq, focalPoint); 
}


void Model::multiply(const matrix_t transform){
	//4 * 4 transformation * original 4 * n matrix
	for(size_t i = 0; i < numVertices; i++){
		double original[4] = {vertices[i][0], vertices[i][1], vertices[i][2], vertices[i][3]};
		
		for(size_t j = 0; j < 4; j++){
			vertices[i][j] = 0;
			for(size_t k = 0; k < 4; k++){
				vertices[i][j] += transform[j][k] * original[k]; 
			}
		}
	}	
}

void Model::printModelMatrix() {
	cout << "printing model matrix:" << endl;
	for(size_t j = 0; j < 4; j++){
		cout << "[ ";
		for(size_t i = 0; i < numVertices; i++){
			cout << vertices[i][j] << ", ";
		}
		cout << "]" << endl;
	}
}

void Model::printModelInfo(){
	cout << "Printing model:" << endl;
	cout << "Number of vertices: " << numVertices << endl;
	cout << "Number of faces: " << numFaces << endl;
	cout << "Mean vertex: (" << meanVertex[0] << ", " << meanVertex[1] << ", " << meanVertex[2] << ")" << endl;
	cout << "Bounding box:" << endl;
	cout << "min: " << minBound << endl;
	cout << "max: " << maxBound << endl;
	cout << "Bounding " << bigSphere << endl;
	cout << "--------------------------" <<endl;
}
