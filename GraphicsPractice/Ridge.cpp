#include "Ridge.h"



glm::vec3* Ridge::generate_ridge(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width)
{
	float mid_width = 0.2*ridge_width;//2 times smaller than ridge
	ridge_width = 0.4*ridge_width;//max width can be only 0.4 of whole as it's double ridge
	mid_low = 0.2*mid_low;
	peak_height = 0.1 + 0.1 * peak_height;// range of 0.6 - 1
	

	float ridge_peak_close = 0.5;

	float top_end = 0.5f - mid_width / 2;
	float top_start = top_end - ridge_width;
	
	float bottom_start = 0.5f + mid_width / 2;
	float bottom_end = bottom_start + ridge_width;

	glm::vec3* verts = nullptr;
	verts = new glm::vec3[resolution.x * resolution.y];

	glm::vec2 steps;
	steps.x = 1 / resolution.x;
	steps.y = 1 / resolution.y;

	for (int i = 0; i < resolution.y; i++)
	{
		for (int j = 0; j < resolution.x; j++)
		{
			float cur_y = i * steps.y;
			int cur_index = i * (int)resolution.x + j;
			if (cur_y > top_start && cur_y < top_end)
			{
				float range_size = (top_end - top_start);
				if ((cur_y - top_start) > range_size*ridge_peak_close)
				{
					verts[cur_index].y = (range_size - (cur_y - top_start)) / range_size * peak_height / ridge_peak_close;
				}
				else
				{
					verts[cur_index].y =  (cur_y - top_start) / range_size * peak_height / ridge_peak_close;
				}
					

				//verts[cur_index].y = (cur_y - top_start) / range_size * peak_height;
			}
			else if (cur_y > top_end && cur_y < bottom_start)
			{
				float range_size = (bottom_start - top_end);
				if ((cur_y - top_end) > range_size*0.5)
				{
					verts[cur_index].y = (range_size - (cur_y - top_end)) / range_size * mid_low * 2;
				}
				else
				{
					verts[cur_index].y = (cur_y - top_end) / range_size * mid_low * 2;
				}
			}
			else if(cur_y > bottom_start && cur_y < bottom_end)
			{
				float range_size = (bottom_end - bottom_start);
				if ((cur_y - bottom_start) > range_size*ridge_peak_close)
				{
					verts[cur_index].y = (range_size - (cur_y - bottom_start)) / range_size * peak_height / ridge_peak_close;
				}
				else
				{
					verts[cur_index].y = (cur_y - bottom_start) / range_size * peak_height / ridge_peak_close;
				}


				//verts[cur_index].y = (cur_y - top_start) / range_size * peak_height;
			}
			else
				verts[cur_index].y = 0;

			std::cerr << " " << verts[cur_index].y;
		}
		std::cerr << std::endl;
	}


		return verts;
}

Ridge::Ridge()
{
}


Ridge::~Ridge()
{
}
