// COMP410 HW1
// Can Berk Alakir & Furkan Tuna
// Due 2/4/23
// We declare that we have consulted the book in this project
// The project is partial and does not work 100%

#include "Angel.h"
#include <GLUT/glut.h>
#define GL_SILENCE_DEPRECATION

typedef vec4  color4;
typedef vec4  point4;
const int NumVertices = 36;
point4 points[NumVertices];
color4 colors[NumVertices];

float posInitVal = 0;
float posX = posInitVal-2;
float posY = posInitVal;
float velX = 0.008;
float velY = 0.006;
float gConst = 0.001;
float friction = 0.0001;
vec4 initColor = color4( 0.0, 0.0, 0.0, 1.0 );
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4(-0.8, 0.6, 0.2, 1.0),
       point4(-0.8, 0.8, 0.2, 1.0),
       point4(-0.6, 0.8, 0.2, 1.0),
       point4(-0.6, 0.6, 0.2, 1.0),
       point4(-0.8, 0.6, -0.2, 1.0),
       point4(-0.8, 0.8, -0.2, 1.0),
       point4(-0.6, 0.8, -0.2, 1.0),
       point4(-0.6, 0.6, -0.2, 1.0)
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
//-------------------------------------------------------------
//PER FRAGMENT LIGHTING OF SPHERE MODEL
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;
const int NumVerticesSphere = 3 * NumTriangles;

point4 pointSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];
color4 colorsSphere[NumVerticesSphere];

int IndexSphere = 0;
void
triangle( const point4& a, const point4& b, const point4& c ) {
    vec3 normal = normalize( cross(b - a, c - b) );
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = a; IndexSphere++;
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = b; IndexSphere++;
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = c; IndexSphere++;
}

point4
unit( const point4& p )
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    point4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}

void divide_triangle( const point4& a, const point4& b, const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle( a, v1, v2, count - 1 );
        divide_triangle( c, v2, v3, count - 1 );
        divide_triangle( b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}
void
tetrahedron( int count) {
    point4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.333333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.333333, 1.0 )
    };
    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}

void movement( void ) {
    posX = posX + velX;
    velY = velY - gConst;
    posY = posY + velY;
    int ctr = 0;
    if (posY < 600) {
        velY = -velY;
        ctr++;
        if(ctr % 2 == 0){
            velY = velY - friction;
        } else {
            velY = velY + friction;
        }
    }
}

enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
GLuint program;

//----------------------------------------------------------------------------

int Index = 0;

void quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors

void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

GLuint vao;
void cube(){
    IndexSphere = 0;
    Index = 0;
    colorcube();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint buffer;
    glTranslatef(-0.8, 0.8, 0.0);
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
}


void sphere () {
    Index = 0;
    IndexSphere = 0;
    tetrahedron(NumTimesToSubdivide);
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pointSphere) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointSphere), pointSphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointSphere), sizeof(normals), normals);
    program = InitShader( "vshader56.glsl", "fshader56.glsl" );
    glUseProgram( program );
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(0) );
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(pointSphere)) );

    // Initialize shader lighting parameters
    point4 light_position( 0.0, 0.0, 2.0, 0.0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 1.0, 0.0, 1.0, 1.0 );
    float  material_shininess = 5.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
    glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );
    glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
         
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//---------------------------------------------------------------------
//
// init
//

void init()
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);
    colorcube(); // create the cube in terms of 6 faces each of which is made of two triangles
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    
    // Set projection matrix
    mat4  projection;
    projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Ortho(): user-defined function in mat.h
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}


//---------------------------------------------------------------------
//
// display
//

void display( void ) {
    glBindVertexArray( vao );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    movement();
    glUniformMatrix4fv( ModelView, 1, GL_TRUE );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices[] );
    glutSwapBuffers();
}

void
idle( void )
{
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch( key ) {
        case GLFW_KEY_ESCAPE: case GLFW_KEY_Q:
            exit( EXIT_SUCCESS );
            break;
    }
}
vec4 color;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    int counter = 0;
    if ( action == GLFW_MOUSE_BUTTON_RIGHT ) {
        counter++;
        if(counter % 2 != 0) {
            sphere();
            color = vec4(0.0, 0.0, 0.0, 1.0);
        } else {
            cube();
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }
}

void reshape( int width, int height ) {

    glViewport( 0, 0, width, height );
    
    GLfloat left = -2.0, right = 2.0;
    GLfloat top = 2.0, bottom = -2.0;
    GLfloat zNear = -20.0, zFar = 20.0;
    GLfloat aspect = GLfloat(width)/height;

    if (aspect > 1.0) {
        left *= aspect;
        right *= aspect;
    }
    else {
        top /= aspect;
        bottom /= aspect;
    }
    mat4 projection = Ortho(left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

//---------------------------------------------------------------------
//

int main(int argc, char **argv)
{
    if (!glfwInit()) exit(EXIT_FAILURE);
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        
        GLFWwindow* window = glfwCreateWindow(1000,1000, "Spin Cube", NULL, NULL);
        glfwMakeContextCurrent(window);
        
        if (!window) {
                glfwTerminate();
                exit(EXIT_FAILURE);
            }

        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        
        init();

        double FPS = 120, currentTime, previousTime = 0.0;
        while (!glfwWindowShouldClose(window))
        {
            currentTime = glfwGetTime();
            if (currentTime - previousTime >= 1/FPS){
                previousTime = currentTime;
                movement();
            }
            
            display();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_SUCCESS);
}




