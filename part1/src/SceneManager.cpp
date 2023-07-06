#include "SceneManager.hpp"
#include <glm/common.hpp>

#define PI_F 3.14159265358979f

SceneManager::SceneManager(int wind_width, int wind_height, scene_container* scene, ShaderManager* shaderMng)
{
	this->wind_width = wind_width;
	this->wind_height = wind_height;
	this->scene = scene;
	this->position = scene->scene.camera_pos;
    this->shaderMng = shaderMng;
}

void SceneManager::init(){
    init_buffers();
}

void SceneManager::update(float deltaTime)
{
	update_scene(deltaTime);
	update_buffers();
}

void SceneManager::updateMouse(float newX, float newY){
	if (firstMouse){
		lastX = newX;
		lastY = newY;
		firstMouse = false;
	}

	float xoffset = newX - lastX;
	float yoffset = lastY - newY;
	lastX = newX;
	lastY = newY;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

void SceneManager::update_scene(float deltaTime)
{
	front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	right = glm::normalize(glm::cross(-front, world_up));
	scene->scene.quat_camera_rotation = glm::quat(glm::vec3(glm::radians(-pitch), glm::radians(yaw), 0));

	float speed = deltaTime * 0.003;
	
	if (up_pressed) {
		position += front * speed;
	} else if (down_pressed){
		position -= front * speed;
	} else if (left_pressed){
		position -= right * speed;
	} else if (right_pressed){
		position += right * speed;
	}
	scene->scene.camera_pos = position;
}

rt_material SceneManager::create_material(glm::vec3 color, int specular, float reflect, float refract, glm::vec3 absorb, float diffuse, float kd, float ks){
	rt_material material = {};

	material.color = color;
	material.absorb = absorb;
	material.specular = specular;
	material.reflect = reflect;
	material.refract = refract;
	material.diffuse = diffuse;
	material.kd = kd;
	material.ks = ks;

	return material;
}

rt_sphere SceneManager::create_sphere(glm::vec3 center, float radius, rt_material material, bool hollow)
{
	rt_sphere sphere = {};
	sphere.obj = glm::vec4(center, radius);
	sphere.hollow = hollow;
	sphere.material = material;

	return sphere;
}

rt_light_point SceneManager::create_light_point(glm::vec4 position, glm::vec3 color, float intensity, float linear_k,
	float quadratic_k) {
	rt_light_point light = {};

	light.intensity = intensity;
	light.pos = position;
	light.color = color;
	light.linear_k = linear_k;
	light.quadratic_k = quadratic_k;

	return light;
}

rt_light_direct SceneManager::create_light_direct(glm::vec3 direction, glm::vec3 color, float intensity) {
	rt_light_direct light = {};

	light.intensity = intensity;
	light.direction = direction;
	light.color = color;

	return light;
}

rt_scene SceneManager::create_scene(int width, int height){
	rt_scene scene = {};

	scene.camera_pos = {};
	scene.canvas_height = height;
	scene.canvas_width = width;
	scene.bg_color = { 0,0.0,0.0 };
	scene.reflect_depth = 5;

	return scene;
}


template<typename T>
void SceneManager::init_buffer(GLuint* ubo, const char* name, int bindingPoint, std::vector<T>& v) {
	shaderMng->init_buffer(ubo, name, bindingPoint, sizeof(T) * v.size(), v.data());
}

void SceneManager::init_buffers() {
	shaderMng->init_buffer(&sceneUbo, "scene_buf", 0, sizeof(rt_scene), nullptr);
	init_buffer(&sphereUbo, "spheres_buf", 1, scene->spheres);
	init_buffer(&lightPointUbo, "lights_point_buf", 2, scene->lights_point);
	init_buffer(&lightDirectUbo, "lights_direct_buf", 3, scene->lights_direct);
}


template<typename T>
void SceneManager::update_buffer(GLuint ubo, std::vector<T>& v) const
{
	if (!v.empty())
	{
		shaderMng->update_buffer(ubo, sizeof(T) * v.size(), v.data());
	}
}

void SceneManager::update_buffers() const
{
	shaderMng->update_buffer(sceneUbo, sizeof(rt_scene), &scene->scene);
	update_buffer(sphereUbo, scene->spheres);
	update_buffer(lightPointUbo, scene->lights_point);
}



glm::vec3 SceneManager::get_color(float r, float g, float b)
{
	return glm::vec3(r / 255, g / 255, b / 255);
}
