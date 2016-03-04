import os
#os.system("pa2 input16.pgm output.pgm>result.txt")

print "BEGIN TESTING ==="
os.system("tester")
os.system("pa4 camera/airplane_camera3.txt material/airplane_materials model/airplane.ply out.ppm > tempForPython")
os.system("diff -b out.ppm ../p3/airplaneAnswer.ppm ")

os.system("pa4 camera/airplane_camera3.txt material/airplane_materials_2point model/airplane.ply out.ppm > tempForPython")
os.system("diff -b out.ppm ../p3/airplaneAnswer2.ppm ")

print "END TESTING ==="


