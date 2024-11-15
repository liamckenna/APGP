#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include "transform.h"
#include "vertex.h"
#include "mesh.h"
#include "camera.h"
#include "scene.h"
#include "mouse.h"
#include "light.h"
#include "input.h"
#include "user.h"
#include "shaders.h"
#include "json.h"
#include "object_parser.h"
#include <vector>

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1440;

//Uniform IDs

float last_time = 0.f;
float last_fps_time = 0.f;
int frame_count = 0;
float delta_time = 0.f;

//forward declarations of functions
static User* InitializeUser();
static void ProgramInitialization(User*& user);
std::string LoadShader(const char* filepath);
GLuint createShader(GLenum type, const char* shaderSource);
static void error_callback(int error, const char* description);
static Mouse* InitializeMouse(GLFWwindow*& window);
static void MouseCallback(User*& user, Scene*& scene);
void CameraInitialization(Camera*& camera);
static bool ShaderInitialization(Scene*& scene, nlohmann::json data);
static Scene* SceneGeneration(std::string file);
static void BufferArrayInitialization(Scene*& scene);
static void ProcessInput(User*& user, Scene*& scene);
void CalculateFrameRate();
void CalculateDeltaTime();
static void DebugPrinting(GLuint& shader_program, GLuint& vertex_array, GLFWwindow*& window, Camera*& camera, Mesh*& mesh);
static glm::mat4 CalculateMVP(Camera*& camera, Mesh*& mesh);
static void RenderScene(User*& user, Scene*& scene);
static void Cleanup(User*& user, Scene*& scene);
static Input* InitializeInput();
static void CameraGeneration(Scene*& scene, nlohmann::json data);
static void LightGeneration(Scene*& scene, nlohmann::json data);
static void MeshGeneration(Scene*& scene, nlohmann::json data);
static void ObjectGeneration(Scene*& scene, nlohmann::json data);
static void UpdateCameraUniforms(Camera*& camera);
static void UpdateLightUniforms(Scene*& scene);
static void UpdateUniforms(Scene*& scene, Camera*& camera, Mesh*& mesh);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void DefaultMaterialGeneration(Scene*& scene, nlohmann::json data);
static void UpdateTextureUniforms(Scene*& scene);
static void AttachCamera(Scene*& scene, Object* new_parent);
static void Launch(Scene*& scene, Object* object);
static void ProcessPhysics(Scene*& scene);

glm::vec3 gravity;
glm::vec3 launch_vector;
float launch_force;

int main() {
	User* user;
	Scene* scene;
	
	user = InitializeUser();

	std::cout << "initialized program" << std::endl;

	scene = SceneGeneration("data/jsons/scene.json");
	scene->user = user;

	std::cout << "initialized scene" << std::endl;

	glfwSetWindowUserPointer(user->window, scene);

	nlohmann::json data = ReadJsonFromFile("data/jsons/scene.json");
	if (!ShaderInitialization(scene, data)) {
		std::cout << "uh oh" << std::endl;
		return -1;
	}

	std::cout << "initialized shader" << std::endl;

	BufferArrayInitialization(scene);

	std::cout << "initialized buffers and arrays" << std::endl;

	UpdateTextureUniforms(scene);

	std::cout << "initialized textures" << std::endl;

	scene->PrintObjectTrees();

	RenderScene(user, scene);

	std::cout << "finished rendering scene" << std::endl;

	Cleanup(user, scene);

	std::cout << "finished cleanup" << std::endl;

	exit(EXIT_SUCCESS);
}

static User* InitializeUser() {
	User* user = new User();
	user->input = new Input();

	ProgramInitialization(user);

	user->input->mouse = InitializeMouse(user->window);

	return user;
	
}

