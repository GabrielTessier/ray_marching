#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void initialisePositionAndAngle();
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif
