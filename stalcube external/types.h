#pragma once
#include "ext/imgui/imgui.h"
#include <string>

struct vec3 {
	float x,y,z;
};

struct vec2 {
	float x,y;

	operator ImVec2() const { return ImVec2(x, y); }
};

struct vec4 {
	float x, y, z, w;

	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct GLfloat16 {
	float values[16];
};

struct GLint4 {
	int values[4];
};

struct BBox2D {
	vec2 min;
	vec2 max;
};

struct BBox3D {
	vec3 min;
	vec3 max;
};