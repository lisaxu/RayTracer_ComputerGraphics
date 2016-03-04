#include "polygon.h"

using namespace std;

ostream& operator<<(ostream& out, const Polygon& p){
	p.printPolygon(out);
	return out;
}

void Polygon::printPolygon(ostream& out) const{
	out << "-----------print polygon-------------" << endl;
	cout << "Vertices: ";
	for(size_t i = 0; i < numVertices; i++){
		cout << vertices[i];
	}
	cout << endl;
	cout << "Edges: ";
	for(size_t i = 0; i < numEdges; i++){
		cout << edges[i];
	}
	cout << endl;
	cout << "Surface Nornal: " << surfaceNormal << endl;
	cout << "Bounding " << boundingSphere << flush;
	cout << "material Kd " << Kd << " Ks: " << Ks << " alpha: " << alpha << " translucency: " << translucency << endl; 
	out << "-----------End polygon-------------" << endl;
}

