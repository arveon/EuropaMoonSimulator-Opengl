/* by Aleksejs Loginovs - October 2018 */

#include "Camera.h"

void Camera::update(float dt, float cur_distance)
{
	view_matrix = glm::lookAt(
		glm::vec3(0, 0, cur_distance),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	//view_matrix = glm::translate(view_matrix, glm::vec3(0, 0, -cur_distance));
}

void Camera::reset(float distance)
{//resets movement and rotation vectors of camera to 0, so it's facing the system and positioned as it initially was
	movement = glm::vec3(0);
	rotation = glm::vec3(0);
	movement.z = distance;
	view_matrix = glm::lookAt(
		glm::vec3(0, 0, distance),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

}

Camera::Camera()
{
}


Camera::~Camera()
{
}
