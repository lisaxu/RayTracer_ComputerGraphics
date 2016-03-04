// PA2 Assignment
// Author: Zichun Xu
// Date:   10/12/2015
// Class:  CS410
#include <time.h>
#include "scene.h"
#include "modelBuilder.h"
#include "model.h"
#include "matrix.h"
#include "polygon.h"
using namespace std;

void printUsage(){
	cout << "USAGE: ./pa4 cameraFile materialFile plyFile1, ply..., outputImageFile" << endl;
}


int main(int argc, char *argv[])
{
	if(argc < 5){
		cerr << "Error: four or more arguments needed" << endl;
		printUsage();
		return -1;
	}
	
	size_t numModels = argc - 4;
	
	
	Scene scene(numModels);
	
	try{
		scene.readInCamera(argv[1]); //throws
	}catch(const exception& e){
		cerr << "Error in reading in camera File: " << e.what() << endl;
		return -1;
	}
	
	unique_ptr<Model>* mList = new unique_ptr<Model>[numModels];
	for(size_t i = 0; i < numModels; i++){
		ModelBuilder p(argv[3+i], scene.focalPoint);
		try{
			p.build();
		}catch(const exception& e){
			cerr << "Error in reading in model: " << e.what() << endl;
			return -1;
		}
		mList[i] = p.getModel();
		mList[i]->calculateMeanBounding();
		mList[i]->printModelInfo();
	}
	
	
	scene.setModels(mList);
	try{
		scene.readInMaterial(argv[2]);
	}catch(const exception& e){
		cerr << "Error in reading in material File: " << e.what() << endl;
		return -1;
	}	
	
	scene.createImagePlane();
	scene.printCameraInfo();
	//clock_t tStart = clock();
	scene.rayTracing();
	//cout << "ray tracing Time taken: " <<  (double)(clock() - tStart)/CLOCKS_PER_SEC * 1000 << " milli s" << endl;
	
	try{
		scene.outputImage(argv[argc-1]);
	}catch(const exception& e){
		cerr << "Error in writing image File (We're so close...): " << e.what() << endl;
		return -1;
	}

	return 0;
}









