#Wei Xiangyu 20160530
#Student Number:2014011312
#Mesh

all:main.exe

main.exe:main.cpp point.h face.h edge.h edge.cpp face.cpp point.cpp
	g++ -o main.exe main.cpp point.h face.h edge.h edge.cpp face.cpp point.cpp
clean: 
	del *.obj *.exe