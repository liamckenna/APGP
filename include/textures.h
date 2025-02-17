#pragma once

#include <vector>
#include "texture.h"
#include <GL/glew.h>
#include "scene.h"

struct Scene;

struct Textures {
	std::vector<Texture*> textures;
	int it;
	GLuint ubo;
	GLuint binding_point;

	Scene* scene;

	Textures(Scene* scene);
	void SetupBuffer();
	void GenerateBuffer();
	void PopulateBuffer();
	void BindBuffer();
	void InsertAndLoad(Texture* texture);
	void Insert(Texture* texture);
	int IterateIndex();
};
