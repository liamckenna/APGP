#include "texture.h"
#include <iostream>
#include <stb_image.h>

Texture::Texture(const std::string& file_path) : file_path(file_path) {}

bool Texture::Load() {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Error: Failed to load texture: " << file_path << std::endl;
        return false;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLenum format = (channels == 1) ? GL_RED : (channels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    is_loaded = true;
    return true;
}

void Texture::Bind(int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Free() {
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
        is_loaded = false;
    }
}
