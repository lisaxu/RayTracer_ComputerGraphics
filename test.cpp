#include "scene.h"
#include "util.h"
#include "model.h"
#include "modelBuilder.h"
#include "matrix.h"
#include "vector3D.h"
#include "ray.h"
#include "sphere.h"
#include <gtest/gtest.h>

namespace {
void expectVectorEqual(Vector3D v1, Vector3D v2, int line){
	for(int i = 0; i < 3; i++){
		EXPECT_DOUBLE_EQ(v1[i], v2.content[i]) << "### From line: " << line;
	}
}

TEST(ModelBuilder, IOerror) {
  ModelBuilder p("in.txt", Vector3D(0,0,0));
  EXPECT_THROW(p.build(), ios_base::failure);
  ModelBuilder p2("errorcase/emptyin", Vector3D(0,0,0));
  EXPECT_THROW(p2.build(), ios_base::failure);
	
  //header
  ModelBuilder p3("errorcase/elementVertex", Vector3D(0,0,0));
  EXPECT_THROW(p3.build(), ios_base::failure);
  ModelBuilder p4("errorcase/elementFace", Vector3D(0,0,0));
  EXPECT_THROW(p4.build(), ios_base::failure); 
  
  //face
  ModelBuilder f("errorcase/errorFace", Vector3D(0,0,0));
  EXPECT_THROW(f.build(), ios_base::failure); 
  ModelBuilder f1("errorcase/errorFaceLess", Vector3D(0,0,0));
  EXPECT_THROW(f.build(), ios_base::failure); 
  
  //vertices
  ModelBuilder p5("errorcase/errorVertex", Vector3D(0,0,0));
  EXPECT_THROW(p5.build(), ios_base::failure); 
  ModelBuilder p6("errorcase/extraInfo", Vector3D(0,0,0));
  EXPECT_THROW(p6.build(), ios_base::failure); 
}

TEST(ModelBuilder, buildApple) {
	ModelBuilder apple("model/apple", Vector3D(0,0,0));
	EXPECT_NO_THROW(apple.build());
	EXPECT_EQ(867, apple.getNumVertex());
	EXPECT_EQ(1704, apple.getNumFace()); 
	EXPECT_EQ(167, apple.getHeader().length());
	
	unique_ptr<Model> w(apple.getModel());
	expectVectorEqual(Vector3D(0.00472708, 0.0012, -0.000833515), w->vertices[0], __LINE__);
	expectVectorEqual(Vector3D(0, 0.07776, 0), w->vertices[866], __LINE__);	
		//cout << w->polygons[0] << endl;
	//check bounding sphere of polygon[0]
	/*
	0.00472708 0.0012 -0.000833515 
	0.0048 0.0012 0 
	0 0 0 
	mean: 0.003175693, 0.0008, -0.000277838
	*/
	expectVectorEqual(Vector3D(0.00952708/3,0.0008,-0.000833515/3), w->polygons[0].boundingSphere.center, __LINE__);
	double max = getDistanceSquared(w->polygons[0].boundingSphere.center, w->polygons[0].vertices[2]);
	EXPECT_DOUBLE_EQ(max, w->polygons[0].boundingSphere.diameterSq);
		
}


TEST(ModelBuilder, buildellell) {
	ModelBuilder ell("model/ellell", Vector3D(0,0,0));
	EXPECT_NO_THROW(ell.build());
	EXPECT_EQ(20, ell.getNumVertex());
	EXPECT_EQ(18, ell.getNumFace());
	
	unique_ptr<Model> e(ell.getModel()); 
	
	//check model bounding sphere
	e->calculateMeanBounding();
	expectVectorEqual(Vector3D(0.8,1,1), e->bigSphere.center, __LINE__);
	EXPECT_DOUBLE_EQ(5.64, e->bigSphere.diameterSq);
	
	//check polygons
	EXPECT_EQ(4, e->polygons[0].numVertices);
	EXPECT_EQ(4, e->polygons[0].numEdges);

	expectVectorEqual(Vector3D(0,0,0), e->polygons[0].vertices[0], __LINE__); //polygon0,vertex0
	expectVectorEqual(Vector3D(0,1,0), e->polygons[0].vertices[1], __LINE__); //polygon0,vertex1
	expectVectorEqual(Vector3D(1,1,0), e->polygons[0].vertices[2], __LINE__);//polygon0,vertex2
	expectVectorEqual(Vector3D(1,0,0), e->polygons[0].vertices[3], __LINE__);//polygon0,vertex3

	expectVectorEqual(Vector3D(0,1,0), e->polygons[0].edges[0], __LINE__);
	expectVectorEqual(Vector3D(1,0,0), e->polygons[0].edges[1], __LINE__);
	expectVectorEqual(Vector3D(0,-1,0), e->polygons[0].edges[2], __LINE__);
	expectVectorEqual(Vector3D(-1,0,0), e->polygons[0].edges[3], __LINE__);
	//vec norm0 = {0,0,-1};
	expectVectorEqual(Vector3D(0,0,-1), e->polygons[0].surfaceNormal, __LINE__);
	EXPECT_DOUBLE_EQ(0, e->polygons[0].dTerm);
	
	expectVectorEqual(Vector3D(0,0,-2), e->polygons[17].edges[3], __LINE__);//polygon17,edge3
	//vec finNorm = {0,-1,0}; // without normalize is 0,-2,0
	expectVectorEqual(Vector3D(0,-1,0), e->polygons[17].surfaceNormal, __LINE__);
	EXPECT_DOUBLE_EQ(2, e->polygons[17].dTerm);
	//polygon bounding sphere
	expectVectorEqual(Vector3D(0.5,0.5,0), e->polygons[0].boundingSphere.center, __LINE__);
	expectVectorEqual(Vector3D(0.5,2,2), e->polygons[17].boundingSphere.center, __LINE__);
	EXPECT_DOUBLE_EQ(1.25, e->polygons[17].boundingSphere.diameterSq);

}

TEST(Model, calculateModelStat) {
	ModelBuilder test("model/test", Vector3D(0,0,0));
	EXPECT_NO_THROW(test.build());
	unique_ptr<Model> t(test.getModel());
	t->calculateMeanBounding();
	expectVectorEqual(Vector3D(2,3,6), t->meanVertex,  __LINE__);

	expectVectorEqual(Vector3D(-9.9,0,-1), t->minBound,  __LINE__);
	expectVectorEqual(Vector3D(9.9,8,9), t->maxBound,  __LINE__);
}

void testMatrixEqual(const matrix_t& a, const matrix_t& b){
	for(size_t i = 0; i < 4; i++)
		for(size_t j = 0; j < 4; j++){
			EXPECT_DOUBLE_EQ(a[i][j], b[i][j]);
		}
}

void testMatrixAlmostEqual(const matrix_t& a, const matrix_t& b){
	for(size_t i = 0; i < 4; i++)
		for(size_t j = 0; j < 4; j++){
			EXPECT_NEAR(a[i][j], b[i][j], 1e-15);
		}
}

matrix_t identity = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0},{0,0,0,1}};

