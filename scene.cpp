#include "scene.h"
using namespace std;

#define LIMIT 1.0/512

void Scene::createImagePlane(){
	rows = Vmax - Vmin + 1;
	cols = Umax - Umin + 1;
	imagePlane = new Vector3D*[rows];
	for(size_t i = 0; i < rows; i++){
		imagePlane[i] = new Vector3D[cols];
	}
	nVec = getVector(focalPoint, lookatPoint);
	nVec.normalize();
	uVec = crossProduct(vup, nVec);
	uVec.normalize();
	vVec = crossProduct(nVec, uVec);	
	dnTerm = nVec * focalLength;//dn stays the same throughout the process
}


void Scene::getPixelVec(Vector3D& pixcelVector, Vector3D& pixcelPoint, double r, double c){
	//U = dn + ru + cv & normalize. 
	pixcelVector = dnTerm + uVec * r + vVec * c;	
	// L = Focal + U
	pixcelPoint = focalPoint + pixcelVector;
	pixcelVector.normalize();
}

void Scene::getPixelVecRand(Vector3D& pixcelVector, Vector3D& pixcelPoint, double r, double c){
	//U = dn + ru + cv & normalize. 
	pixcelVector = dnTerm + uVec * (r + generator.getNumber()) + vVec * (c + generator.getNumber());
	//pixcelVector = dnTerm + uVec * r  + vVec * c ;	
	// L = Focal + U
	pixcelPoint = focalPoint + pixcelVector;
	pixcelVector.normalize();
}

void Scene::rayTracing(){
	size_t r, c;
	int row, col;
	for(r = 0, row = Vmax; r < rows; r++, row--){
		for(c = 0, col = Umax; c < cols; c++, col--){
			Vector3D pixcelVector;
			Vector3D pixcelPoint;
			Vector3D color;
			
			for(int i = 0; i < 5; i++){
				getPixelVecRand(pixcelVector, pixcelPoint, col, row); 
				Ray outgoingRay(pixcelPoint, pixcelVector);
				color += giveMeColor(outgoingRay, 1 , 1);
				
				//imagePlane[r][c]= giveMeColor(outgoingRay, 1 , 1); //counter initialized as 1. [1] is from pixelPoint		
			}
			imagePlane[r][c] = color / 5.0;
			
		}
	}
}

//R = N(rayDrcn.N)*2 - rayDrcn
Vector3D getReflectDir(const Vector3D& direction, const Vector3D& normal){
	return normal * (dotProduct(direction,normal) * 2) - direction;
}

