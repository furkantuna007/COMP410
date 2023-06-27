#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include "Angel.h"

typedef vec4  color4;
typedef vec4  point4;
const int NumVertices = 972; //(6 faces)(2 triangles/face)(3 vertices/triangle)


//------------------------------------------------------
// Rubik's Cube variables
const int numCubes = 27;
const int cubeFaces = 6;
const int faceVertices = 6;
const int numOneCubeVertices = faceVertices * cubeFaces; // 36
const int numCubeVertices = numOneCubeVertices * numCubes; // 972

// Animation speed
const int animationSpeed = 10;

// Default view aspect of the cube
double aspectX = 135.0;
double aspectZ = -45.0;
double scaleFactor = 1.0;
//------------------------------------------------------

point4 verticesArray[3][3][3][8];

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

point4 vertices[8] = {
    point4( -0.5 - 1.05, -0.5 - 1.05,  0.5 - 1.05, 1.0 ), // left bottom front
    point4( -0.5 - 1.05,  0.5 - 1.05,  0.5 - 1.05, 1.0 ), // right bottom front
    point4(  0.5 - 1.05,  0.5 - 1.05,  0.5 - 1.05, 1.0 ), // left top front
    point4(  0.5 - 1.05, -0.5 - 1.05,  0.5 - 1.05, 1.0 ), // right top front
    point4( -0.5 - 1.05, -0.5 - 1.05, -0.5 - 1.05, 1.0 ), // left bottom back
    point4( -0.5 - 1.05,  0.5 - 1.05, -0.5 - 1.05, 1.0 ), // right bottom back
    point4(  0.5 - 1.05,  0.5 - 1.05, -0.5 - 1.05, 1.0 ), // left top back
    point4(  0.5 - 1.05, -0.5 - 1.05, -0.5 - 1.05, 1.0 )  // right top back
};
// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

void calculateVertices() {
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                for (int m=0; m<8; m++) {
                    verticesArray[i][j][k][m] = vertices[m];
                }
            }
        }
    }
    
    
    
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                for (int m=0; m<8; m++) {
                    if(i==1){
                        verticesArray[i][j][k][m].x = verticesArray[i][j][k][m].x + 1.05;
                    }
                    if(i==2){
                        verticesArray[i][j][k][m].x = verticesArray[i][j][k][m].x + 2.10;
                    }
                    if(j==1){
                        verticesArray[i][j][k][m].y = verticesArray[i][j][k][m].y + 1.05;
                    }
                    if(j==2){
                        verticesArray[i][j][k][m].y = verticesArray[i][j][k][m].y + 2.10;
                    }
                    if(k==1){
                        verticesArray[i][j][k][m].z = verticesArray[i][j][k][m].z + 1.05;
                    }
                    if(k==2){
                        verticesArray[i][j][k][m].z = verticesArray[i][j][k][m].z + 2.10;
                    }
                }
            }
        }
    }
}

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection, selectedFace,GlobalModelView;
point4 points[NumVertices];
color4 colors[NumVertices];
mat4 rotationMatrix[numCubes];
color4 colorSelector[numCubes];

// Cube position
int currentCubePos[3][3][3];
int nextCubePos[3][3][3];

// Cube rotation variables
int rotationAxis = Yaxis;
int currentBlock = 1;
int rotateAngle = 0;

// Uniform location
int edge;
int currentCube;
int vRotation[numCubes];

// Variables for the shader
int selectedCube;
int cubeColor[numCubes];

//----------------------------------------------------------------------------


int Index = 0;
// quad generates two triangles for each face and assigns colors to the vertices
void quad( int a, int b, int c, int d, int e )
{
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            for (int k=0; k<3; k++) {
                //e = std::rand() % 6 + 1; this randomizing the colors but not strictly working for a rubiks cube
                colors[Index] = vertex_colors[e]; points[Index] = verticesArray[i][j][k][a]; Index++;
                colors[Index] = vertex_colors[e]; points[Index] = verticesArray[i][j][k][b]; Index++;
                colors[Index] = vertex_colors[e]; points[Index] = verticesArray[i][j][k][c]; Index++;
                colors[Index] = vertex_colors[e]; points[Index] = verticesArray[i][j][k][a]; Index++;
                colors[Index] = vertex_colors[e]; points[Index] = verticesArray[i][j][k][c]; Index++;
                colors[Index] = vertex_colors[e]; points[Index] = verticesArray[i][j][k][d]; Index++;
            }
        }
    }
}


void colorcube()
{
    quad( 1, 0, 3, 2, 1);
    quad( 2, 3, 7, 6, 2);
    quad( 3, 0, 4, 7, 3);
    quad( 6, 5, 1, 2, 4);
    quad( 4, 5, 6, 7, 5);
    quad( 5, 4, 0, 1, 6);
    
}