TEST(Matrix, scale) {
	Matrix m;
	testMatrixEqual(identity, m.getMatrix());
	m.scale(5,4,3);
	matrix_t correct = {{5,0,0,0}, {0,4,0,0},{0,0,3,0},{0,0,0,1}};
	testMatrixEqual(correct, m.getMatrix());
	m.scale(1.0/5,1.0/4,1.0/3);
	testMatrixEqual(identity, m.getMatrix());
}

TEST(Matrix, translate) {
	Matrix m;
	m.translate(5,4,3);
	matrix_t correct = {{1,0,0,5}, {0,1,0,4}, {0,0,1,3}, {0,0,0,1}};
	testMatrixEqual(correct, m.getMatrix());
	m.translate(-5,-4,-3);
	testMatrixEqual(identity, m.getMatrix());
}

TEST(Matrix, rotate) {
	Matrix m;
	//test rotate
	m.rotate(1,2,3,45);
	//printM(m.matrix);
	m.rotate(1,2,3,-45);
	testMatrixAlmostEqual(identity, m.getMatrix());
	
	Matrix con;
	con.rotate(1,0,0,180); //x axis 180 degree
	matrix_t correct = {{1,0,0,0}, {0,-1,0,0}, {0,0,-1,0}, {0,0,0,1}};
	testMatrixAlmostEqual(correct, con.getMatrix());
}