//return Vector3D color that's seen from that ray's POV, direct reflection + recursive
Vector3D Scene::giveMeColor(Ray& outgoingRay, double product, size_t counter){
	//cout << "[" << counter << "] original ray " << outgoingRay << endl;
	Vector3D color;
	//get first hit polygon
	if(!outgoingRay.getFirstHit(models, numModels, counter)){
		//cout << "[" << counter << "] didn't hit polygons, return " << color << endl;
		return color;
	}
	
	//add ambient light
	
	const Polygon& firstHitPoly = models[outgoingRay.model_index]->polygons[outgoingRay.polygon_index];
	//cout << "[" << counter << "] hit polygon " << outgoingRay.polygon_index << endl;
	color = diagonalMultiply(firstHitPoly.Kd, ambient);
	//cout << "[" << counter << "] added ambient " << color << endl;
	
	// V and adjust N
	Vector3D viewingDirection = outgoingRay.direction * (-1);
	//cout << "[" << counter << "] viewing direction " << viewingDirection << endl;
	Vector3D normal = firstHitPoly.surfaceNormal;
	if(normal.dotProduct(viewingDirection) < 0){
		normal = normal * (-1);
	}
	
	//check each light src
	for(size_t lt = 0; lt < lightSources.size(); lt++){
		Vector3D lightDirection = getVector(outgoingRay.iPoint, lightSources[lt].position);
		lightDirection.normalize(); //move 
		
		//check light src side
		if(normal.dotProduct(lightDirection) < 0){
			//cout << "[" << counter << "] light cource is on the other side of plane" << endl;
			continue; //light is on the other side of plane, away from view point
		}
		
		//check blocking
		bool block = false;
		Ray lightRay(outgoingRay.iPoint, lightDirection);
		double maxBlockDistance = getNormSquared(getVector(outgoingRay.iPoint, lightSources[lt].position));
		block = lightRay.hitBlockPlane(models, numModels, sqrt(maxBlockDistance));
		if(block){
			//cout << "[" << counter << "] light cource is blocked" << endl;
			continue;
		}
		cout << "here" << endl;
		//diffuse reflection
		color += diagonalMultiply(firstHitPoly.Kd, lightSources[lt].RGB) * dotProduct(lightDirection, normal);
		cout << "[" << counter << "] after add diffuse reflection" <<color << endl;
		//specular reflection:  R = N(L.N)*2 - L, reflection of the light source
		Vector3D reflectionDirection = getReflectDir(lightDirection, normal);
		//I = KsB(V.R)^alpha
		if(dotProduct(viewingDirection,reflectionDirection) > 0){
			color += lightSources[lt].RGB * firstHitPoly.Ks * pow(dotProduct(viewingDirection,reflectionDirection), firstHitPoly.alpha);
		}
		//cout << "[" << counter << "] after add specular reflection" <<color << endl;
	}
	//cout << "direct color: " << color << endl;
	//check if recursion is needed
	
	counter++;
	if(counter >= 10 ){
		//cout << "[" << counter << "] couter >=10, return" << endl;
		return color;
	}
	double nextProduct = product * firstHitPoly.Ks;
	//cout << "[" << counter << "] Ks product:" << nextProduct << endl;
	if(nextProduct > LIMIT){
		Ray fanSheGuang(outgoingRay.iPoint, getReflectDir(viewingDirection, normal));	
		//cout << "[" << counter << "]  fanSheGuang:" << fanSheGuang << endl;
		color += (giveMeColor(fanSheGuang, nextProduct, counter) * firstHitPoly.Ks);
	}
	nextProduct = product * firstHitPoly.translucency;
	//cout << "[" << counter << "] Kt product:" << nextProduct << endl;
	if(nextProduct > LIMIT){
		Ray zheSheGuang(outgoingRay.iPoint, viewingDirection * -1);
		//cout << "[" << counter << "]  zheSheGuang:" << zheSheGuang << endl;
		Vector3D add = (giveMeColor(zheSheGuang, nextProduct, counter) * firstHitPoly.translucency);
		
		color += add;
	}
	
	//cout << color  << endl;
	return color;
}

//currently not in use, need too many parameters
Vector3D getReflectionColor(const Vector3D& lightDirection, const Vector3D& surfaceNormal, const Polygon& poly, const LightSource& lightSrc, const Vector3D& viewingDirection){
	Vector3D result;
	
	//diffuse reflection
	result += diagonalMultiply(poly.Kd, lightSrc.RGB) * dotProduct(lightDirection, surfaceNormal);
	
	//specular reflection
	//R = N(L.N)*2 - L
	Vector3D reflectionDirection = surfaceNormal * (dotProduct(lightDirection,surfaceNormal) * 2) - lightDirection;
	//I = KsB(V.R)^alpha
	result += lightSrc.RGB * poly.Ks * pow(dotProduct(viewingDirection,reflectionDirection), poly.alpha);
	//cout << result << endl;
	return result;
}

