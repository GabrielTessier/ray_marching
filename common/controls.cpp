
#include <stdio.h>
// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include "../external/glm-0.9.7.1/glm/glm.hpp"
#include "../external/glm-0.9.7.1/glm/gtc/matrix_transform.hpp"
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 5, 5, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.9f;
// Initial vertical angle : none
float verticalAngle = -0.6f;
//3.916140, -0.659511
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void initialisePositionAndAngle() {
    printf("INIT POS ANGLE\n");
    position = glm::vec3( 5, 5, 5 );
    horizontalAngle = 3.9f;
    verticalAngle = -0.6f;
}

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

    int windoWidth, windowHeight;
    glfwGetWindowSize(window, &windoWidth, &windowHeight);

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
    glfwSetCursorPos(window, windoWidth/2, windowHeight/2);

	// Compute new orientation
    horizontalAngle += mouseSpeed * float(windoWidth/2 - xpos );
    verticalAngle   += mouseSpeed * float( windowHeight/2 - ypos );

    if (verticalAngle > 3.14f/2.0f) verticalAngle = 3.14f/2.0f;
    if (verticalAngle < -3.14f/2.0f) verticalAngle = -3.14f/2.0f;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
        sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
    glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );
	
	// World Up Vector
	glm::vec3 top(0, 1, 0);

    glm::vec3 forward = glm::vec3(sin(horizontalAngle), 0, cos(horizontalAngle));

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
        position += forward * deltaTime * speed;  //direction
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        position -= forward * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
	// Move up
	if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS){
		position += top * deltaTime * speed;
	}
	// Move down
	if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS){
		position -= top * deltaTime * speed;
	}

    if (glfwGetKey( window, GLFW_KEY_Y ) == GLFW_PRESS){
        verticalAngle   += mouseSpeed * 2.0f;
    }
    // Move down
    if (glfwGetKey( window, GLFW_KEY_H ) == GLFW_PRESS){
        verticalAngle   -= mouseSpeed * 2.0f;
    }

    //INIT
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        initialisePositionAndAngle();
    }

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(FoV, float(windoWidth) / float(windowHeight), 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