TEST(Model, multiplyTransformationMatrix) {
	ModelBuilder test("model/test", Vector3D(0,0,0));
	EXPECT_NO_THROW(test.build());
	unique_ptr<Model> t(test.getModel());
	t->calculateMeanBounding();
	//t->printModelMatrix();
	Matrix m;
	m.scale(-1,-1,1);
	t->multiply(m.matrix);
	//t->printModelMatrix();
}

TEST(scene, FileIO){
	Scene s(0);
	EXPECT_THROW(s.readInCamera("camera/NoSuchFile"), ios_base::failure);
	Scene s1(0);
	EXPECT_THROW(s1.readInCamera("camera/error1"), ios_base::failure);
	Scene s2(0);
	EXPECT_THROW(s2.readInCamera("camera/error2"), ios_base::failure);
	Scene s3(0);
	EXPECT_THROW(s3.readInCamera("camera/error3"), ios_base::failure);
	Scene s4(0);
	EXPECT_THROW(s4.readInCamera("camera/error4"), ios_base::failure);
	Scene s5(0);
	EXPECT_THROW(s5.readInCamera("camera/error5"), ios_base::failure);
	Scene s6(0);
	EXPECT_THROW(s6.readInCamera("camera/error6"), ios_base::failure);
	Scene corr(0);
	EXPECT_NO_THROW(corr.readInCamera("camera/example"));

	expectVectorEqual(Vector3D(0,0,0), corr.focalPoint, __LINE__); 
	expectVectorEqual(Vector3D(0,0,-100), corr.lookatPoint, __LINE__);
	expectVectorEqual(Vector3D(0,2,0), corr.vup, __LINE__);
	EXPECT_DOUBLE_EQ(340, corr.focalLength);
	EXPECT_EQ(-100, corr.Umin);
	EXPECT_EQ(-128, corr.Vmin);
	EXPECT_EQ(127, corr.Umax);
	EXPECT_EQ(127, corr.Vmax);
	corr.createImagePlane();
	EXPECT_EQ(256, corr.rows);
	EXPECT_EQ(228, corr.cols);

	expectVectorEqual(Vector3D(0,0,-1), corr.nVec, __LINE__); 
	expectVectorEqual(Vector3D(-1,0,0), corr.uVec, __LINE__);
	expectVectorEqual(Vector3D(0,1,0), corr.vVec, __LINE__);
	
	double test = 0;
	ASSERT_TRUE(isZero(test));
	test = 10.0 / 3.0 - 3;
	ASSERT_FALSE(isZero(test));
	test = 10.0 / 3.0 - 10.0 / 3.0;
	ASSERT_TRUE(isZero(test));
}

TEST(Scene, materialIO){
	ModelBuilder p("model/ellell", Vector3D(0,0,0));
	p.build();
	
	unique_ptr<Model>* mList = new unique_ptr<Model>[1];
	mList[0] = p.getModel();
	
	Scene test(1);
	test.setModels(mList);
	EXPECT_NO_THROW(test.readInCamera("camera/example"));
	EXPECT_NO_THROW(test.readInMaterial("material/example"));

	//for(int i = 0; i < 18; i++){
	//	cout << mList[0]->polygons[i] << endl;
	//}
	expectVectorEqual(Vector3D(1,1,1), test.ambient, __LINE__);
}

TEST(scene, LandU){
	Scene corr(0);
	EXPECT_NO_THROW(corr.readInCamera("camera/example"));
	corr.createImagePlane();
	Vector3D pixcelVector;
	Vector3D pixcelPoint;
	corr.getPixelVec(pixcelVector, pixcelPoint, 34, 56); //r, c
	//pixcelVector = dn+ru+cv = 340*(0,0,-1) + 34*(-1,0,0) + 56*(0,1,0) = (-34,56,-340) and normalize
	Vector3D expVec(-34.0/sqrt(119892), 56.0/sqrt(119892),-340.0/sqrt(119892));
	expectVectorEqual(expVec, pixcelVector, __LINE__);
	//pixcelPoint L = dn+ru+cv+focal = (-34,56,-340)+ (0,0,0)
	Vector3D expVecP(-34.0, 56.0,-340.0);
	expectVectorEqual(expVecP, pixcelPoint, __LINE__);
	
	for(int i = 0; i < 100; i++){
		corr.getPixelVecRand(pixcelVector, pixcelPoint, 34, 56);
		//cout  << pixcelPoint[1] << endl;
	}
}

