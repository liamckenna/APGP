#pragma once

struct User;
struct Scene;
struct Clock;

struct Program {
	User* user;
	Scene* scene;


	//metadata ?
	Clock* clock;

};