#include "modelBuilder.h"
using namespace std;



void ModelBuilder::build(){
	openFile();
	readHeader();
	readVertices();
	readFaces();
	checkExtra();
}
	
void ModelBuilder::openFile(){
	istr.open(filename,ios::in);
	if(!istr.is_open()){
		throw ios_base::failure("cannot open " + filename + " for reading");
	}
}

void ModelBuilder::getNextLine(string& line, const string& msg){
	getline(istr, line);
	checkFailure(istr, msg);
}

void ModelBuilder::readHeader(){
	readNumVerticesFaces();
	modelPtr = make_unique<Model>(numVertex, numFace, focalPoint); //create new space for vertices, face, polygons
	readRemain();
}


void ModelBuilder::readNumVerticesFaces(){
	bool vset = false;
	bool fset = false;
	string line;
	while(!(vset && fset)){
		getNextLine(line, "next line for info on the number of vertices, faces"); 
		header += line + "\n";
		
		stringstream ss(line);
		//cout << "string stream: " << line << endl;
		string word;
		ss >> word;
		checkFailure(ss, "the first word on string: " + line);
		if(strcmp(word.c_str(), "element") != 0)
			continue;
		else{
			ss >> word;
			checkFailure(ss, "the next word in string: " + line );
			if(strcmp(word.c_str(), "vertex") == 0){
				ss >> numVertex;
				checkFailure(ss, "vertex number in string: " + line );
				vset = true;
			}else if(strcmp(word.c_str(), "face") == 0){
				ss >> numFace;
				checkFailure(ss, "face number in string: " + line );
				fset = true;
			}
		}
	}
}

void ModelBuilder::readRemain(){
	string line;
	getNextLine(line, "next line to find end_header");
	header+=line + "\n";	
	while(strncmp("end_header", line.c_str(), 10) != 0){
		getNextLine(line, "next line to find end_header");
		header+=line + "\n";		
	}
}

void ModelBuilder::readVertices(){
	string line;
	double x,y,z;
	for(size_t i = 0; i < numVertex; i++){
		getNextLine(line, "next line for vertex #" + to_string(i));
		stringstream ss(line);
		ss >> x >> y >> z;
		checkFailure(ss, "vertices in string: " + line);
		modelPtr->vertices[i] = Vector3D(x,y,z);
		modelPtr->meanVertex += modelPtr->vertices[i];
	}
	modelPtr->meanVertex /= numVertex;
}

void updateMinMax(const Vector3D& target, Vector3D& minBound, Vector3D& maxBound){
	for(size_t i = 0; i < 3; i++){
		if(target[i] < minBound[i]){
			minBound[i] = target[i];
		}else if(target[i] > maxBound[i]){
			maxBound[i] = target[i];
		}
	}
}

void ModelBuilder::readFaces(){
	string line;
	int sizeBucket, verBucket;
	Vector3D edgeVector, normVector;
	
	for(size_t i = 0; i < numFace; i++){
		getNextLine(line, "next line for face #" + to_string(i));
		stringstream ss(line);
		ss >> sizeBucket; //there will be this many vertices index
		if(ss.fail()){ 
			throw ios_base::failure("Invalid number of vertex in face list line [" + to_string(i) + "]: " + to_string(sizeBucket));
		}
		if(sizeBucket < 3){
			throw ios_base::failure("Bad number of vertex in face list line [" + to_string(i) + "]: " + to_string(sizeBucket));
		}
		
		//initialize polygon object's vertices list
		modelPtr->polygons[i].initialize(sizeBucket);
		Vector3D sum; //for sphere center
		for(int k = 0; k < sizeBucket; k++){
			ss >> verBucket;
			if(verBucket < 0 || static_cast<size_t>(verBucket) > numVertex - 1){
				throw ios_base::failure("Bad vertex index in face list [" + to_string(i) + "]: " + to_string(verBucket));
			}else{		
					modelPtr->polygons[i].vertices[k] = modelPtr->vertices[verBucket];
					sum += modelPtr->polygons[i].vertices[k];
					/*if(k == 0){
						minBound = maxBound = modelPtr->polygons[i].vertices[k];
					}else{
						updateMinMax(modelPtr->polygons[i].vertices[k], minBound, maxBound);
					}*/
			}
			
			if(k > 0){ //compute edge
				edgeVector = getVector(modelPtr->polygons[i].vertices[k-1], modelPtr->polygons[i].vertices[k]);
				modelPtr->polygons[i].edges[k-1] = edgeVector;
			}
		}
		//last edge, from last vertex to vertex 0
		edgeVector = getVector(modelPtr->polygons[i].vertices[sizeBucket - 1], modelPtr->polygons[i].vertices[0]);
		modelPtr->polygons[i].edges[sizeBucket - 1]= edgeVector;

		//compute surface normal
		for(size_t n = 1; n < modelPtr->polygons[i].numEdges; n++){
			normVector = crossProduct(modelPtr->polygons[i].edges[n-1], modelPtr->polygons[i].edges[n]);
			if(!(isZero(normVector[0]) && isZero(normVector[1]) && isZero(normVector[2]))){
				modelPtr->polygons[i].surfaceNormal = normVector;
				modelPtr->polygons[i].surfaceNormal.normalize();
				modelPtr->polygons[i].hasNoVolumn = false;
				break;
			}	
		}//otherwise, hasNoVolumn=true;

		//compute d
		modelPtr->polygons[i].dTerm = -1 * dotProduct(modelPtr->polygons[i].surfaceNormal, modelPtr->polygons[i].vertices[0]);
		//compute Bounding Sphere
		Vector3D ctr = sum /sizeBucket;
		double maxDistanceSq = -1;
		for(size_t v = 0; v < modelPtr->polygons[i].numVertices; v++){
			double normSq = getDistanceSquared(ctr, modelPtr->polygons[i].vertices[v]);
			if( normSq > maxDistanceSq){
				maxDistanceSq = normSq;
			}
		}
		//double diameter = max(getNormSquared(getVector(ctr, minBound)), getNormSquared(getVector(ctr, maxBound)));
		modelPtr->polygons[i].boundingSphere.setSphereSq(ctr, maxDistanceSq, focalPoint);
	
	}
}

void ModelBuilder::checkExtra(){
	//see if there's tailing faces info
	string line;
	try{
		getNextLine(line, "extra lines");
	}catch(...){
		return;
	}
	
	double temp;
	stringstream ss(line);
	ss >> temp;
	if(!ss.fail()){
		throw ios_base::failure("Extra line for face: " + line);
	}
}

unique_ptr<Model> ModelBuilder::getModel(){
	//cout << "get polygon" << endl; 
	return move(modelPtr);
}
