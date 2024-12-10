#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include <algorithm>
#include "transform.h"
#include "vertex.h"
#include "mesh.h"
#include "window.h"
#include "camera.h"
#include "scene.h"
#include "cursor.h"
#include "light.h"
#include "input.h"
#include "user.h"
#include "shaders.h"
#include "json.h"
#include "object_parser.h"
#include "program.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL


//Uniform IDs

float last_time = 0.f;
float last_fps_time = 0.f;
int frame_count = 0;
float delta_time = 0.f;

//forward declarations of functions
static User* UserGeneration(std::string file);
static Program* ProgramGeneration(std::string program_filepath);
std::string GetExecutableDirectory();
std::string LoadShader(const char* filepath);
GLuint createShader(GLenum type, const char* shaderSource);
static void error_callback(int error, const char* description);
static void MouseCallback(User*& user, Scene*& scene);
static bool ShaderInitialization(Scene*& scene, nlohmann::json data);
static Scene* SceneGeneration(std::string file);
static void BufferArrayInitialization(Scene*& scene);
static void ProcessInput(User*& user, Scene*& scene);
void CalculateFrameRate();
void CalculateDeltaTime();
static void RenderScene(User*& user, Scene*& scene);
static void Cleanup(User*& user, Scene*& scene);
static void CameraGeneration(Scene*& scene, nlohmann::json data);
static void LightGeneration(Scene*& scene, nlohmann::json data);
static void MeshGeneration(Scene*& scene, nlohmann::json data);
static void ObjectGeneration(Scene*& scene, nlohmann::json data);
static void UpdateCameraUniforms(Camera*& camera);
static void UpdateLightUniforms(Scene*& scene);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void DefaultMaterialGeneration(Scene*& scene, nlohmann::json data);
static void UpdateTextureUniforms(Scene*& scene);
static void AttachCamera(Scene*& scene, Object* new_parent);
static void Launch(Scene*& scene, Object* object);
static void ProcessPhysics(Scene*& scene);
static void UpdateMeshBuffer(Scene*& scene, Mesh* mesh);
static void FramebufferInitialization(User*& user, Scene*& scene);
static void RenderFullScreenQuad(Scene*& scene);
static bool ProcessShader(Scene*& scene, std::string file, SHADER_TYPE TYPE, bool composite);
static void RenderDirectlyToScreen(Scene*& scene);
static void RenderToFramebuffer(Scene*& scene);
glm::vec3 gravity;
glm::vec3 launch_vector;
float launch_force;

int main() {

	Program* program = ProgramGeneration("/data/jsons/program.json");

	Scene* scene = program->scene;
	User* user = program->user;

	FramebufferInitialization(user, scene);

	BufferArrayInitialization(scene);

	std::cout << "initialized buffers and arrays" << std::endl;

	UpdateTextureUniforms(scene);

	std::cout << "initialized textures" << std::endl;

	//scene->PrintObjectTrees();

	//std::cout << "texture count: " << scene->textures.size() << std::endl;
	//std::cout << "texture filepath:" << scene->textures[0]->file_path << std::endl;
	//std::cout << "texture index:" << scene->textures[0]->index << std::endl;
	int textureUniformLoc = glGetUniformLocation(scene->shaders->shader_program, "textures[0]");
	if (textureUniformLoc == -1) {
		std::cerr << "Uniform 'textures[0]' not found in the shader!" << std::endl;
	}
	else {
		//std::cerr << "Uniform 'textures[0]' WAS FOUND in the shader!" << std::endl;
	}
	RenderScene(user, scene);

	std::cout << "finished rendering scene" << std::endl;

	Cleanup(user, scene);

	std::cout << "finished cleanup" << std::endl;

	exit(EXIT_SUCCESS);
}

