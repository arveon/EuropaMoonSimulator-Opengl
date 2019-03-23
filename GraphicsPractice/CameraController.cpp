#include "CameraController.h"



void CameraController::update(float dt, glm::vec3 cursor_deltas)
{
	if (mode == free)
	{
		cur_distance -= cursor_deltas.z;
		camera.update(dt, -cur_distance);
	}
	else if (mode == controlled)
	{
		float pc_done = ((cur_distance - start_distance) / (target_distance - start_distance));
		float cur_speed = 0;
		if (pc_done < .8f)
			cur_speed = (pc_done * (max_speed - min_speed)) + min_speed;
		else
			cur_speed = (1.f-pc_done)*(max_speed) + 1.0f;

		float delta_move = cur_speed * dt;
		cur_distance -= delta_move;

		camera.update(dt,-cur_distance);
		if (cur_distance <= target_distance)
		{
			mode = free;
			cur_distance = target_distance;
		}
	}

	//make sure only max and min are there
	cur_distance = (cur_distance > max_distance) ? max_distance : cur_distance;
	cur_distance = (cur_distance < min_distance) ? min_distance : cur_distance;

}

CameraController::CameraController()
{
	camera.reset(start_distance);
	cur_distance = start_distance;
}


CameraController::~CameraController()
{
}
