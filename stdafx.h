#ifndef stdafxH
#define stdafxH

//opengl
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/gl.h>
#include <gl/glu.h>
//dos
#include <time.h>
#include <stdio.h>
#include <math.h>
//file stats
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//stl
#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
//texture
#include "classes/image/texture.h"

using namespace std;

//global log file variable - in stdafx.cpp
extern ofstream LogFile;

//sign template
template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

//typedefs
typedef void (*tVoidFunc) ();

//rand function
unsigned int RandomChance(unsigned int n);
int RandomValue(int Min, int Max);
bool IsPowerOfTwo(unsigned long x);

//IMPORTANT!
#define SHR_LAYOUT_VERTEX 0
#define SHR_LAYOUT_UV 1

#endif
