#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
Texture::Texture(const std::string& file_path, const TEXTURE_TYPES& type) {
	this->file_path = file_path;
	this->type = type;
	this->id = 0;
	wrap_s = GL_REPEAT;
   	wrap_t = GL_REPEAT;
	min_filter = GL_LINEAR_MIPMAP_LINEAR;
	mag_filter = GL_LINEAR;
}

bool Texture::LoadTexture() {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + index); //bind to specific texture unit
	glBindTexture(GL_TEXTURE_2D, id);
	

	int width, height, nrChannels; //load texture data using stb_image
	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//set texture wrapping/filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

		stbi_image_free(data);
		return true;
	} else {
		std::cout << "Failed to load texture: " << file_path << std::endl;
		stbi_image_free(data);
		return false;
	}
}

void Texture::Bind(GLenum texture_unit) const {
	glActiveTexture(texture_unit); //activate texture unit
	glBindTexture(GL_TEXTURE_2D, id); //bind texture ID to texture unit
}