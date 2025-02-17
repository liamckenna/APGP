#include "textures.h"
#include "texture.h"

Textures::Textures(Scene* scene) {
	it = 0;
	this->scene = scene;
	binding_point = TEXTURE_BINDING_POINT;
	GenerateBuffer();
}

void Textures::SetupBuffer() {
	PopulateBuffer();
	BindBuffer();
}

void Textures::GenerateBuffer() {
	glGenBuffers(1, &ubo);
}

void Textures::PopulateBuffer() {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	std::vector<int> texture_indices;

	for (auto& texture : textures) {
		texture_indices.push_back(texture->index);
	}

	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) * texture_indices.size(), texture_indices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Textures::BindBuffer() {
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
}

void Textures::InsertAndLoad(Texture* texture) {
	Insert(texture);
	texture->LoadTexture();
}

void Textures::Insert(Texture* texture) {
	texture->index = IterateIndex();
	textures.push_back(texture);
}

int Textures::IterateIndex() {
	int idx = it;
	it++;
	return idx;
}
