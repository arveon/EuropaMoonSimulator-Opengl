/* by Aleksejs Loginovs - October 2018 */

#include "Lightsource.h"



Lightsource::Lightsource(Shader light_shader)
{
	sphere = Sphere(light_shader);
	sphere.makeSphere(20, 20);
	light_position = glm::vec4(-5, 0, -20, 1);
	scale = glm::vec3(0.1, 0.1, 0.1);
	sphere.normals_enabled = false;
	sphere.set_draw_normals(false);
}

Lightsource::~Lightsource()
{
}

void Lightsource::draw()
{
	sphere.translate(light_position);
	sphere.scale(scale);
	sphere.drawSphere(0);
}

void Lightsource::set_view_matrix(glm::mat4 mat)
{
	sphere.set_view_matrix(mat);
}

void Lightsource::set_model_matrix(glm::mat4 mat)
{
	sphere.set_model_matrix(mat);
}

void Lightsource::move_to(glm::vec4 pos)
{
	this->light_position = pos;
}

void Lightsource::shift(glm::vec3 pos)
{
	light_position.x += pos.x;
	light_position.y += pos.y;
	light_position.z += pos.z;
}