void Scene::outputImage(const string& outputFile){
	ofstream out;
	out.open(outputFile, ios::out);
	if(!out.is_open()){
		throw ios_base::failure("Error : unable to open file: " + outputFile + " for writing image");
	} 
	//find min max
	double min, max;
	max = min = imagePlane[0][0][0];
	for(size_t r = 0; r < rows; r++){
		for(size_t c = 0; c < cols; c++){
			//if(r == 93 && c == 79) cout << "93.79: "<< imagePlane[r][c] << endl;
			for(size_t i = 0; i < 3; i++){
				if(imagePlane[r][c][i] < min){
					min = imagePlane[r][c][i];
				}else if(imagePlane[r][c][i] > max){
					max = imagePlane[r][c][i];
				}
			}
		}
	}
	
	
cout << "min: " << min << " max=" << max <<endl;
	//output
	out << "P3" << endl;
	out << cols << " " << rows << " 255" <<endl;
	if(max == min){ //that's an dull image...
		for(size_t r = 0; r < rows; r++){
			for(size_t c = 0; c < cols; c++){
				for(size_t i = 0; i < 3; i++){
					out << max << " ";
				}
			}
			out << endl;
		}
	}else{
		double diff = max - min;
		for(size_t r = 0; r < rows; r++){
			for(size_t c = 0; c < cols; c++){
				
				for(size_t i = 0; i < 3; i++){
					out << (int)(255.0 * (imagePlane[r][c][i] - min) / diff) << " ";
				}
			}
			out << endl;
		}
	}
	out.close();	
}

Vector3D readVectorPerLine(istream& istr, const string& msg){
	Vector3D result;
	string line;
	double bucket;
	getline(istr, line);
	checkFailure(istr, "line in camera file");
	
	stringstream ss(line);
	ss >> result[0] >> result[1] >> result[2];
	checkFailure(ss, "floats for " + msg + " on line: " + line);
	ss >> bucket;	

	if(!ss.fail()){
		throw ios_base::failure("extra info on line: " + line);
	}
	ss.clear();
	return result;
}

void Scene::readInCamera(const string& fileName){
	ifstream istr(fileName, ios::in);
	if(!istr.is_open()){
		throw ios_base::failure("cannot open " + fileName + " for reading");
	}
	focalPoint = readVectorPerLine(istr, "focal point");
	lookatPoint = readVectorPerLine(istr, "lookat point");
	vup = readVectorPerLine(istr, "VUP");
	
	string line;
	double bucket;
	getline(istr, line);
	checkFailure(istr, "line in camera file: focal length");
	stringstream ss(line);
	ss >> focalLength;
	checkFailure(ss, "floats for focal length on line: " + line);
	if(focalLength <= 0 ){
		throw ios_base::failure("Negative focal length");
	}
	ss >> bucket;
	if(!ss.fail()){
		throw ios_base::failure("extra info on line: " + line);
	}
	ss.clear();
	
	int temp;
	getline(istr, line);
	checkFailure(istr, "line in camera file: focal length");
	ss.str(line);
	ss >> Umin >> Vmin >> Umax >> Vmax;
	checkFailure(ss, "ints for U/Vmin/max on line: " + line);
	if(Umin > Umax || Vmin > Vmax){
		throw ios_base::failure("Bad U/Vmin/max: min bigger than max on line: " + line);
	}
	ss >> temp;
	if(!ss.fail()){
		throw ios_base::failure("extra info on line: " + line);
	}
	
}


Vector3D readVector(istream& istr, const string& msg){
	Vector3D result;
	istr >> result[0] >> result[1] >> result[2];
	checkFailure(istr, "floats for " + msg);
	return result;
}

void checkExtraDouble(istream& istr, const string& line){
	double bucket;
	istr >> bucket;	
	if(!istr.fail()){
		throw ios_base::failure("extra info on line: " + line);
	}
	istr.clear();
}

