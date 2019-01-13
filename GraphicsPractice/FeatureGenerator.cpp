/*	by Aleksejs Loginovs - November 2018
	generates specific terrain features e.g. ridges or craters
*/

#include "FeatureGenerator.h"



std::vector<glm::vec3> FeatureGenerator::generate_ridge(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width)
{
	float mid_width = 0.2f*ridge_width;//2 times smaller than ridge
	ridge_width = 0.4f*ridge_width;//max width can be only 0.4 of whole as it's double ridge
	mid_low = 0.2f*mid_low;	

	float ridge_peak_close = 0.5f;

	float top_end = 0.5f - mid_width / 2;
	float top_start = top_end - ridge_width;
	
	float bottom_start = 0.5f + mid_width / 2;
	float bottom_end = bottom_start + ridge_width;

	glm::vec3* verts = nullptr;
	verts = new glm::vec3[(int)resolution.x * (int)resolution.y];

	glm::vec2 steps;
	steps.x = 1 / resolution.x;
	steps.y = 1 / resolution.y;

	int numpoints = 0;
	for (int i = 0; i < resolution.y; i++)
	{
		for (int j = 0; j < resolution.x; j++)
		{
			float cur_y = i * steps.y;
			int cur_index = i * (int)resolution.x + j;
			verts[cur_index] = glm::vec3(j,1.f,i);
			if (cur_y > top_start && cur_y < top_end)
			{//the first peak
				float range_size = (top_end - top_start);
				if ((cur_y - top_start) > range_size*ridge_peak_close)
				{//peak falling
					verts[cur_index].y = (range_size - (cur_y - top_start)) / range_size * peak_height / ridge_peak_close;
				}
				else
				{//peak rising
					verts[cur_index].y =  (cur_y - top_start) / range_size * peak_height / ridge_peak_close;
				}
				numpoints++;
			}
			else if (cur_y > top_end && cur_y < bottom_start)
			{//the trough
				float range_size = (bottom_start - top_end);
				if ((cur_y - top_end) > range_size*0.5)
				{//height falling
					verts[cur_index].y = (range_size - (cur_y - top_end)) / range_size * mid_low * 2;
				}
				else
				{//height increasing
					verts[cur_index].y = (cur_y - top_end) / range_size * mid_low * 2;
				}
				numpoints++;
			}
			else if(cur_y > bottom_start && cur_y < bottom_end)
			{//the second peak
				float range_size = (bottom_end - bottom_start);
				if ((cur_y - bottom_start) > range_size*ridge_peak_close)
				{//height falling
					verts[cur_index].y = (range_size - (cur_y - bottom_start)) / range_size * peak_height / ridge_peak_close;
				}
				else
				{//height increasing
					verts[cur_index].y = (cur_y - bottom_start) / range_size * peak_height / ridge_peak_close;
				}
				numpoints++;
			}
			else//if vert unused, mark it with -999 so will be skipped in next step
				verts[cur_index] = glm::vec3(-999,-999,-999);
		}
	}

	//add all generated points to return vector (excluding the marked ones that are unused)
	std::vector<glm::vec3> fin_verts;
	for(int i = 0; i < resolution.y*resolution.x;i++)
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

std::vector<glm::vec3> FeatureGenerator::generate_ridge_sphere(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width)
{
	float mid_width = 0.2f*ridge_width;//2 times smaller than ridge
	ridge_width = 0.4f*ridge_width;//max width can be only 0.4 of whole as it's double ridge
	mid_low = 0.2f*mid_low;

	float ridge_peak_close = 0.5f;

	float top_end = 0.5f - mid_width / 2;
	float top_start = top_end - ridge_width;

	float bottom_start = 0.5f + mid_width / 2;
	float bottom_end = bottom_start + ridge_width;

	glm::vec3* verts = nullptr;
	verts = new glm::vec3[(int)resolution.x * (int)resolution.y];

	glm::vec2 steps;
	steps.x = 1 / resolution.x;
	steps.y = 1 / resolution.y;

	float high_dif = peak_height - 1.f;
	float low_dif = mid_low - 1.f;

	int numpoints = 0;
	for (int i = 0; i < resolution.y; i++)
	{
		for (int j = 0; j < resolution.x; j++)
		{
			float cur_y = i * steps.y;
			int cur_index = i * (int)resolution.x + j;
			verts[cur_index] = glm::vec3(j, 1.f, i-resolution.x/2);
			if (cur_y > top_start && cur_y < top_end)
			{//the first peak
				float range_size = (top_end - top_start);
				if ((cur_y - top_start) > range_size*ridge_peak_close)
				{//peak falling
					verts[cur_index].y = (range_size - (cur_y - top_start)) / range_size * peak_height / ridge_peak_close;
				}
				else
				{//peak rising
					verts[cur_index].y = (cur_y - top_start) / range_size * peak_height / ridge_peak_close;
				}
				verts[cur_index].y *= high_dif;
				verts[cur_index].y += 1.f;
				numpoints++;
			}
			else if (cur_y > top_end && cur_y < bottom_start)
			{//the trough
				float range_size = (bottom_start - top_end);
				if ((cur_y - top_end) > range_size*0.5)
				{//height falling
					verts[cur_index].y = (range_size - (cur_y - top_end)) / range_size * mid_low * 2;
				}
				else
				{//height increasing
					verts[cur_index].y = (cur_y - top_end) / range_size * mid_low * 2;
				}
				verts[cur_index].y *= low_dif;
				verts[cur_index].y += 1.f;
				numpoints++;
			}
			else if (cur_y > bottom_start && cur_y < bottom_end)
			{//the second peak
				float range_size = (bottom_end - bottom_start);
				if ((cur_y - bottom_start) > range_size*ridge_peak_close)
				{//height falling
					verts[cur_index].y = (range_size - (cur_y - bottom_start)) / range_size * peak_height / ridge_peak_close;
				}
				else
				{//height increasing
					verts[cur_index].y = (cur_y - bottom_start) / range_size * peak_height / ridge_peak_close;
				}
				verts[cur_index].y *= high_dif;
				verts[cur_index].y += 1.f;
				numpoints++;
			}
			else//if vert unused, mark it with -999 so will be skipped in next step
				verts[cur_index] = glm::vec3(-999, -999, -999);
		}
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

FeatureGenerator::FeatureGenerator()
{
}


FeatureGenerator::~FeatureGenerator()
{
}
