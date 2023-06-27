//
//  Display a rotating cube
//
#define GL_SILENCE_DEPRECATION
#include "Angel.h"
#include <glfw3.h>
#include "main.h"


void rubiksCube() {
        calculateVertices();
        colorcube();
}


mat3 faces[6] = {
    mat3(0,1,2,3,4,5,6,7,8),         // Front face
    mat3(0,1,2,9,10,11,18,19,20),    // Top face
    mat3(2,11,20,8,17,26,5,14,23),   // Left face
    mat3(18,9,0,24,15,6,21,12,3),    // Right face
    mat3(6,7,8,15,16,17,24,25,26),   // Bottom face
    mat3(18,19,20,21,22,23,24,25,26) // Back face
};
const int ppc = 36;                        // Points Per Cube = (6 faces) * (6 vertices/face)
const int lppc = 48;
mat4 globalModelView;
mat4 ModelViews[27];
float width, height;

GLfloat t;
const int NumPoints = 972 + 330;
bool selected = false;
//------------------------------- new try
GLuint pickingFramebuffer;
GLuint pickingTexture;

void initPickingFramebuffer() {
    glGenFramebuffers(1, &pickingFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pickingFramebuffer);

    glGenTextures(1, &pickingTexture);
    glBindTexture(GL_TEXTURE_2D, pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1000, 1000, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTexture, 0);

    GLuint pickingRenderBuffer;
    glGenRenderbuffers(1, &pickingRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, pickingRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1000, 1000);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: Framebuffer is not complete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void displayForPicking(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glFlush();
}



int performPicking(int x, int y) {
    displayForPicking();
    unsigned char pixel[3];
    glReadPixels(x, 1000 , 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, pickingFramebuffer);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return pixel[0] + pixel[1] * 256 + pixel[2] * 256 * 256;
}

void updateCubePositions(int face, int direction) {
    mat3 currentFace = faces[face];
    mat3 newFace;

    // Update the face positions
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k= 0; k<3; k++) {
                
                if (direction == 1) { // clockwise
                    newFace[j][2 - i] = currentFace[i][j];
                } else { // counterclockwise
                    newFace[2 - j][i] = currentFace[i][j];
                }
            }
        }
    }
    faces[face] = newFace;
}

void rotateFace(int face, int direction) {
    // face: the index of the face you want to rotate (0-5)
    // direction: 1 for clockwise, -1 for counterclockwise

    mat3 currentFace = faces[face];

    // Perform a 90-degree rotation on the face
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int currentPos = currentFace[i][j];
            switch (face) {
                case 0: // Front
                    rotationMatrix[currentPos] = RotateZ(90 * direction) * rotationMatrix[currentPos];
                    break;
                case 1: // Top
                    rotationMatrix[currentPos] = RotateY(-90 * direction) * rotationMatrix[currentPos];
                    break;
                case 2: // Left
                    rotationMatrix[currentPos] = RotateX(-90 * direction) * rotationMatrix[currentPos];
                    break;
                case 3: // Right
                    rotationMatrix[currentPos] = RotateX(90 * direction) * rotationMatrix[currentPos];
                    break;
                case 4: // Bottom
                    rotationMatrix[currentPos] = RotateY(90 * direction) * rotationMatrix[currentPos];
                    break;
                case 5: // Back
                    rotationMatrix[currentPos] = RotateZ(-90 * direction) * rotationMatrix[currentPos];
                    break;
            }
            glUniformMatrix4fv(vRotation[currentPos], 1, GL_TRUE, rotationMatrix[currentPos]);
        }
    }

    // Update the currentCubePos and nextCubePos arrays to reflect the rotation
    updateCubePositions(face, direction);
}


//---------------------------------------------------------------------
//
// init
//

void init()
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );
    rubiksCube();
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW ); //size'i kontrol et!!
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );
    
    // Retrieve transformation uniform variable locations
    GlobalModelView = glGetUniformLocation( program, "GlobalModelView" );
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    selectedFace = glGetUniformLocation( program, "selectedFace" );

    
    // Set projection matrix
    mat4  projection;
    projection = Ortho(-7, 7, -7, 7, -7, 7); // Ortho(): user-defined function in mat.h
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    initPickingFramebuffer();
}

// Returns true if a face contains a specific cube piece
bool contains(mat3 face, int cubeID){
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            if(face[i][j] == cubeID) return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------
//
// display
//

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //  Generate the model-view matrix
            const vec3 displacement( 0.0, 0.0, 0.0 );
            mat4  model_view = (  Translate( displacement ) * Scale(0.2, 0.2 ,0.2) *
                                RotateX( Theta[Xaxis] ) *
                                RotateY( Theta[Yaxis] ) *
                                RotateZ( Theta[Zaxis] ) );  // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h
            
            //glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
        
    for (int i = 0; i < 27; ++i) {
        glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
        glDrawArrays(GL_TRIANGLES, i * ppc, ppc);
    }

        
        glfwPollEvents();
    
    
    
    
    
    
    // glDrawArrays( GL_TRIANGLES, 0, NumVertices );
            //glFlush();
        
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch( key ) {
        case GLFW_KEY_ESCAPE: case GLFW_KEY_Q:
            exit( EXIT_SUCCESS );
            break;
        
        case GLFW_KEY_LEFT:
            Axis = Zaxis;
            break;
        case GLFW_KEY_DOWN:
            Axis = Xaxis;
            break;
        case GLFW_KEY_SPACE:
            Axis = 4;
            break;
        case GLFW_KEY_RIGHT:
            Axis = Yaxis;
            break;
        case GLFW_KEY_R:
            rotateFace(0, 180);
            break;

        case GLFW_KEY_L:
            rotateFace(2, 90);
            break;

        case GLFW_KEY_U:
            rotateFace(1, 90);
            break;

        case GLFW_KEY_D:
            rotateFace(4, 90);
            break;

        case GLFW_KEY_F:
            rotateFace(3, 90);
            break;

        case GLFW_KEY_B:
            rotateFace(5, 90);
            break;
        
            
    }
    
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // 
 
}



void update( void )
{
    if(Theta[Axis]  < 360) {
        Theta[Axis] += 1.0;
        
    if(Theta[Axis] == 360) {
            Theta[Axis] = 0.0;
    
        }
    }
}

//---------------------------------------------------------------------
//
// main
//

int main()
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "HW 2", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    init();
    
    double frameRate = 150, currentTime, previousTime = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1/frameRate){
            previousTime = currentTime;
            update();
        }
        
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}




