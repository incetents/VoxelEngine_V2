// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "opengl/glUtil.h"
#include "window/glfwUtil.h"

#include "window/window.h"
#include "input/Input.h"

#include "utilities/Loader.h"
#include "utilities/logger.h"
#include "utilities/stringUtil.h"

#include "opengl/Geometry.h"
#include "opengl/Mesh.h"
#include "opengl/Shader.h"
#include "opengl/Texture.h"

#include "math/Camera.h"
#include "math/Color.h"
#include "math/Transform.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include <iostream>


/**/
using namespace Vxl;
using namespace std;

#if _DEBUG
// Console Mode
int main()
#else
// Disable Console in release mode
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
#endif
{
	glfwUtil::init();

	Window.Setup("Vxl Engine", 800, 800);

	glUtil::initGlew();
	glUtil::initHints();

	std::cout << "OpenGL Vers. " << glUtil::getOpenGLVersion() << std::endl;
	std::cout << glUtil::getRendererVersion() << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	/**/
	Geometry::Setup();
	/**/
	Loader::LoadScript_Shaders("./scripts/ShaderLoader.txt");
	/**/
	ShaderProgram* Sp = ShaderProgram::m_database.Get("gbuffer");

	/**/

	Mesh* m = new Mesh();

	Vector3 pos[] = {
		Vector3(-0.5f, -0.5f, 0.0f),
		Vector3(0.5f, -0.5f, 0.0f),
		Vector3(0.5f, 0.5f, 0.0f),
		Vector3(-0.5f, 0.5f, 0.0f)
	};
	Vector2 uvs[] = {
		Vector2(0,0),
		Vector2(1,0),
		Vector2(1,1),
		Vector2(0,1)
	};
	Vector3 nor[] = {
		Vector3(0,0,1),
		Vector3(0,0,1),
		Vector3(0,0,1),
		Vector3(0,0,1)
	};
	GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };

	m->m_positions.set(pos, 4);
	m->m_uvs.set(uvs, 4);
	m->m_normals.set(nor, 4);
	m->m_indices.set(indices, 6);

	m->Bind();

	// Texture
	Texture* tex = new Texture("./assets/textures/beato.png");

	// Transform
	Camera cam(Vector3(0,0,0));
	cam.setOrthographic(-5, 5, -5, 5);
	cam.update();

	static float rot = 0.0f;
	Transform model(Vector3(0, 0.5f, 0), Vector3(0, 0, 0));

	while (!Window.GetClosed())
	{
		glUtil::clearBuffer();
		glUtil::clearColor(Color3F(0.1f, 0.1f, 0.3f));

		if (Input.getKeyDown(KeyCode::ESCAPE))
			Window.Close();

		Sp->Bind();

		model.setRotationX(rot / 2);
		model.setRotationY(rot++);
		//model.setScale(0.2f);

		// SUBROUTINES
		// glSubroutine Sub = Sp->GetSubroutine(ShaderType::FRAGMENT);
		// Sub.set("Colour1", "ColorRed");
		// Sub.set("Colour2", "ColorGreen");
		// Sub.set("myNum", "t2");
		// Sub.bind();

		// UNIFORM BLOCKS
		//auto block = Sp->GetUniformBlock("ColorBlock_0");
		//float myFloats[3] = { 0.5f, 0.0f, 1.0f };
		//block->set(myFloats, 3);
		//float r = 0.5f;
		//float g = 1.0f;
		//float b = 0.0f;
		//block->set(&r, 1, 0);
		//block->set(&g, 1, 1);

		// UNIFORMS
		// Sp->SetUniform("value1", 0.1f);
		// Sp->SetUniform("value2", 0.1f);
		// Sp->SetUniform("value3", 0.1f);

		// Uniform a = Sp->GetUniform("value1");
		// a.set<float>(1.0f);
		// Uniform b = Sp->GetUniform("value2");
		// b.set<float>(0.0f);
		// Uniform c = Sp->GetUniform("value3");
		// c.set<float>(0.5f);

		Sp->SetUniform("model", model.getModel());
		
		Sp->SetUniform("view", cam.getView());
		Sp->SetUniform("projection", cam.getProjection());

		if (Input.getKey(KeyCode::LEFT))
		{
			//cam.m_transform.increasePositionX(-1.0f);
		}
		else if (Input.getKey(KeyCode::RIGHT))
		{
			//cam.m_transform.increasePositionX(+1.0f);
		}
		cam.update();

		// TEX 
		glActiveTexture(GL_TEXTURE0);
		tex->bind();

		// MESHES
		//glUtil::cullMode(Cull_Type::NONE);
		Geometry::GetCube()->Draw();
		//Geometry::GetFullQuad()->Draw();

		//
		Input.Update();
		Window.Update();
	}

	Window.Terminate();

	return 0;
}