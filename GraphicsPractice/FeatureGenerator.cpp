/*	by Aleksejs Loginovs - November 2018
	generates specific terrain features e.g. ridges or craters
*/

#include "FeatureGenerator.h"



std::vector<glm::vec3> FeatureGenerator::generate_ridge(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width)
{
	//the low part of ridge will take whatever is left over from ridges
	float mid_width = 1.f - ridge_width * 2;

	//double ridge generated consists of a top single ridge, bottom single ridge and a middle through
	//the single ridge formation at the top of the whole formation generated
	float top_end = 0.5f - mid_width / 2;//the formation is always centered around the middle, so the ridge doesn't start at 0
	float top_start = top_end - ridge_width;

	float bottom_start = 0.5f + mid_width / 2;
	float bottom_end = bottom_start + ridge_width;

	//preallocate, temporary DEM
	glm::vec3* verts = nullptr;
	verts = new glm::vec3[(int)resolution.x * (int)resolution.y];

	//the size of one step in percents. used later in calculations
	glm::vec2 steps;
	steps.x = 1 / resolution.x;
	steps.y = 1 / resolution.y;

	//how high above real terrain is high point and how low the low point (actual terrain height is 100%, so 101% height converts to 1% highER than base terrain and 99% would convert to 1% lowER than base terrain)
	float high_dif = peak_height - 1.f;
	float low_dif = 1.0f - mid_low;

	int numpoints = 0;
	for (int i = 0; i < resolution.y; i++)
	{
		for (int j = 0; j < resolution.x; j++)
		{
			//calculate what this current y value maps to if described in percentages
			float cur_y = i * steps.y;

			//calculate the array index of given vert
			int cur_index = i * (int)resolution.x + j;
			//set the heigth(y) to 1 and x and z to respective values
			verts[cur_index] = glm::vec3(i, 1.f, j);//need to flip x and z for sphere to render it properly

			//shift height based on within which area the point falls into
			if (cur_y >= top_start && cur_y <= top_end)
			{//if falls within first peak area
				float range_size = (top_end - top_start);
				if ((cur_y - top_start) > range_size / 2)
				{//peak falling
					verts[cur_index].y = (range_size - (cur_y - top_start)) / range_size * peak_height * 2;//need to multiply by 2, because otherwise it would go up to half the value (since it goes only up to half the width)
				}
				else
				{//peak rising
					verts[cur_index].y = (cur_y - top_start) / range_size * peak_height * 2;
				}
				verts[cur_index].y *= high_dif;
				verts[cur_index].y += 1.f;
				numpoints++;
			}
			else if (cur_y > top_end && cur_y < bottom_start)
			{//the trough
				float range_size = (bottom_start - top_end);
				if ((cur_y - top_end) > range_size / 2)
				{//height falling
					verts[cur_index].y = 1.f - ((top_end + range_size - cur_y) / range_size) * low_dif * 2;
				}
				else
				{//height increasing
					verts[cur_index].y = 1.f - ((cur_y - top_end) / range_size) * low_dif * 2;
				}
				numpoints++;
			}
			else if (cur_y >= bottom_start && cur_y <= bottom_end)
			{//the second peak
				float range_size = (bottom_end - bottom_start);
				if ((cur_y - bottom_start) > range_size / 2)
				{//height falling
					verts[cur_index].y = (range_size - (cur_y - bottom_start)) / range_size * peak_height * 2;
				}
				else
				{//height increasing
					verts[cur_index].y = (cur_y - bottom_start) / range_size * peak_height * 2;
				}
				verts[cur_index].y *= high_dif;
				verts[cur_index].y += 1.f;
				numpoints++;
			}
			else//if vert unused, mark it with -999 so will be skipped in next step
				verts[cur_index] = glm::vec3(-999, -999, -999);
		}
		//std::cout << verts[(int)(i * resolution.y) - 1].y << std::endl;
	}

	//add all generated points to return vector (excluding the marked ones that are unused)
	std::vector<glm::vec3> fin_verts;
	for (int i = 0; i < resolution.y*resolution.x; i++)
		if (verts[i].x != -999)
		{
			fin_verts.push_back(verts[i]);
		}

	delete verts;
	return fin_verts;
}

std::vector<glm::vec3>  FeatureGenerator::generate_crater(glm::vec2 resolution, float height, float radius)
{
	glm::vec3* verts = nullptr;
	verts = new glm::vec3[(int)resolution.x * (int)resolution.y];

	glm::vec2 center = glm::vec2(std::round(resolution.x / 2), std::round(resolution.y / 2));

	float step_x = 1 / resolution.x;
	float step_y = 1 / resolution.y;

	for(int i = 0; i < resolution.y; i++)
		for (int j = 0; j < resolution.x; j++)
		{
			//get distance from center
			float dist_from_center_x = abs(j*step_x - 0.5f);
			float dist_from_center_y = abs(i*step_y - 0.5f);
			int id = i * (int)resolution.x + j;

			//if distance from center greater than radius, mark point as ignored, if less than radius, set it to the height value
			verts[id] = glm::vec3(j,1.f,i);
			float dist_from_center = std::sqrt(pow(dist_from_center_x,2) + pow(dist_from_center_y,2));
			if (dist_from_center < radius)
				verts[id].y = height;
			else
				verts[id].x = -999;
		}

	std::vector<glm::vec3> fin_verts;
	for (int i = 0; i < resolution.y*resolution.x; i++)
		if (verts[i].x != -999)
		{
			fin_verts.push_back(verts[i]);
		}

	delete verts;

	return fin_verts;
}

FeatureGenerator::FeatureGenerator()
{
}


FeatureGenerator::~FeatureGenerator()
{
}
