#pragma once
#include <iostream>
struct User;
struct Windows;
struct Scene;
struct Shaders;
struct Clock;
struct GraphicsConfig;

enum PROGRAM_MODE
{

};

struct Program {
	User* user;
	Windows* windows;
	Scene* scene;
	Shaders* shaders;
	GraphicsConfig* graphics_config;
	
	Clock* clock;
	

	Program(const std::string& filepath);

	void Run();
	void Cleanup();
};