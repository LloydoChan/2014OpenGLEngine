//VertexTypes.h
#pragma once
#include <glm.hpp>
using namespace glm;

struct CustomVertexHud{
	glm::vec2 vertexPoint;
	glm::vec2 uv;
};

struct CustomVertexUV{
	glm::vec3 vertexPoint;
	glm::vec3 uv;
};

struct CustomVertexNormUV{
	glm::vec3 vertexPoint;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct CustomVertexNorm{
	glm::vec3 vertexPoint;
	glm::vec3 normal;
};


struct CustomVertexNormBiTangentUV{
	glm::vec3 vertexPoint;
	glm::vec3 normal;
	glm::vec3 biTangent;
	glm::vec2 uv;
};
