#pragma once
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

class CameraController
{
public:
	enum camera_mode
	{
		free,
		controlled
	};

	const float max_speed = 55.f;
	const float min_speed = 35.f;
	const float target_distance = 25.f;
	float cur_distance;
	const float start_distance = 100.f;
	const float max_distance = 130.f;
	const float min_distance = 15.f;

	Camera camera;

	void set_mode(camera_mode mode) { this->mode = mode; };
	void update(float dt, glm::vec3 cursor_deltas);

	void reset_camera() { camera.reset(start_distance); mode = controlled; cur_distance = start_distance; }
	glm::mat4 get_view_matrix() { return camera.get_view_matrix(); }
	bool is_camera_free() { return mode == free; }
	CameraController();
	~CameraController();
private:
	camera_mode mode;

};

