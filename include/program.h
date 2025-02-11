#pragma once
#include <iostream>
struct User;
struct Scene;
struct Clock;

struct Program {
	User* user;
	Scene* scene;
	//metadata ?
	Clock* clock;

	Program(const std::string& filepath);
};