void Scene::readInMaterial(const string& fileName){
	ifstream istr(fileName, ios::in);
	if(!istr.is_open()){
		throw ios_base::failure("cannot open " + fileName + " for reading");
	}
	
	string line;
	char type;
	getline(istr, line);
	while(!istr.fail()){
		if(line.empty()){
			getline(istr, line);
			continue;
		}
		stringstream ss(line);
		ss >> type;
		checkFailure(ss, "character light(L) or Material(M) on line: " + line);
		
		switch(type){
			case 'L':{
				Vector3D rgb = readVector(ss, "light source RGB vaules");
				if(rgb[0] < 0 || rgb[0] > 1 || rgb[1] < 0 || rgb[1] > 1 || rgb[2] < 0 || rgb[2] > 1){
					throw ios_base::failure("Bad light source RGB value, must be 0-1 on line: " + line);
				}
				int stop = ss.tellg();
				double bucket;
				ss >> bucket;			
				if(!ss.fail()){
					ss.seekg(stop);
					Vector3D pos = readVector(ss, "light source 3D position");
					lightSources.push_back(LightSource(rgb,pos));	
				}else{ // we assume it's A A A				
					ambient = rgb;
				}
				break;
			}
			case 'M':{
				int index, first, last;
				ss >> index >> first >> last;
				checkFailure(ss, "material indeices on line: " + line);
				if(index < 0 || index >= (int)numModels){
					throw ios_base::failure("Model index out of bound in material file on line: " + line);
				}
				index = static_cast<size_t>(index);
				if(first < 0 || first > last || first >= (int)models[index]->numFaces || last < 0 || last >= (int)models[index]->numFaces){
					throw ios_base::failure("Model polygon range index out of bound in material file on line: " + line);
				}
				Vector3D KdRGB = readVector(ss, "material Kd matrix vaules");
				if(KdRGB[0] < 0 || KdRGB[0] > 1 || KdRGB[1] < 0 || KdRGB[1] > 1 || KdRGB[2] < 0 || KdRGB[2] > 1){
					throw ios_base::failure("Bad Kd matrix RGB value, must be 0-1 on line: " + line);
				}
				double Ks, alpha, translucency;
				ss >> Ks >> alpha >> translucency;
				checkFailure(ss, "material Ks or alpha on line: " + line);
				if(Ks < 0 || Ks > 1 || alpha < 0){
					throw ios_base::failure("Bad Ks or alpha value on line: " + line);
				}
				if(translucency < 0 || translucency > 1){
					throw ios_base::failure("Bad translucency value on line: " + line);
				}
				//put information into polygon information
				for(int i = first; i <= last; i++){
					models[index]->polygons[i].Kd = KdRGB;
					models[index]->polygons[i].Ks = Ks;
					models[index]->polygons[i].alpha = alpha;
					models[index]->polygons[i].translucency = translucency;
				}
				break;
			}
			
			default:
				throw ios_base::failure("First char is not L/M in material file on line: " + line);
		}
		getline(istr, line);
	}
	
	/*
	for(size_t modelIter = 0;  modelIter < numModels; modelIter++){
		for(size_t polyIter = 0; polyIter < models[modelIter]->numFaces; polyIter++){
			if(models[modelIter]->polygons[polyIter].alpha == -1){
				models[modelIter]->polygons[polyIter].Kd = Vector3D(0.5,0.5,0.5);
				models[modelIter]->polygons[polyIter].Ks = 0;
				models[modelIter]->polygons[polyIter].alpha = 1;
			}
		}
	}
	*/
	
}
void Scene::printCameraInfo(){
	cout << "printing camera info: " << endl;
	cout << "U min: " << Umin << ", max: " << Umax << endl;
	cout << "V min: " << Vmin << ", max: " << Vmax << endl;
	cout << "focal point: " << focalPoint<< endl;
	cout << "focal length: " << focalLength << endl;
	cout << "VUP: " <<  (vup) << endl;
	cout << "n: " <<  (nVec)<< endl;
	cout << "u: " <<  (uVec)<< endl;
	cout << "v: " <<  (vVec)<< endl;
	cout << "rows: " << rows << endl;
	cout << "col: " << cols << endl;
	cout << "---------------------" <<endl;
}
