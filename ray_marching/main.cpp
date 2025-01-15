#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include <GL/glew.h>
#include "../external/glew-1.13.0/include/GL/glew.h"

//#include <glfw3.h>
#include <GLFW/glfw3.h>
//#include "../external/glfw-3.1.2/include/GLFW/glfw3.h"

GLFWwindow* window;

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>
#include "../external/glm-0.9.7.1/glm/glm.hpp"
#include "../external/glm-0.9.7.1/glm/gtc/matrix_transform.hpp"
#include "../external/glm-0.9.7.1/glm/gtx/transform.hpp"

using namespace glm;

#include "../common/shader.hpp"
// #include "../common/texture.hpp"
// #include "../common/controls.hpp"
// #include "../common/objloader.hpp"

int main( void )
{

    //srand (time(NULL));

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1910, 1070, "Playground", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Dark blue background
    // glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    // glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    // glEnable(GL_CULL_FACE);


    int windoWidth, windowHeight;
    glfwGetWindowSize(window, &windoWidth, &windowHeight);
    glfwSetCursorPos(window, windoWidth/2, windowHeight/2);


    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    // // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "ray_marching/simplevertexshader.glsl", "ray_marching/simplefragmentshader.glsl" );


    GLuint u_resolutionID = glGetUniformLocation(programID, "iResolution");
    GLuint u_mouseID = glGetUniformLocation(programID, "iMouse");
    GLuint u_timeID = glGetUniformLocation(programID, "iTime");


    static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, // triangle 1 : begin
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,  // triangle 1 : end
    1.0f, 1.0f,0.0f, // triangle 2 : begin
    -1.0f,1.0f,0.0f,
    -1.0f, -1.0f,0.0f, // triangle 2 : end
    };

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


    // int timer = 0;
    // bool timerMove = true;
    // bool resetPos = true;

    int timer = 0;

    double mouseX = 0;
    double mouseY = 0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    do{
        timer++;

        // Draw nothing, see you in tutorial 2 !  SUP 2
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);
        // Draw triangle...

        // if (timerMove) {
        //     timer += 1;
        // }
        // if (glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS){
        //     timerMove = false;
        // }
        // if (glfwGetKey( window, GLFW_KEY_T ) == GLFW_PRESS){
        //     timerMove = true;
        // }
        // computeMatricesFromInputs();
        // if (resetPos && timer==30) {
        //     resetPos = false;
        //     initialisePositionAndAngle();
        //     glfwSetCursorPos(window, windoWidth/2, windowHeight/2);
        //     computeMatricesFromInputs();
        // }

    
        glUniform2f(u_resolutionID, (float) windoWidth, (float) windowHeight);
        double relativeCursorX = 0.0;
        double relativeCursorY = 0.0;
        glfwGetCursorPos(window, &relativeCursorX, &relativeCursorY);
        mouseX += relativeCursorX - windoWidth/2;
        mouseY -= relativeCursorY - windowHeight/2;
        glfwSetCursorPos(window, windoWidth/2, windowHeight/2);
        glUniform2f(u_mouseID, mouseX, mouseY);
        glUniform1f(u_timeID, (float) timer/100);




        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 2*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);



        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );


    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