TEST(Vector3D, basic){
	Vector3D a(0,2,3);
	Vector3D b(4,5,2);
	Vector3D result;
	//test subscript operator [] overload
	EXPECT_DOUBLE_EQ(5, b[1]);
	a[0] = 1;
	EXPECT_DOUBLE_EQ(1, a[0]);
	//test copy constructor
	Vector3D cp = b;
	cp[0] = 9999;
	EXPECT_DOUBLE_EQ(4, b[0]);
	
	//test + - *
	result = a + b;
	expectVectorEqual(Vector3D(5,7,5),result,  __LINE__);
	a+= b;
	expectVectorEqual(Vector3D(5,7,5), a,  __LINE__);
	a-=b;
	expectVectorEqual(Vector3D(1,2,3), a,  __LINE__);
	result = a - b;
	expectVectorEqual(Vector3D(-3,-3,1),result,  __LINE__);
	result = a*4;
	expectVectorEqual(Vector3D(4,8,12),result,  __LINE__);
	result = a / 4;
	expectVectorEqual(Vector3D(1.0/4,2.0/4,3.0/4),result,  __LINE__);
	//test normalize
	Vector3D t(1, 8.9, -9.33);
	EXPECT_DOUBLE_EQ(167.2589, t.getNormSquared());
	EXPECT_DOUBLE_EQ(167.2589, getNormSquared(t));
	t.normalize();
	EXPECT_DOUBLE_EQ(1, t.getNormSquared());
	//test cross product
	result= a.crossProduct(b);
	expectVectorEqual(Vector3D(-11,10,-3), result, __LINE__);
	result= crossProduct(a, b);
	expectVectorEqual(Vector3D(-11,10,-3), result, __LINE__);
	//test dot product
	a = Vector3D(2,-3,7);
	b = Vector3D(-4,2,-4);
	EXPECT_DOUBLE_EQ(-42, a.dotProduct(b));
	//diagonal multiply
	expectVectorEqual(Vector3D(-8,-6,-28), diagonalMultiply(a,b), __LINE__);
	//test getVector
	result = getVector(a,b);
	expectVectorEqual(Vector3D(-6,5,-11), result, __LINE__);
	//get distanceSq between a b 
	EXPECT_DOUBLE_EQ(182, getDistanceSquared(a,b));

}

TEST(Ray, intersectPlane){
	ModelBuilder p("model/ellell", Vector3D(0,0,0));
	p.build();
	unique_ptr<Model> modelPtr(p.getModel());
	
	Ray testRay(Vector3D(-1,10,9),Vector3D(1,0,0));
	Ray testRay2(Vector3D(-1,10,9),Vector3D(-1,10,9));
	//polygon0 is surfNorm(0,0,-1), d = 0
	ASSERT_FALSE(testRay.intersectPlane(&(modelPtr->polygons[0])));
	ASSERT_FALSE(testRay2.intersectPlane(&(modelPtr->polygons[0])));
	//Vector3D pixcelVector2(1,0,-23);
	Ray testRay3(Vector3D(-1,10,9),Vector3D(1,0,-23));
	ASSERT_TRUE(testRay3.intersectPlane(&(modelPtr->polygons[0])));
	//t = 9/23
	Vector3D exp(-14.0/23, 10, 0);
	expectVectorEqual(exp, testRay3.iPoint, __LINE__);
	
	//polygon17 is surfNorm(0,-1,0), d = 20
	ASSERT_FALSE(testRay.intersectPlane(&(modelPtr->polygons[17])));
	testRay = Ray(Vector3D(23,-3,9),Vector3D(23,-3,9));
	ASSERT_FALSE(testRay.intersectPlane(&(modelPtr->polygons[17])));
	testRay = Ray(Vector3D(23,-3,9),Vector3D(-1,10,9));
	ASSERT_TRUE(testRay.intersectPlane(&(modelPtr->polygons[17])));
	Vector3D exp2(22.5,2,13.5);
	expectVectorEqual(exp2, testRay.iPoint, __LINE__);
}

