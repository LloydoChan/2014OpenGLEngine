//TeapotApp.h
#pragma once
#include "MainApp.h"
#include "GBufferData.h"
#include "ForwardShader.h"
#include "DeferredShader.h"


class TeapotApp : public App{
public:
	bool Init();
	void Run();
	void ShutDown();

private:
	bool   CreateGBuffer();

	void RenderForward(const vec3* lightPositions, const vec3* teapotPositions);
	void RenderDeferred(const vec3* teapotPositions);

	float CalcSphereDistance(const PointLightData& pLight, int index);

	ForwardShader		  m_teapotShader;
	DeferredShader		  m_deferredShader;

	StaticMesh            m_teapotMesh;
	StaticMesh			  m_sphereMesh;

	GLuint				  m_groundPlaneBuffer;


	GBufferData		m_gBuffer;
	PointLightData  m_lights;
	vec3            lightPos[5];

	//full screen quad
	GLuint m_quadBuffer;
};