static void ProgramInitialization(User*& user) {
	glfwSetErrorCallback(error_callback);
	
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16);


	user->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Epic Style", NULL, NULL);
	if (!user->window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(user->window);


	//glewExperimental = GL_TRUE;


	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwSetScrollCallback(user->window, scroll_callback);


	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	glfwSwapInterval(0);
	glfwSetInputMode(user->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glLineWidth(2.0f);
}

std::string LoadShader(const char* filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open shader file: " << filepath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

GLuint createShader(GLenum type, const char* shaderSource) {
	GLuint shader = glCreateShader(type);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error occurred: " << error << std::endl;
	}
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static Mouse* InitializeMouse(GLFWwindow*& window) {
	Mouse* mouse = new Mouse(SCR_WIDTH, SCR_HEIGHT, window, 3.f);
	return mouse;
}

static void MouseCallback(User*& user, Scene*& scene)
{
	user->input->mouse->Update();
	
	user->input->mouse->offset_x *= user->input->mouse->sensitivity * delta_time / SCR_WIDTH 	* 100000.f;
	user->input->mouse->offset_y *= user->input->mouse->sensitivity * delta_time / SCR_HEIGHT 	* 100000.f;

	
	scene->GetObjectByName("camera shell")->t->local.RotateYaw(user->input->mouse->offset_x);
	scene->GetObjectByName("camera shell")->t->local.RotatePitch(user->input->mouse->offset_y);
	scene->GetObjectByName("camera shell")->UpdateTree();
}

static bool ProcessShader(Scene*& scene, std::string file, SHADER_TYPE TYPE) {
		
	std::string src = LoadShader(file.c_str());
	if (src.empty()) {
		std::cerr << "Shader source loading failed." << std::endl;
		return false;
	}
	GLuint ss;
	if (TYPE == SHADER_TYPE::VERTEX) {
		ss = createShader(GL_VERTEX_SHADER, src.c_str());
		Shader s = Shader(src, ss, VERTEX);
		scene->shaders->vertex_shaders.push_back(s);
	} else if (TYPE == SHADER_TYPE::GEOMETRY) {
		ss = createShader(GL_GEOMETRY_SHADER, src.c_str());
		Shader s = Shader(src, ss, GEOMETRY);
		scene->shaders->geometry_shaders.push_back(s);
	} else if (TYPE == SHADER_TYPE::FRAGMENT) {
		ss = createShader(GL_FRAGMENT_SHADER, src.c_str());
		Shader s = Shader(src, ss, FRAGMENT);
		scene->shaders->fragment_shaders.push_back(s);
	}
	glAttachShader(scene->shaders->shader_program, ss);
	glDeleteShader(ss);
	return true;
}

static bool ShaderInitialization(Scene*& scene, nlohmann::json data) {
	scene->shaders->shader_program = glCreateProgram();
	for (int i = 0; i < data["shaders"]["vertex_shaders"].size(); i++) {
		std::string fn = "shaders/" + std::string(data["shaders"]["vertex_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, VERTEX)) return false;
	}
	for (int i = 0; i < data["shaders"]["geometry_shaders"].size(); i++) {
		std::cout << "tryingto call geom shaders" << std::endl;
		std::string fn = "shaders/" + std::string(data["shaders"]["geometry_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, GEOMETRY)) return false;
	}
	for (int i = 0; i < data["shaders"]["fragment_shaders"].size(); i++) {
		std::string fn = "shaders/" + std::string(data["shaders"]["fragment_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, FRAGMENT)) return false;
	}
	glLinkProgram(scene->shaders->shader_program);
	int success;
	char infoLog[512];
	glGetProgramiv(scene->shaders->shader_program, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(scene->shaders->shader_program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}




	scene->user->matrix_id = glGetUniformLocation(scene->shaders->shader_program, "MVP");
	scene->user->view_matrix_id = glGetUniformLocation(scene->shaders->shader_program, "V");
	scene->user->model_matrix_id = glGetUniformLocation(scene->shaders->shader_program, "M");
	scene->user->triangle_color_id = glGetUniformLocation(scene->shaders->shader_program, "triangle_colors");
	scene->user->triangle_normal_id = glGetUniformLocation(scene->shaders->shader_program, "triangle_normals");
	scene->user->camera_position_id = glGetUniformLocation(scene->shaders->shader_program, "camera_position");
	scene->user->shading_mode_id = glGetUniformLocation(scene->shaders->shader_program, "shading_mode");
	scene->user->phong_exponent_id = glGetUniformLocation(scene->shaders->shader_program, "phong_exponent");
	scene->user->light_count_id = glGetUniformLocation(scene->shaders->shader_program, "light_count");
	scene->user->light_position_id = glGetUniformLocation(scene->shaders->shader_program, "lights[0].position");
	scene->user->light_strength_id = glGetUniformLocation(scene->shaders->shader_program, "lights[0].strength");
	scene->user->light_color_id = glGetUniformLocation(scene->shaders->shader_program, "lights[0].color");
	scene->user->light_active_id = glGetUniformLocation(scene->shaders->shader_program, "lights[0].active");
	scene->user->ambient_intensity_id = glGetUniformLocation(scene->shaders->shader_program, "ambient_intensity");
	scene->user->textures_id = glGetUniformLocation(scene->shaders->shader_program, "textures");

	return true;
}

static Scene* SceneGeneration(std::string file) {
	nlohmann::json data = ReadJsonFromFile(file);
	
	Scene* scene = new Scene();
	scene->name = data["name"];
	if (data.contains("default_draw_mode")) scene->SetDefaultDrawMode(data["default_draw_mode"]);
	else scene->SetDefaultDrawMode("GL_TRIANGLES");
	if (data.contains("shading_mode")) scene->shading_mode = data["shading_mode"];
	else scene->shading_mode = 3;
	launch_force = data["launch_force"];
	DefaultMaterialGeneration(scene, data);
	
	CameraGeneration(scene, data);
	LightGeneration(scene, data);
	MeshGeneration(scene, data);
	ObjectGeneration(scene, data);
	scene->SetHeldObject(scene->GetObjectByName("camera shell"));
	scene->buffers = new Buffers();
	scene->shaders = new Shaders();
	std::cout << "scene generation completed" << std::endl;
	return scene;
}

static void DefaultMaterialGeneration(Scene*& scene, nlohmann::json data) {
	Material* mtl = new Material();
	mtl->name = data["default_material"]["name"];
	mtl->index = 0;
	mtl->colors.dif = glm::vec3(data["default_material"]["dif"][0], data["default_material"]["dif"][1], data["default_material"]["dif"][2]); 
	mtl->colors.amb = glm::vec3(data["default_material"]["amb"][0], data["default_material"]["amb"][1], data["default_material"]["amb"][2]); 
	mtl->colors.spc = glm::vec3(data["default_material"]["spc"][0], data["default_material"]["spc"][1], data["default_material"]["spc"][2]);
	mtl->colors.ems = glm::vec3(data["default_material"]["ems"][0], data["default_material"]["ems"][1], data["default_material"]["ems"][2]);
	mtl->shininess 	= data["default_material"]["shininess"];
	mtl->glossiness = data["default_material"]["glossiness"];
	mtl->opacity 	= data["default_material"]["opacity"];
	scene->default_material = mtl;
	scene->current_material = scene->default_material;
	scene->materials.push_back(mtl);
	std::cout << "default material generation completed" << std::endl;
}

static void MeshGeneration(Scene*& scene, nlohmann::json data) {
	for (int i = 0; i < data["meshes"].size(); i++) {
		glm::vec3 pos = glm::vec3(	data["meshes"][i]["transform"]["position"][0],
									data["meshes"][i]["transform"]["position"][1],
									data["meshes"][i]["transform"]["position"][2]);
		glm::vec3 rot = glm::vec3(	data["meshes"][i]["transform"]["rotation"][0],
									data["meshes"][i]["transform"]["rotation"][1],
									data["meshes"][i]["transform"]["rotation"][2]);
		glm::vec3 scl = glm::vec3(	data["meshes"][i]["transform"]["scale"][0],
									data["meshes"][i]["transform"]["scale"][1],
									data["meshes"][i]["transform"]["scale"][2]);
		std::string fn = "data/objects/" + std::string(data["meshes"][i]["file"]);
		Mesh* m = new Mesh();
		m->visible = data["meshes"][i]["visible"];
		m->current_scene = scene;
		m->t = new Transform(pos, rot, scl);
		m->name = data["meshes"][i]["name"];
		m->verbose = data["meshes"][i]["verbose"];
		m->idx = i;
		if (data["meshes"][i].contains("draw_mode")) m->SetDrawMode(data["meshes"][i]["draw_mode"]);
		else m->SetDefaultDrawMode();
		if (data["meshes"][i].contains("dif_only")) {
			m->dif_only = data["meshes"][i]["dif_only"];
		} else m->dif_only = false;
		ObjectParser::ParseObjFile(fn, m);
		scene->meshes.push_back(m);
		std::cout << m->name << " mesh completed" << std::endl;
	}
	std::cout << "mesh generation completed" << std::endl;
}

static void ObjectGeneration(Scene*& scene, nlohmann::json data) {
	for (int i = 0; i < data["objects"].size(); i++) {
		glm::vec3 pos = glm::vec3(	data["objects"][i]["transform"]["position"][0],
									data["objects"][i]["transform"]["position"][1],
									data["objects"][i]["transform"]["position"][2]);
		glm::vec3 rot = glm::vec3(	data["objects"][i]["transform"]["rotation"][0],
									data["objects"][i]["transform"]["rotation"][1],
									data["objects"][i]["transform"]["rotation"][2]);
		glm::vec3 scl = glm::vec3(	data["objects"][i]["transform"]["scale"][0],
									data["objects"][i]["transform"]["scale"][1],
									data["objects"][i]["transform"]["scale"][2]);
		
		Object* o = new Object();
		o->current_scene = scene;
		o->t = new Transform(pos, rot, scl);
		o->name = data["objects"][i]["name"];
		if (data["objects"][i].contains("children")) {
			for (int j = 0; j < data["objects"][i]["children"].size(); j++) {
				if (data["objects"][i]["children"][j]["type"] == "object") 		o->AttachChild(dynamic_cast<Object*>(scene->GetObjectByName(data["objects"][i]["children"][j]["name"])));
				else if (data["objects"][i]["children"][j]["type"] == "mesh") 	o->AttachChild(dynamic_cast<Object*>(scene->GetMeshByName(	data["objects"][i]["children"][j]["name"])));
				else if (data["objects"][i]["children"][j]["type"] == "camera") o->AttachChild(dynamic_cast<Object*>(scene->GetCameraByName(data["objects"][i]["children"][j]["name"])));
				else if (data["objects"][i]["children"][j]["type"] == "light") 	o->AttachChild(dynamic_cast<Object*>(scene->GetLightByName(	data["objects"][i]["children"][j]["name"])));
			}
		}
		scene->objects.push_back(o);
		std::cout << o->name << " object completed" << std::endl;
	}
	std::cout << "object generation completed" << std::endl;
}

static void CameraGeneration(Scene*& scene, nlohmann::json data) {
	for (int i = 0; i < data["cameras"].size(); i++) {
		glm::vec3 pos = glm::vec3(	data["cameras"][i]["transform"]["position"][0],
									data["cameras"][i]["transform"]["position"][1],
									data["cameras"][i]["transform"]["position"][2]);
		glm::vec3 rot = glm::vec3(	data["cameras"][i]["transform"]["rotation"][0],
									data["cameras"][i]["transform"]["rotation"][1],
									data["cameras"][i]["transform"]["rotation"][2]);
		glm::vec3 scl = glm::vec3(	data["cameras"][i]["transform"]["scale"][0],
									data["cameras"][i]["transform"]["scale"][1],
									data["cameras"][i]["transform"]["scale"][2]);
		Transform* t = new Transform(pos, rot, scl);
		byte projection_type;
		if (data["cameras"][i]["projection_type"] == "PERSPECTIVE") projection_type = PERSPECTIVE;
		else if (data["cameras"][i]["projection_type"] == "ORTHOGRAPHIC") projection_type = ORTHOGRAPHIC;
		Camera* c = new Camera(t,	data["cameras"][i]["velocity"], projection_type, data["cameras"][i]["fov"],
									data["cameras"][i]["x_range"], data["cameras"][i]["y_range"], data["cameras"][i]["z_near"],
									data["cameras"][i]["z_far"], data["cameras"][i]["sensitivity"]);
		c->name = data["cameras"][i]["name"]; //TODO: need unique camera values, not just by name
		c->active = data["cameras"][i]["active"]; 
		c->current_scene = scene;
		scene->cameras.push_back(c);
		std::cout << c->name << " camera completed" << std::endl;

	}
	int active_camera = data["active_camera"];
	scene->main_camera = scene->cameras[active_camera];
	std::cout << "camera generation completed" << std::endl;
}

static void LightGeneration(Scene*& scene, nlohmann::json data) {
	scene->ambient_intensity = data["ambient_intensity"];
	for (int i = 0; i < data["lights"].size(); i++) {
		glm::vec3 pos = glm::vec3(	data["lights"][i]["transform"]["position"][0],
									data["lights"][i]["transform"]["position"][1],
									data["lights"][i]["transform"]["position"][2]);
		glm::vec3 rot = glm::vec3(	data["lights"][i]["transform"]["rotation"][0],
									data["lights"][i]["transform"]["rotation"][1],
									data["lights"][i]["transform"]["rotation"][2]);
		glm::vec3 scl = glm::vec3(	data["lights"][i]["transform"]["scale"][0],
									data["lights"][i]["transform"]["scale"][1],
									data["lights"][i]["transform"]["scale"][2]);
		Transform* t = new Transform(pos, rot, scl);
		Color c = Color(data["lights"][i]["color"][0], data["lights"][i]["color"][1],
						data["lights"][i]["color"][2], 255);
		Light* l = new Light(t, data["lights"][i]["strength"], c, data["lights"][i]["active"]);
		l->name = data["lights"][i]["name"];
		scene->lights.push_back(l);
		std::cout << l->name << " light completed" << std::endl;
	}
	std::cout << "light generation completed" << std::endl;
}

static void BufferArrayInitialization(Scene*& scene) {

	glGenBuffers(1, &scene->buffers->light_uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, scene->buffers->light_uniform_buffer);
	std::vector<FlattenedLight> flattenedLights = scene->flattenLights();
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FlattenedLight) * scene->lights.size(), flattenedLights.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, scene->buffers->light_binding_point, scene->buffers->light_uniform_buffer);

	glGenBuffers(1, &scene->buffers->shader_uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, scene->buffers->shader_uniform_buffer);
	std::vector<FlattenedMaterial> flattened_materials = scene->flattenMaterials();
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FlattenedMaterial) * flattened_materials.size(), flattened_materials.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, scene->buffers->shader_uniform_buffer);

	for (int i = 0; i < scene->meshes.size(); i++) {
		scene->buffers->GenerateBuffers();
		std::vector<FlattenedVertex> flattened_vert_array = scene->meshes[i]->flattenVertices();
		glGenVertexArrays(1, &scene->buffers->vertex_arrays[i]);
		glBindVertexArray(scene->buffers->vertex_arrays[i]);

		glGenBuffers(1, &scene->buffers->vertex_buffers[i]);
		glBindBuffer(GL_ARRAY_BUFFER, scene->buffers->vertex_buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, flattened_vert_array.size() * sizeof(FlattenedVertex), flattened_vert_array.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, color));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, tex_coord));
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(4, 1, GL_INT, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, material_index));
		glEnableVertexAttribArray(4);
		
	}
	std::cout << "buffer array generation completed" << std::endl;
}

void CalculateFrameRate() {
	float current_time = glfwGetTime();
	frame_count++;

	if (current_time - last_fps_time >= 1.0) {
		double fps = frame_count / (current_time - last_fps_time);
		std::cout << "FPS: " << fps << std::endl;
		last_fps_time = current_time;
		frame_count = 0;
	}
}
	//TODO: Merge these into one function i guess
void CalculateDeltaTime() {
	float current_time = glfwGetTime();
	delta_time = current_time - last_time;
	last_time = current_time;
}

static void DebugPrinting(GLuint& shader_program, GLuint& vertex_array, GLFWwindow*& window, Camera*& camera, Mesh*& mesh) {

	// std::cout << "x: " << camera->t->local.pos_x << std::endl;
	// std::cout << "y: " << camera->t->local.pos_y << std::endl;
	// std::cout << "z: " << camera->t->local.pos_z << std::endl;
	// std::cout << "fwd: " << glm::to_string(camera->t->local.fwd) << std::endl;
	// std::cout << "rt: " << glm::to_string(camera->t->local.rt) << std::endl;
	// std::cout << "up: " << glm::to_string(camera->t->local.up) << std::endl;
	//std::cout << "mesh pos: " << glm::to_string(mesh->vertices[0]->t->pos) << std::endl;
}

static void UpdateCameraUniforms(Camera*& camera) {
	glUniformMatrix4fv(camera->current_scene->user->view_matrix_id, 1, GL_FALSE, &camera->view[0][0]);
	glUniform3f(camera->current_scene->user->camera_position_id, camera->t->global.pos[0], camera->t->global.pos[1], camera->t->global.pos[2]);
	
}

static void UpdateLightUniforms(Scene*& scene) {
	scene->UpdateLights();
	glUniform1i(scene->user->light_count_id, scene->lights.size());
	std::vector<FlattenedLight> flattenedLights = scene->flattenLights();
	
	glBindBuffer(GL_UNIFORM_BUFFER, scene->buffers->light_uniform_buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FlattenedLight) * flattenedLights.size(), flattenedLights.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniform1i(scene->user->shading_mode_id, scene->shading_mode);
	glUniform1i(scene->user->phong_exponent_id, 20);
	glUniform1f(scene->user->ambient_intensity_id, scene->ambient_intensity);
}

static void UpdateTextureUniforms(Scene*& scene) {
	for (size_t i = 0; i < scene->textures.size(); ++i) {
		GLenum texture_unit = GL_TEXTURE0 + i;
		scene->textures[i]->Bind(texture_unit);
		glUniform1i(scene->user->textures_id + i, i);
	}
}

static void UpdateUniforms(Scene*& scene, Camera*& camera) {
	
	UpdateCameraUniforms(camera);
	UpdateLightUniforms(scene);
	UpdateTextureUniforms(scene);
}

static void RenderScene(User*& user, Scene*& scene) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	do
	{
		CalculateFrameRate();

		CalculateDeltaTime();

		ProcessPhysics(scene); //comment out if running personal scene

		ProcessInput(user, scene);

		MouseCallback(user, scene);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(scene->shaders->shader_program);
		
		scene->UpdateObjectTrees(true);


		UpdateLightUniforms(scene);
		
		scene->DrawObjectTrees();

		glfwSwapBuffers(user->window);

		glfwPollEvents();
		
	} while (!glfwWindowShouldClose(user->window));
}

