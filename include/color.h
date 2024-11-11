#pragma once

#include "byte.h"
#include "colors.h"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
struct Color {

	union {
		struct {
			byte r; //red
			byte g; //green
			byte b; //blue
			byte a; //alpha (transparency value)
		};
		byte rgba[4];
	};

	bool NameMatch(COLORS color_name);
	bool operator==(Color c);
	glm::vec4 ToVec4();
	glm::vec3 ToVec3();

	template<typename T, typename U, typename V, typename W = byte>
	Color(T red, U green, V blue, W alpha = static_cast<W>(0xFF));

	template<typename W = byte>
	Color(COLORS color_name = COLORS::DEFAULT, W alpha = static_cast<W>(0xFF));
};

template<typename T, typename U, typename V, typename W>
Color::Color(T red, U green, V blue, W alpha) {
	r = ConvertToByte(red);
	g = ConvertToByte(green);
	b = ConvertToByte(blue);
	a = ConvertToByte(alpha);
}

template<typename W>
Color::Color(COLORS color_name, W alpha) {
	r = (color_name >> 16) & 0xFF;
	g = (color_name >> 8) & 0xFF;
	b =  color_name & 0xFF;
	a = ConvertToByte(alpha);
}