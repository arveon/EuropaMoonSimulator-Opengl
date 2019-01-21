/* by Aleksejs Loginovs - October 2018 */

#include "Camera.h"

void Camera::update(float dt, glm::vec3 cursor_deltas)
{
	view_matrix = glm::translate(view_matrix, glm::vec3(0, 0, -cursor_deltas.z));
}

void Camera::reset()
{//resets movement and rotation vectors of camera to 0, so it's facing the system and positioned as it initially was
	movement = glm::vec3(0);
	rotation = glm::vec3(0);
	movement.z = -20;
	view_matrix = glm::lookAt(
		glm::vec3(0, 0, -25),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

}

Camera::Camera()
{
	reset();
}


Camera::~Camera()
{
}