static void Cleanup(User*& user, Scene*& scene) {
	scene->buffers->CleanupBuffers();
	scene->shaders->CleanupShaders();
	glfwDestroyWindow(user->window);
	glfwTerminate();
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
	Object* camera = scene->GetObjectByName("camera");
	
	if (yoffset != 0 && camera->t->local.pos[2] >= 0) { //scrolling up
		camera->t->local.TranslateForward(dynamic_cast<Camera*>(camera->GetChildByNameTree("camera 1"))->velocity * yoffset * 500.f, delta_time);
		camera->t->UpdateGlobal();
	}
	if (camera->t->local.pos[2] < 0) {
		camera->t->local.SetValue(camera->t->local.pos, glm::vec3(camera->t->local.pos[0], camera->t->local.pos[1], 0));
		camera->t->UpdateGlobal();
	}
	
}

static void AttachCamera(Scene*& scene, Object* new_parent) {
	Object* camera = scene->GetObjectByName("camera shell");
	if (camera == new_parent || camera->parent == new_parent) return;
	camera->t->local.SetValue(camera->t->local.pos, glm::vec3(0.0, 0.0, 0.0));
	camera->t->local.SetQuatValue(camera->t->local.orn, glm::quat(1.0, 0.0, 0.0, 0.0));
	camera->UpdateTree();
	if (new_parent == nullptr) camera->parent->DetatchChild(camera);
	else {
		if (camera->parent != nullptr) camera->parent->DetatchChild(camera);
		new_parent->AttachChild(camera);
	}
}

