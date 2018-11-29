/* by Aleksejs Loginovs - October 2018 */
#pragma once
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Constants.h"


class Camera
{
private:
	glm::vec3 direction;
	glm::vec3 movement;

	glm::vec3 rotation_dir;
	glm::vec3 rotation;

	glm::mat4 view_matrix;
public:
	void update(float dt, glm::vec2 cursor_deltas);

	void set_movement_dir(glm::vec3 movement_dir) { this->movement = movement_dir; }
	void set_x_mov(int x) { direction.x = x; }
	void set_y_mov(int y) { direction.y = y; }
	void set_z_mov(int z) { direction.z = z; }

	void set_x_rot(int x) { rotation_dir.x = x; }
	void set_y_rot(int y) { rotation_dir.y = y; }
	void set_z_rot(int z) { rotation_dir.z = z; }

	void reset();

	glm::mat4 get_view_matrix() { return view_matrix; }
	glm::vec3 get_position() { return movement; }
	Camera();
	~Camera();
};