TEST(Ray, isInside){
	ModelBuilder p("model/ellell", Vector3D(0,0,0));
	p.build();
	unique_ptr<Model> modelPtr(p.getModel());
	
	Ray testRay;
	testRay.iPoint = Vector3D(-1,0.9,0);
	ASSERT_FALSE(testRay.isInsidePolygon(&(modelPtr->polygons[0])));
	testRay.iPoint = Vector3D(0.9,-1,0);
	ASSERT_FALSE(testRay.isInsidePolygon(&(modelPtr->polygons[0])));
	testRay.iPoint = Vector3D(0.9,0.9,0);
	ASSERT_TRUE(testRay.isInsidePolygon(&(modelPtr->polygons[0])));
	testRay.iPoint = Vector3D(0.9,1,0);
	ASSERT_TRUE(testRay.isInsidePolygon(&(modelPtr->polygons[0])));
	testRay.iPoint = Vector3D(0,0,0);
	ASSERT_TRUE(testRay.isInsidePolygon(&(modelPtr->polygons[0])));

	testRay.iPoint = Vector3D(1,2,3);
	ASSERT_TRUE(testRay.isInsidePolygon(&(modelPtr->polygons[17])));
	testRay.iPoint = Vector3D(0,1,3);
	ASSERT_TRUE(testRay.isInsidePolygon(&(modelPtr->polygons[17])));
}

TEST(Ray, hitCloserPolygon){
	ModelBuilder p("model/ellell", Vector3D(-2, 0.5,0.5));
	p.build();
	unique_ptr<Model> modelPtr(p.getModel());
	
	Ray testRay(Vector3D(-1,0.5,0.5),Vector3D(1,0,0));
	
	ASSERT_TRUE(testRay.hitCloserPolygon(&(modelPtr->polygons[11])));
	ASSERT_FALSE(testRay.hitCloserPolygon(&(modelPtr->polygons[0])));
	ASSERT_FALSE(testRay.hitCloserPolygon(&(modelPtr->polygons[7])));
	ASSERT_FALSE(testRay.hitCloserPolygon(&(modelPtr->polygons[12])));
	ASSERT_TRUE(testRay.hitCloserPolygon(&(modelPtr->polygons[6])));
	ASSERT_FALSE(testRay.hitCloserPolygon(&(modelPtr->polygons[11])));
	expectVectorEqual(Vector3D(0, 0.5, 0.5), testRay.iPoint, __LINE__);
	
	testRay = Ray(Vector3D(0, 0.5, 0.5),Vector3D(1,0,0));
	ASSERT_FALSE(testRay.hitCloserPolygon(&(modelPtr->polygons[6])));
}

TEST(Ray, getFirstHitBlockHit){
	ModelBuilder p("model/ellell", Vector3D(3, 0.5,0.5));
	p.build();
	
	unique_ptr<Model> mList[1];
	mList[0] = p.getModel();
	mList[0]->calculateMeanBounding();
	Ray testRay(Vector3D(3,0.5,0.5),Vector3D(-1,0,0));
	//ASSERT_TRUE(testRay.getFirstHit(mList,1,1));
	//EXPECT_EQ(11, testRay.polygon_index);
	//EXPECT_EQ(0, testRay.model_index);
	
	//Ray testRay2(Vector3D(2, 1, 0.5 ),Vector3D(-2.0/sqrt(4.25),0.5/sqrt(4.25),0));
	//ASSERT_TRUE(testRay2.getFirstHit(mList,1,1)); // hit 7-> update to 9 -> update to 11
	//EXPECT_EQ(9, testRay2.polygon_index);
	//EXPECT_EQ(0, testRay2.model_index);
	
	ASSERT_TRUE(testRay.hitBlockPlane(mList,1, 1));
	//ASSERT_FALSE(testRay.hitBlockPlane(mList,1, 0.9));
	
	//Ray testRay3(Vector3D(2,0.5,0.5),Vector3D(-1,0,0));
	//ASSERT_FALSE(testRay3.hitBlockPlane(mList,1, 1));
}