static void ProcessInput(User*& user, Scene*& scene) {
	if (scene->held_object != nullptr) {
		if (glfwGetKey(user->window, GLFW_KEY_W) == GLFW_PRESS) { //w, translate forward
			if (glfwGetKey(user->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity, delta_time);
			}
		}

		
		if (glfwGetKey(user->window, GLFW_KEY_S) == GLFW_PRESS) { //s, translate backward
			if (glfwGetKey(user->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * -1.f, delta_time);
			}
		}

		if (glfwGetKey(user->window, GLFW_KEY_A) == GLFW_PRESS) { //a, translate left
			if (glfwGetKey(user->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * -1.f, delta_time);
			}
		}

		if (glfwGetKey(user->window, GLFW_KEY_D) == GLFW_PRESS) { //d, translate right
			if (glfwGetKey(user->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity, delta_time);
			}
		}

		if (glfwGetKey(user->window, GLFW_KEY_SPACE) == GLFW_PRESS) { //space, translate up
			if (glfwGetKey(user->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity, delta_time);
			}
		}

		if (glfwGetKey(user->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { //left ctrl, translate down
			if (glfwGetKey(user->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * -1.f, delta_time);
			}
		}
	}
	if (glfwGetKey(user->window, GLFW_KEY_LEFT) == GLFW_PRESS) { //left arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateRight(-5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			scene->held_object->t->local.RotateYaw(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("pen") && glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) { //J4
			scene->held_object->parent->parent->t->local.RotateYaw(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J6
			scene->held_object->t->local.RotateRoll(-45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window, GLFW_KEY_RIGHT) == GLFW_PRESS) { //right arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateRight(5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			scene->held_object->t->local.RotateYaw(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("pen") && glfwGetKey(user->window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) { //J4
			scene->held_object->parent->parent->t->local.RotateYaw(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J6
			scene->held_object->t->local.RotateRoll(45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window, GLFW_KEY_DOWN) == GLFW_PRESS) { //down arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateForward(-5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			//no action required
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			scene->held_object->t->local.RotatePitch(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			scene->held_object->parent->t->local.RotatePitch(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J5
			scene->held_object->parent->t->local.RotatePitch(-45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window, GLFW_KEY_UP) == GLFW_PRESS) { //up arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateForward(5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			//no action required
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			scene->held_object->t->local.RotatePitch(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			scene->held_object->parent->t->local.RotatePitch(45.f * delta_time);
		}  else if (scene->held_object == scene->GetObjectByName("pen")) { //J5
			scene->held_object->parent->t->local.RotatePitch(45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window, GLFW_KEY_COMMA) == GLFW_PRESS) { //comma ","
		
	}
	if (glfwGetKey(user->window, GLFW_KEY_PERIOD) == GLFW_PRESS) { //period "."
		
	}
	if (glfwGetKey(user->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { //escape "esc"
		glfwSetWindowShouldClose(user->window, GL_TRUE);
	}
	//p2
	if (glfwGetKey(user->window, GLFW_KEY_B) == GLFW_PRESS) { //b
		scene->SetHeldObject(scene->GetObjectByName("base"));
	}
	if (glfwGetKey(user->window, GLFW_KEY_T) == GLFW_PRESS) { //t
		scene->SetHeldObject(scene->GetObjectByName("top"));
	}
	
	if (glfwGetKey(user->window, GLFW_KEY_P) == GLFW_PRESS) { //p
		scene->SetHeldObject(scene->GetObjectByName("pen"));
	}
	if (glfwGetKey(user->window, GLFW_KEY_1) == GLFW_PRESS) { //one "1"
		scene->SetHeldObject(scene->GetObjectByName("arm1"));
	}
	if (glfwGetKey(user->window, GLFW_KEY_2) == GLFW_PRESS) { //two "2"
		scene->SetHeldObject(scene->GetObjectByName("arm2"));
	}
	if (glfwGetKey(user->window, GLFW_KEY_3) == GLFW_PRESS) { //three "3"
		scene->shading_mode = 1;
	}
	if (glfwGetKey(user->window, GLFW_KEY_4) == GLFW_PRESS) { //four "4"
		scene->shading_mode = 2;
	}
	if (glfwGetKey(user->window, GLFW_KEY_5) == GLFW_PRESS) { //five "5"
		scene->shading_mode = 3;
	}
	if (glfwGetKey(user->window, GLFW_KEY_6) == GLFW_PRESS) { //six "6"
		scene->shading_mode = 4;
	}

	if (glfwGetMouseButton(user->window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) { //mouse button 5 (far thumb button)
		scene->GetObjectByName("quadrant lights")->SetActiveLocalTree(true); //turn on quadrant lights
	}
	if (glfwGetMouseButton(user->window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) { //mouse button 4 (close thumb button)
		scene->GetObjectByName("quadrant lights")->SetActiveLocalTree(false); //turn off quadrant lights
	}
	if (glfwGetKey(user->window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) { //caps lock
		scene->PrintObjectTrees();
	}
	if (glfwGetKey(user->window, GLFW_KEY_F1) == GLFW_PRESS) { //function 1 "f1"
		
	}
	if (glfwGetKey(user->window, GLFW_KEY_F) == GLFW_PRESS) { //f
		dynamic_cast<Light*>(scene->GetObjectByName("camera shell")->GetChildByNameTree("camera")->GetChildByNameTree("flashlight"))->SetActiveLocalTree(true); //turn on flashlight
	}
	if (glfwGetKey(user->window, GLFW_KEY_G) == GLFW_PRESS) { //g
		dynamic_cast<Light*>(scene->GetObjectByName("camera shell")->GetChildByNameTree("camera")->GetChildByNameTree("flashlight"))->SetActiveLocalTree(false); //turn off flashlight
	}
	if (glfwGetKey(user->window, GLFW_KEY_R) == GLFW_PRESS) { //r
		Object* camera_shell = scene->GetObjectByName("camera shell");
		if (camera_shell->parent == nullptr) {

			camera_shell->t->local.SetValue(camera_shell->t->local.pos, glm::vec3(0.0f));
			camera_shell->t->local.SetQuatValue(camera_shell->t->local.orn, glm::quat(1.0, 0.0, 0.0, 0.0));
			camera_shell->t->local.Rotate(45.f, -45.f, 0.f);
			Object* camera = camera_shell->GetChildByNameTree("camera");
			camera->t->local.SetValue(camera->t->local.pos, glm::vec3(0.f, 0.f, 10.f));

		}
		AttachCamera(scene, nullptr); //reset camera to origin and detach from any parents

	}
	if (glfwGetKey(user->window, GLFW_KEY_L) == GLFW_PRESS) { //l
		if (!scene->GetMeshByName("ink_blot")->visible) {
			scene->GetMeshByName("ink_blot")->visible = true;
			Launch(scene, scene->GetObjectByName("ink"));
		}
	}
	if (glfwGetKey(user->window, GLFW_KEY_ENTER) == GLFW_PRESS) { //enter "return"
		AttachCamera(scene, scene->held_object); //reset camera to origin and detach from any parents
	}

}

static void Launch(Scene*& scene, Object* object) {

	glm::vec3 launch_point = scene->GetObjectByName("ink")->t->global.pos;
	launch_vector = scene->GetObjectByName("pen")->t->global.orn * glm::vec3(1.f, 0, 0) * launch_force;
	scene->GetObjectByName("pen")->DetatchChild(scene->GetObjectByName("ink"));

	scene->GetObjectByName("ink")->t->local.SetQuatValue(scene->GetObjectByName("ink")->t->local.orn, glm::quat(1.f, 0.f, 0.f, 0.f));
	glm::vec3 old_pos = scene->GetObjectByName("ink")->t->global.pos;
	scene->GetObjectByName("ink")->t->global.SetValue(scene->GetObjectByName("ink")->t->global.pos, launch_point);
	scene->GetObjectByName("ink")->t->UpdateLocalPositionFromGlobal(old_pos);
	

	gravity = glm::vec3(0.f);


	
	


}

static void ProcessPhysics(Scene*& scene) {

	if (scene->GetMeshByName("ink_blot")->visible) {
		scene->GetMeshByName("ink_blot")->t->local.TranslateGlobalUp(gravity[1], delta_time);
		scene->GetMeshByName("ink_blot")->t->local.TranslateGlobal(launch_vector[0], launch_vector[1], launch_vector[2], delta_time);
		scene->GetObjectByName("ink")->UpdateTree();
		gravity -= glm::vec3(0, 9.8f * delta_time, 0);
	}

	if (scene->GetMeshByName("ink_blot")->t->global.pos[1] <= 0) {
		scene->GetMeshByName("ink_blot")->visible = false;

		scene->GetObjectByName("base")->t->local.SetValue(scene->GetObjectByName("base")->t->local.pos, scene->GetMeshByName("ink_blot")->t->global.pos + glm::vec3(0.f, 1.f, 0.f));
		scene->GetObjectByName("pen")->AttachChild(scene->GetObjectByName("ink"));
		scene->GetObjectByName("ink")->t->local.SetValue(scene->GetObjectByName("ink")->t->local.pos, glm::vec3(0.f, 0.f, -1.f));
		scene->GetMeshByName("ink_blot")->t->local.SetValue(scene->GetMeshByName("ink_blot")->t->local.pos, glm::vec3(0.f, 0.f, 0.f));
		
		scene->UpdateObjectTrees();
	}
}

