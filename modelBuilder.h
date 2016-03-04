#ifndef POLYBUILDER_H
#define POLYBUILDER_H
#include "model.h"
#include "vector3D.h"
using namespace std;
class ModelBuilder{
public:
	ModelBuilder(string file, Vector3D foco): filename(file), focalPoint(foco){}
		//cout<< "polybuilder constructed" << endl; 
		//polygon = make_unique<Polygon>(4,5);
	ModelBuilder(const ModelBuilder&) = delete;
    ModelBuilder& operator=(const ModelBuilder&) = delete;
	~ModelBuilder(){}
	
	void build();
	unique_ptr<Model> getModel();
	size_t getNumVertex(){return numVertex;}
	size_t getNumFace(){return numFace;}
	string getHeader(){return header;}
	
private:
	void openFile();
	void readHeader();
	void readVertices();
	void readFaces();
	void checkExtra();
	void getNextLine(string& line, const string& msg);
	void readNumVerticesFaces();
	void readRemain();
	unique_ptr<Model> modelPtr;
	string filename;
	size_t numVertex;
	size_t numFace;
	ifstream istr; //temporary use obj, not copyable
	string header;
	
	Vector3D focalPoint;
};

void checkFailure(istream& istr, const string& msg);

#endif