TEST(Sphere, basic){
	Sphere test(Vector3D(1,5,10), 5, Vector3D(1,2,3));
	expectVectorEqual(Vector3D(0,3,7), test.EO, __LINE__);
	EXPECT_DOUBLE_EQ(25, test.diameterSq);
	EXPECT_DOUBLE_EQ(58, test.distanceSq);
	ASSERT_FALSE(hitSphere(test, Vector3D(0,1,0)));
	ASSERT_TRUE(hitSphere(test, Vector3D(0,0,1)));

}

TEST(Sphere, Ray){
//Sphere(const Vector3D& ctr, double dia, const Vector3D& focal)
	Sphere test(Vector3D(1,5,10), 5, Vector3D(1,2,3));
	Sphere back(Vector3D(-10,-10,-10), 5, Vector3D(1,2,3));
	Ray ray(Vector3D(0,1,2), Vector3D(0,1,0));
	Ray ray2(Vector3D(0,1,2), Vector3D(0,0,1));
	
	EXPECT_DOUBLE_EQ(25, test.diameterSq);
	ASSERT_FALSE(ray.hitSphereNF(test));
	ASSERT_TRUE(ray2.hitSphereNF(test));
	ASSERT_FALSE(ray.hitSphereNF(back));
	ASSERT_FALSE(ray2.hitSphereNF(back));

}

TEST(FUnction, reflectionCOlor){
	LightSource lightSrc(Vector3D(0.1, 0.2, 0.3), Vector3D(1,2,3));
	Vector3D surfaceNormal(0,1,0);
	Vector3D viewingDirection(1/sqrt(5), 2/sqrt(5),0);
	Vector3D lightDirection(-3.0/5, 4.0/5, 0);
	
	Polygon poly;
	poly.Kd = Vector3D(0.5,1,0.3); 
	poly.Ks = 0.1; //how specular it is, %light reflected, mirror = 1
	poly.alpha = 200;
	
	
	Vector3D result = getReflectionColor(lightDirection, surfaceNormal, poly, lightSrc, viewingDirection);
	//diffuse reflection is [0.04,0.16,0.072]
	//specular reflection [0.00968,0.01936,0.02904,]
	//expectVectorEqual(Vector3D(0.04968,0.17936,0.10104), result, __LINE__);
	Vector3D direction(1,2,3);
	result = getReflectDir(direction, surfaceNormal);
	expectVectorEqual(Vector3D(-1, 2, -3), result , __LINE__);
}

TEST(randomNumber, rand){
	numGen test;
	for(int i = 0; i < 100; i++){
		//cout << test.getNumber() << endl;
	}
}

TEST(multiBounce,ellell){
	ModelBuilder p("model/ellell", Vector3D(0,0,0));
	p.build();
	
	unique_ptr<Model>* mList = new unique_ptr<Model>[1];
	mList[0] = p.getModel();
	
	Scene test(1);
	test.setModels(mList);
	EXPECT_NO_THROW(test.readInCamera("camera/ellellTestCase"));
	EXPECT_NO_THROW(test.readInMaterial("material/example"));
	
	Ray testRay(Vector3D(-1,0.5,0.5), Vector3D(1,0,0));
	
	//cout << test.giveMeColor(testRay, 1,0) <<endl;
}

TEST(multiBounce,simpleplane){
	ModelBuilder p("model/applePlane.ply", Vector3D(0,0,10000));
	p.build();
	ModelBuilder a("model/furtherPlane", Vector3D(0,0,10000));
	a.build();
	
	unique_ptr<Model>* mList = new unique_ptr<Model>[2];
	mList[0] = p.getModel();
	mList[1] = a.getModel();
	
	Scene test(2);
	test.setModels(mList);
	EXPECT_NO_THROW(test.readInCamera("camera/apple_camera1"));
	EXPECT_NO_THROW(test.readInMaterial("material/plane"));
	
	Ray testRay(Vector3D(0,0,10000), Vector3D(0,0,-1));
	
	//cout << test.giveMeColor(testRay, 1,0) <<endl;
}
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
 