static void FramebufferInitialization(User*& user, Scene*& scene) {
	// Generate and bind framebuffer
	glGenFramebuffers(1, &scene->buffers->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, scene->buffers->framebuffer);

	// Create and attach AccumColor texture
	glGenTextures(1, &scene->buffers->accum_color_tex);
	glBindTexture(GL_TEXTURE_2D, scene->buffers->accum_color_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, user->window->width, user->window->height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene->buffers->accum_color_tex, 0);

	// Create and attach AccumAlpha texture
	glGenTextures(1, &scene->buffers->accum_alpha_tex);
	glBindTexture(GL_TEXTURE_2D, scene->buffers->accum_alpha_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, user->window->width, user->window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, scene->buffers->accum_alpha_tex, 0);

	// Create and attach depth renderbuffer
	glGenRenderbuffers(1, &scene->buffers->depth_attachment);
	glBindRenderbuffer(GL_RENDERBUFFER, scene->buffers->depth_attachment);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, user->window->width, user->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, scene->buffers->depth_attachment);

	// Specify draw buffers
	GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

	// Check framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	else {
		std::cout << "Framebuffer successfully created!" << std::endl;
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static User* UserGeneration(std::string file) {
	
	nlohmann::json data = ReadJsonFromFile(file);

	User* user = new User();
	
	user->window = new Window(	data["window"].contains("width")		? int(data["window"]["width"])					: 1920,
								data["window"].contains("height")		? int(data["window"]["height"])					: 1080,
								data["window"].contains("msaa")			? int(data["window"]["msaa"])					: 1,
								data["window"].contains("pos_x")		? int(data["window"]["pos_x"])					: 0,
								data["window"].contains("pos_y")		? int(data["window"]["pos_y"])					: 0,
								data["window"].contains("resizable")	? bool(data["window"]["resizable"])				: true,
								data["window"].contains("decorated")	? bool(data["window"]["decorated"])				: true,
								data["window"].contains("focused")		? bool(data["window"]["focused"])				: true,
								data["window"].contains("visible")		? bool(data["window"]["visible"])				: true,
								data["window"].contains("display_mode") ? std::string(data["window"]["display_mode"])	: "windowed",
								data["window"].contains("title")		? std::string(data["window"]["title"])			: "Window Title" );
	user->input = new Input(user->window, 
							(data["settings"].contains("input") && data["settings"]["input"].contains("cursor") && data["settings"]["input"]["cursor"].contains("sensitivity")) 
							? float(data["settings"]["input"]["cursor"]["sensitivity"]) : 3.f);


	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glfwWindowHint(GLFW_SAMPLES, user->window->msaa);
	glfwWindowHint(GLFW_RESIZABLE, user->window->resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, user->window->decorated ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_FOCUSED, user->window->focused ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, user->window->visible ? GLFW_TRUE : GLFW_FALSE);


	if (user->window->display_mode == FULLSCREEN) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
		user->window->window = glfwCreateWindow(user->window->width, user->window->height, user->window->title.c_str(), monitor, NULL);
	}
	else if (user->window->display_mode == WINDOWED_FULLSCREEN) {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
		user->window->window = glfwCreateWindow(user->window->width, user->window->height, user->window->title.c_str(), NULL, NULL);
		glfwSetWindowPos(user->window->window, 0, 0);
	}
	else {
		user->window->window = glfwCreateWindow(user->window->width, user->window->height, user->window->title.c_str(), NULL, NULL);
		glfwSetWindowPos(user->window->window, user->window->pos_x, user->window->pos_y);
	}


	if (!user->window->window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(user->window->window);
	glfwSetScrollCallback(user->window->window, scroll_callback);

	if (data.contains("settings")) {
		if (data["settings"].contains("gl")) {
			if (data["settings"]["gl"].contains("profile")) {
				if (data["settings"]["gl"]["profile"] == "any") glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
				else if (data["settings"]["gl"]["profile"] == "core") glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				else if (data["settings"]["gl"]["profile"] == "compat") glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
			}
			if (data["settings"]["gl"].contains("depth_test") && data["settings"]["gl"]["depth_test"]) glEnable(GL_DEPTH_TEST);
			if (data["settings"]["gl"].contains("depth_function")) {
				if (data["settings"]["gl"]["depth_function"] == "false") glDepthFunc(GL_NEVER);
				else if (data["settings"]["gl"]["depth_function"] == "<") glDepthFunc(GL_LESS);
				else if (data["settings"]["gl"]["depth_function"] == "==") glDepthFunc(GL_EQUAL);
				else if (data["settings"]["gl"]["depth_function"] == "<=") glDepthFunc(GL_LEQUAL);
				else if (data["settings"]["gl"]["depth_function"] == ">") glDepthFunc(GL_GREATER);
				else if (data["settings"]["gl"]["depth_function"] == "!=") glDepthFunc(GL_NOTEQUAL);
				else if (data["settings"]["gl"]["depth_function"] == ">=") glDepthFunc(GL_GEQUAL);
				else if (data["settings"]["gl"]["depth_function"] == "true") glDepthFunc(GL_ALWAYS);
			}
			if (data["settings"]["gl"].contains("depth_mask")) glDepthMask(data["settings"]["gl"]["depth_mask"] ? GL_TRUE : GL_FALSE);
			if (data["settings"]["gl"].contains("cull_face") && data["settings"]["gl"]["cull_face"]) glEnable(GL_CULL_FACE);
			if (data["settings"]["gl"].contains("cull_side")) {
				if (data["settings"]["gl"]["cull_side"] == "front") glCullFace(GL_FRONT);
				else if (data["settings"]["gl"]["cull_side"] == "back") glCullFace(GL_BACK);
				else if (data["settings"]["gl"]["cull_side"] == "front and back") glCullFace(GL_FRONT_AND_BACK);
			}
			if (data["settings"]["gl"].contains("front_face")) {
				if (data["settings"]["gl"]["front_face"] == "cw") glFrontFace(GL_CW);
				else if (data["settings"]["gl"]["front_face"] == "ccw") glFrontFace(GL_CCW);
			}
			if (data["settings"]["gl"].contains("line_width")) glLineWidth(data["settings"]["gl"]["line_width"]);
			if (data["settings"]["gl"].contains("blend") && data["settings"]["gl"]["blend"]) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
		if (data["settings"].contains("glfw")) {
			if (data["settings"]["glfw"].contains("version_major")) glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, data["settings"]["glfw"]["version_major"]);
			if (data["settings"]["glfw"].contains("version_minor")) glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, data["settings"]["glfw"]["version_minor"]);
			if (data["settings"]["glfw"].contains("swap_interval")) glfwSwapInterval(data["settings"]["glfw"]["swap_interval"]);
		}
		if (data["settings"].contains("input")) {
			if (data["settings"]["input"].contains("cursor")) {
				if (data["settings"]["input"]["cursor"].contains("mode")) {
					if (data["settings"]["input"]["cursor"]["mode"] == "normal") glfwSetInputMode(user->window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					else if (data["settings"]["input"]["cursor"]["mode"] == "hidden") glfwSetInputMode(user->window->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					else if (data["settings"]["input"]["cursor"]["mode"] == "disabled") glfwSetInputMode(user->window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
			}
		}
		if (data["settings"].contains("glew")) {
			if (data["settings"]["glew"].contains("experimental")) glewExperimental = data["settings"]["glew"]["experimental"] ? GL_TRUE : GL_FALSE;
		}
	}

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	return user;
	
}

static Program* ProgramGeneration(std::string program_filepath) {

	nlohmann::json program_json = ReadJsonFromFile(program_filepath);

	Program* program = new Program();

	std::string user_filepath = "/data/jsons/users/" + std::string(program_json["user"]);
	std::string scene_filepath = "/data/jsons/scenes/" + std::string(program_json["scene"]);

	User* user = UserGeneration(user_filepath);
	program->user = user;

	std::cout << "initialized user" << std::endl;

	Scene* scene = SceneGeneration(scene_filepath);
	program->scene = scene;
	scene->user = user;

	std::cout << "initialized scene" << std::endl;

	glfwSetWindowUserPointer(user->window->window, scene);

	nlohmann::json scene_json = ReadJsonFromFile(scene_filepath);
	if (ShaderInitialization(scene, scene_json)) {
		std::cout << "yeah shaders are good" << std::endl;
	}

	std::cout << "initialized program" << std::endl;

	return program;
}

std::string GetExecutableDirectory() {
	return std::filesystem::current_path().string();
}

std::string LoadShader(const char* filepath) {
	std::string absolute_filepath = std::filesystem::current_path().string() + filepath;
	std::ifstream file(absolute_filepath);
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

static void MouseCallback(User*& user, Scene*& scene)
{
	user->input->cursor->Update();
	
	user->input->cursor->offset_x *= user->input->cursor->sensitivity * delta_time / user->window->width 	* 100000.f;
	user->input->cursor->offset_y *= user->input->cursor->sensitivity * delta_time / user->window->height	* 100000.f;

	
	scene->GetObjectByName("camera shell")->t->local.RotateYaw(user->input->cursor->offset_x);
	scene->GetObjectByName("camera shell")->t->local.RotatePitch(user->input->cursor->offset_y);
	scene->GetObjectByName("camera shell")->UpdateTree();
}

static bool ProcessShader(Scene*& scene, std::string file, SHADER_TYPE TYPE, bool composite) {
		
	std::string src = LoadShader(file.c_str());
	if (src.empty()) {
		std::cerr << "Shader source loading failed." << std::endl;
		return false;
	}
	GLuint ss;
	if (TYPE == VERTEX) {
		ss = createShader(GL_VERTEX_SHADER, src.c_str());
		Shader s = Shader(src, ss, VERTEX);
		scene->shaders->vertex_shaders.push_back(s);
	} else if (TYPE == GEOMETRY) {
		ss = createShader(GL_GEOMETRY_SHADER, src.c_str());
		Shader s = Shader(src, ss, GEOMETRY);
		scene->shaders->geometry_shaders.push_back(s);
	} else if (TYPE == FRAGMENT) {
		ss = createShader(GL_FRAGMENT_SHADER, src.c_str());
		Shader s = Shader(src, ss, FRAGMENT);
		scene->shaders->fragment_shaders.push_back(s);
	}

	if (composite) {
		//attach composite shader to dedicated program
		glAttachShader(scene->shaders->composite_program, ss);
		glDeleteShader(ss);
	} else {
		glAttachShader(scene->shaders->shader_program, ss);
		glDeleteShader(ss);
	}
	
	return true;
}

static bool ShaderInitialization(Scene*& scene, nlohmann::json data) {

	scene->shaders->shader_program = glCreateProgram();
	scene->shaders->composite_program = glCreateProgram();
	for (int i = 0; i < data["shaders"]["vertex_shaders"].size(); i++) {
		std::string fn = "/shaders/" + std::string(data["shaders"]["vertex_shaders"][i]["file"]);
		
		if (!ProcessShader(scene, fn, VERTEX, false)) return false;
	}
	for (int i = 0; i < data["shaders"]["geometry_shaders"].size(); i++) {
		std::cout << "trying to call geom shaders" << std::endl;
		std::string fn = "/shaders/" + std::string(data["shaders"]["geometry_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, GEOMETRY, false)) return false;
	}
	for (int i = 0; i < data["shaders"]["fragment_shaders"].size(); i++) {
		std::string fn = "/shaders/" + std::string(data["shaders"]["fragment_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, FRAGMENT, false)) return false;
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

	for (int i = 0; i < data["shaders"]["composite_vertex_shaders"].size(); i++) {
		std::string fn = "/shaders/" + std::string(data["shaders"]["composite_vertex_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, VERTEX, true)) return false;
	}
	for (int i = 0; i < data["shaders"]["composite_fragment_shaders"].size(); i++) {
		std::string fn = "/shaders/" + std::string(data["shaders"]["composite_fragment_shaders"][i]["file"]);
		if (!ProcessShader(scene, fn, FRAGMENT, true)) return false;
	}

	glLinkProgram(scene->shaders->composite_program);
	int composite_success;
	char composite_infoLog[512];
	glGetProgramiv(scene->shaders->composite_program, GL_LINK_STATUS, &composite_success);

	if (!composite_success) {
		glGetProgramInfoLog(scene->shaders->composite_program, 512, NULL, composite_infoLog);
		std::cout << "ERROR::SHADER::COMPOSITE::LINKING_FAILED\n" << composite_infoLog << std::endl;
		return false;
	}

	scene->user->accum_color_tex_id = glGetUniformLocation(scene->shaders->composite_program, "accum_color_tex");
	scene->user->accum_alpha_tex_id = glGetUniformLocation(scene->shaders->composite_program, "accum_alpha_tex");

	std::cout << "initialized shaders" << std::endl;
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
	mtl->roughness = data["default_material"]["roughness"];
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
		std::string fn = "/data/objects/" + std::string(data["meshes"][i]["file"]);
		Mesh* m = new Mesh();
		m->visible = data["meshes"][i]["visible"];
		m->current_scene = scene;
		m->t = new Transform(pos, rot, scl);
		m->name = data["meshes"][i]["name"];
		m->verbose = data["meshes"][i]["verbose"];
		m->idx = i;
		if (data["meshes"][i].contains("draw_mode")) m->SetDrawMode(data["meshes"][i]["draw_mode"]);
		else m->SetDefaultDrawMode();
		ObjectParser::ParseObjFile(fn, m);
		scene->meshes.push_back(m);
		//std::cout << m->name << " mesh completed" << std::endl;
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
		//std::cout << o->name << " object completed" << std::endl;
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
		//std::cout << c->name << " camera completed" << std::endl;

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
		//std::cout << l->name << " light completed" << std::endl;
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
		glBufferData(GL_ARRAY_BUFFER, flattened_vert_array.size() * sizeof(FlattenedVertex), flattened_vert_array.data(), GL_DYNAMIC_DRAW);

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
		glVertexAttribIPointer(5, 1, GL_INT, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, draw_mode));
		glEnableVertexAttribArray(5);
		
	}

	float quadVertices[] = {
		// Positions   // TexCoords
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
	};

	glGenVertexArrays(1, &scene->buffers->framebuffer_vao);
	glBindVertexArray(scene->buffers->framebuffer_vao);

	glGenBuffers(1, &scene->buffers->framebuffer_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, scene->buffers->framebuffer_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

void UpdateMeshBuffer(Scene*& scene, Mesh* mesh) {
	glBindBuffer(GL_ARRAY_BUFFER, scene->buffers->vertex_buffers[mesh->idx]);
	std::vector<FlattenedVertex> flattened_vert_array = scene->meshes[mesh->idx]->flattenVertices();

	GLint buffer_size = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

	size_t new_size = flattened_vert_array.size() * sizeof(FlattenedVertex);

	if (new_size > buffer_size) {
		size_t allocated_size = std::max(new_size, static_cast<size_t>(buffer_size) * 2);
		std::cout << "Resizing buffer: old size = " << buffer_size << ", new size = " << allocated_size << std::endl;

		glBufferData(GL_ARRAY_BUFFER, allocated_size, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, new_size, flattened_vert_array.data());
	}
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, new_size, flattened_vert_array.data());
	}
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
	glUseProgram(scene->shaders->shader_program);

	GLint maxTextures;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextures);

	for (size_t i = 0; i < scene->textures.size(); ++i) {
		if (i >= maxTextures) {
			std::cerr << "Warning: Texture count exceeds maximum supported units (" << maxTextures << ")" << std::endl;
			break;
		}

		GLenum texture_unit = GL_TEXTURE0 + i;
		scene->textures[i]->Bind(texture_unit);

		GLint location = glGetUniformLocation(scene->shaders->shader_program, ("textures[" + std::to_string(i) + "]").c_str());
		if (location == -1) {
			std::cerr << "Warning: Uniform 'textures[" << i << "]' not found in shader!" << std::endl;
			continue;
		}

		glUniform1i(location, i);
	}
}

static void UpdateUniforms(Scene*& scene, Camera*& camera) {
	
	UpdateCameraUniforms(camera);
	UpdateLightUniforms(scene);
	UpdateTextureUniforms(scene);
}

void CheckAttachedShaders(GLuint program) {
	GLint numShaders = 0;
	glGetProgramiv(program, GL_ATTACHED_SHADERS, &numShaders);

	if (numShaders == 0) {
		std::cout << "No shaders are attached to the program." << std::endl;
		return;
	}

	GLuint* shaders = new GLuint[numShaders];
	glGetAttachedShaders(program, numShaders, NULL, shaders);

	std::cout << "Shaders attached to program " << program << ":" << std::endl;
	for (GLint i = 0; i < numShaders; i++) {
		GLint shaderType;
		glGetShaderiv(shaders[i], GL_SHADER_TYPE, &shaderType);

		std::string typeStr;
		if (shaderType == GL_VERTEX_SHADER) typeStr = "Vertex Shader";
		else if (shaderType == GL_FRAGMENT_SHADER) typeStr = "Fragment Shader";
		else if (shaderType == GL_GEOMETRY_SHADER) typeStr = "Geometry Shader";
		else typeStr = "Unknown Shader Type";

		std::cout << "  Shader ID: " << shaders[i] << " (" << typeStr << ")" << std::endl;
	}

	delete[] shaders;
}

void CheckShaderCompilation(GLuint shaderID) {
	GLint success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cerr << "Shader Compilation Error (ID " << shaderID << "):\n" << infoLog << std::endl;
	}
	else {
		std::cout << "Shader (ID " << shaderID << ") compiled successfully." << std::endl;
	}
}

void CheckOpenGLErrors(const std::string& context) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error in " << context << ": " << err << std::endl;
	}
}

static void RenderScene(User*& user, Scene*& scene) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, scene->buffers->framebuffer);
	GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete: " << fbStatus << std::endl;
	}
	else {
		std::cout << "Framebuffer complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::cout << "Accum Color Texture ID: " << scene->buffers->accum_color_tex << std::endl;
	do {
		// Calculate frame timing
		CalculateFrameRate();
		CalculateDeltaTime();

		RenderDirectlyToScreen(scene);
		//RenderToFramebuffer(scene);
		
		// Process inputs and callbacks
		ProcessInput(user, scene);
		MouseCallback(user, scene);

		// Swap buffers and poll events
		glfwSwapBuffers(user->window->window);
		glfwPollEvents();

	} while (!glfwWindowShouldClose(user->window->window));

}


static void Cleanup(User*& user, Scene*& scene) {
	scene->buffers->CleanupBuffers();
	scene->shaders->CleanupShaders();
	glfwDestroyWindow(user->window->window);
	glfwTerminate();
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
	Object* camera = scene->GetObjectByName("camera");
	
	if (yoffset != 0 && camera->t->local.pos[2] >= 0) { //scrolling up
		if (glfwGetKey(scene->user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			scene->GetLightByName("flashlight")->strength += (yoffset * 1000.f * delta_time);
			std::cout << "strength: " << scene->GetLightByName("flashlight")->strength << std::endl;
		} else {
			camera->t->local.TranslateForward(dynamic_cast<Camera*>(camera->GetChildByNameTree("camera 1"))->velocity * yoffset * 500.f, delta_time);
			camera->t->UpdateGlobal();
		}
		
	}
	if (camera->t->local.pos[2] < 0) {
		if (glfwGetKey(scene->user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			scene->GetLightByName("flashlight")->strength -= (yoffset * 1000.f * delta_time);
			std::cout << "strength: " << scene->GetLightByName("flashlight")->strength << std::endl;
		}
		else {
			camera->t->local.SetValue(camera->t->local.pos, glm::vec3(camera->t->local.pos[0], camera->t->local.pos[1], 0));
			camera->t->UpdateGlobal();
		}
		
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
		if (glfwGetKey(user->window->window, GLFW_KEY_W) == GLFW_PRESS) { //w, translate forward
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity, delta_time);
			}
		}

		
		if (glfwGetKey(user->window->window, GLFW_KEY_S) == GLFW_PRESS) { //s, translate backward
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * -1.f, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_A) == GLFW_PRESS) { //a, translate left
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * -1.f, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_D) == GLFW_PRESS) { //d, translate right
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_SPACE) == GLFW_PRESS) { //space, translate up
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { //left ctrl, translate down
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * -1.f, delta_time);
			}
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_LEFT) == GLFW_PRESS) { //left arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateRight(-5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			scene->held_object->t->local.RotateYaw(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("pen") && glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) { //J4
			scene->held_object->parent->parent->t->local.RotateYaw(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J6
			scene->held_object->t->local.RotateRoll(-45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_RIGHT) == GLFW_PRESS) { //right arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateRight(5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			scene->held_object->t->local.RotateYaw(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("pen") && glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) { //J4
			scene->held_object->parent->parent->t->local.RotateYaw(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J6
			scene->held_object->t->local.RotateRoll(45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_DOWN) == GLFW_PRESS) { //down arrow
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
	if (glfwGetKey(user->window->window, GLFW_KEY_UP) == GLFW_PRESS) { //up arrow
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
	if (glfwGetKey(user->window->window, GLFW_KEY_COMMA) == GLFW_PRESS) { //comma ","
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_PERIOD) == GLFW_PRESS) { //period "."
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { //escape "esc"
		glfwSetWindowShouldClose(user->window->window, GL_TRUE);
	}
	//p2
	if (glfwGetKey(user->window->window, GLFW_KEY_B) == GLFW_PRESS) { //b
		scene->SetHeldObject(scene->GetObjectByName("base"));
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_T) == GLFW_PRESS) { //t
		scene->SetHeldObject(scene->GetObjectByName("top"));
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_U) == GLFW_PRESS) { //u
		scene->shading_mode = 5;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_P) == GLFW_PRESS) { //p
		scene->SetHeldObject(scene->GetObjectByName("pen"));
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_1) == GLFW_PRESS) { //one "1"
		scene->SetHeldObject(scene->GetObjectByName("arm1"));
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_2) == GLFW_PRESS) { //two "2"
		scene->SetHeldObject(scene->GetObjectByName("arm2"));
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_3) == GLFW_PRESS) { //three "3"
		scene->shading_mode = 1;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_4) == GLFW_PRESS) { //four "4"
		scene->shading_mode = 2;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_5) == GLFW_PRESS) { //five "5"
		scene->shading_mode = 3;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_6) == GLFW_PRESS) { //six "6"
		scene->shading_mode = 4;
	}

	if (glfwGetMouseButton(user->window->window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) { //mouse button 5 (far thumb button)
		scene->GetObjectByName("quadrant lights")->SetActiveLocalTree(true); //turn on quadrant lights
	}
	if (glfwGetMouseButton(user->window->window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) { //mouse button 4 (close thumb button)
		scene->GetObjectByName("quadrant lights")->SetActiveLocalTree(false); //turn off quadrant lights
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) { //caps lock
		scene->PrintObjectTrees();
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_F1) == GLFW_PRESS) { //function 1 "f1"
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_F) == GLFW_PRESS) { //f
		dynamic_cast<Light*>(scene->GetObjectByName("camera shell")->GetChildByNameTree("camera")->GetChildByNameTree("flashlight"))->SetActiveLocalTree(true); //turn on flashlight
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_G) == GLFW_PRESS) { //g
		dynamic_cast<Light*>(scene->GetObjectByName("camera shell")->GetChildByNameTree("camera")->GetChildByNameTree("flashlight"))->SetActiveLocalTree(false); //turn off flashlight
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_R) == GLFW_PRESS) { //r
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
	if (glfwGetKey(user->window->window, GLFW_KEY_L) == GLFW_PRESS) { //l
		if (scene->GetMeshByName("face")->draw_mode != GL_TRIANGLES) {
			scene->GetMeshByName("face")->SetDrawMode("GL_TRIANGLES");
			UpdateMeshBuffer(scene, scene->GetMeshByName("face"));
		}
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) { //;
		if (scene->GetMeshByName("face")->draw_mode != GL_LINES) {
			scene->GetMeshByName("face")->SetDrawMode("GL_LINES");
			UpdateMeshBuffer(scene, scene->GetMeshByName("face"));
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_ENTER) == GLFW_PRESS) { //enter "return"
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

static void RenderFullScreenQuad(Scene*& scene) {
	glBindVertexArray(scene->buffers->framebuffer_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

static void RenderDirectlyToScreen(Scene*& scene) {
	// Enable depth test and face culling for PBR pass
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// === First Pass: Render Scene to Framebuffer ===
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Check framebuffer completeness
	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete: " << framebufferStatus << std::endl;
		return; // Exit the loop if the framebuffer is not valid
	}

	// Use the PBR shader program
	glUseProgram(scene->shaders->shader_program);

	// Update scene data and draw objects
	scene->UpdateObjectTrees(true);
	UpdateLightUniforms(scene);
	scene->DrawObjectTrees();
}

static void RenderToFramebuffer(Scene*& scene) {
	// Enable depth test and face culling for PBR pass
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// === First Pass: Render Scene to Framebuffer ===
	glBindFramebuffer(GL_FRAMEBUFFER, scene->buffers->framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Check framebuffer completeness
	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer incomplete: " << framebufferStatus << std::endl;
		return; // Exit the loop if the framebuffer is not valid
	}

	// Use the PBR shader program
	glUseProgram(scene->shaders->shader_program);

	// Update scene data and draw objects
	scene->UpdateObjectTrees(true);
	UpdateLightUniforms(scene);
	scene->DrawObjectTrees();

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// === Second Pass: Composite Pass ===

	glDisable(GL_DEPTH_TEST); // Disable depth test for full-screen quad
	glDisable(GL_CULL_FACE); // Disable face culling for the quad

	glClear(GL_COLOR_BUFFER_BIT);

	// Check if the composite program is valid
	if (!glIsProgram(scene->shaders->composite_program)) {
		std::cerr << "Composite shader program is not valid!" << std::endl;
		return; // Exit the loop if the program is invalid
	}

	// Use the composite shader program
	glUseProgram(scene->shaders->composite_program);

	// Bind accumulated textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene->buffers->accum_color_tex);
	glUniform1i(glGetUniformLocation(scene->shaders->composite_program, "accum_color_tex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, scene->buffers->accum_alpha_tex);
	glUniform1i(glGetUniformLocation(scene->shaders->composite_program, "accum_alpha_tex"), 1);

	// Render the full-screen quad
	RenderFullScreenQuad(scene);
}