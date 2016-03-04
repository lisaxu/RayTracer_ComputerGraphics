CC = g++
CFLAGS = -c -I. -Wall -g -std=c++14 -O2
LFLAGS = -Wall -lgtest 
#obj = model.o modelBuilder.o matrix.o polygon.o scene.o util.o
#cFiles = *.c
# -O2

all: pa4 tester
pa4: model.o  modelBuilder.o main.o matrix.o util.o vector3D.o scene.o polygon.o ray.o sphere.o
	$(CC) model.o modelBuilder.o main.o matrix.o util.o vector3D.o scene.o polygon.o ray.o sphere.o $(LFLAGS) -o $@ 
tester: model.o modelBuilder.o test.o matrix.o util.o vector3D.o scene.o polygon.o ray.o sphere.o
	$(CC) modelBuilder.o test.o model.o matrix.o util.o vector3D.o scene.o polygon.o ray.o  sphere.o $(LFLAGS) -o $@ 
	
main.o: main.cpp model.h modelBuilder.h matrix.h util.h vector3D.h sphere.h
	$(CC) $(CFLAGS) $< 

test.o: test.cpp modelBuilder.h model.h matrix.h util.h scene.h vector3D.h ray.h sphere.h
	$(CC) $(CFLAGS) $< 	

model.o: model.cpp model.h util.h polygon.h vector3D.h sphere.h
	$(CC) $(CFLAGS) $<  
	
modelBuilder.o: modelBuilder.cpp modelBuilder.h model.h polygon.h vector3D.h
	$(CC) $(CFLAGS) $<
	
%.o: %.cpp %.h util.h 
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o *~ pa4 tester PA4.tar
target:
	tar -cvf PA4.tar *.h *.cpp README.txt Makefile
	

# $<:The name of the first prerequisite(dependencies)
#targets: target-pattern: prereq-patterns …
#        recipe



