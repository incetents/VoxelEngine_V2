// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "window/window.h"
#include "input/Input.h"

#include "utilities/Loader.h"
#include "utilities/logger.h"
#include "utilities/Time.h"

#include "modules/Material.h"
#include "../game/Scene_Game.h"
#include "modules/RenderManager.h"

/**/
using namespace Vxl;
using namespace std;

Scene_Game* _scene = new Scene_Game();

#if _DEBUG
// Console Mode
int main()
#else
// Disable Console in release mode
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
#endif
{
	// GLFW Setup
	if (!Window.InitGLFW())
		return -1;

	// Window
	Window.Setup("Vxl Engine", SCREEN_WIDTH, SCREEN_HEIGHT);
	//Window.SetCustomAspectRatio(true, 1.0f);

	/* Special */
	MaterialData::NULL_TEXTURE = new Texture("./assets/textures/null_texture.png", Wrap_Mode::CLAMP_STRETCH, Filter_Mode::NEAREST, Filter_Mode::NEAREST);
	Geometry::Setup();
	/* ~ */
	RenderManager.SetNewScene(_scene);
	//_scene->setup();

	/* ~ */
	while (!Window.GetClosed())
	{
		// OpenGl Reset
		if (Input.getKeyDown(KeyCode::R))
		{
			Window.Reload();

			Geometry::Reload();
			
			RenderManager.Reload();
		}

		// Before Frame
		TimeController.StartFrame();
		Window.StartFrame();

		// Scene Update/Render
		while (TimeController.GetTotalDeltaTime() >= 1.0)
		{
			RenderManager.Update();
			TimeController.DecreaseTotalDeltaTime();
		}
		RenderManager.Draw();
		RenderManager.DrawImGui();

		// End of frame update
		Input.Update();
		Window.EndFrame();
		TimeController.EndFrame();
	}

	// Cleanup
	RenderManager.Destroy();
	Window.Shutdown();

	return 0;
}