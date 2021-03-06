// Suqua.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>
#include <time.h>
#include <algorithm>


#include <glad\glad.h>
#include <SDL.H>
#include "stb_image.h"

#include "Shader.h"

#include "PlayerMechLC.h"

#include "PlayerMechGC.h"
#include "ImageGC.h"
#include "AABBLC.h"


#include "Controller.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "BiomeDC.h"
#include "BiomeDrawer.h"
#include "PlayerCam.h"
#include "BiomeGenerator.h"
#include "SwimmerGC.h"
#include "SwimmerLC.h"
#include "PhysicsAABBLC.h"

#include "GLRenderer.h"
#include "TestCommand.h"
#include "TeleportCommand.h"
#include "MoveCommand.h"

const int windowWidth = 1920;
const int windowHeight = 1080;

void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = NULL;
	window = SDL_CreateWindow("Suqua", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Failed.");
		return 1;
	}

	GLRenderer::Init(window);
	int shaderId[2];
	GLRenderer::LoadShaders({ {"shaders/image.vert", "shaders/image.frag"}, {"shaders/image.vert", "shaders/text.frag"} }, &shaderId[0]);
	GLRenderer::GetShaderRef(shaderId[0]).use();
	GLRenderer::GetShaderRef(shaderId[0]).uniform2f("windowRes", windowWidth, windowHeight);
	GLRenderer::GetShaderRef(shaderId[1]).use();
	GLRenderer::GetShaderRef(shaderId[1]).uniform2f("windowRes", windowWidth, windowHeight);


	DebugIO::startDebug("fonts/consolas_0.png", "fonts/consolas.fnt");
	DebugIO::getCommandManager().registerCommand<TestCommand>();
	DebugIO::getCommandManager().registerCommand<TeleportCommand>();
	DebugIO::getCommandManager().registerCommand<MoveCommand>();

	/*------------------------ Player Preparation -----------------------------*/
	Vec2f spawnPos{ 0, -32 };
	unsigned int mechId;
	unsigned int playerId;
	EntitySystem::GenEntities(1, &mechId);
	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<PlayerMechLC>(1, &mechId);
	EntitySystem::GetComp<PlayerMechLC>(mechId)->setPos(spawnPos);
	EntitySystem::MakeComps<PlayerMechGC>(1, &mechId);
	EntitySystem::MakeComps<PhysicsAABBLC>(1, &mechId);
	EntitySystem::GetComp<PhysicsAABBLC>(mechId)->setRes(Vec2f{ 22, 30 });
	EntitySystem::MakeComps<SwimmerGC>(1, &playerId);
	EntitySystem::MakeComps<SwimmerLC>(1, &playerId);
	EntitySystem::MakeComps<PhysicsAABBLC>(1, &playerId);
	EntitySystem::GetComp<PhysicsAABBLC>(playerId)->setRes(Vec2f{ 10, 3 });



	Controller controller;

	/*---------------------------------- Camera Preparation -------------------------------------------------------*/

	Camera cam{ Vec2i{20, -windowHeight / 5 + 16}, Vec2i{windowWidth, windowHeight}, 3.5 };
	cam.center(spawnPos);
	int camId = GLRenderer::addCamera(cam);
	PlayerCamera playerCam{ camId, 0, 50, false, false, Vec2f{0.0, 0.0} };
	int menuCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowHeight, windowHeight }, 1.0 });

	/*-------------------------------------------- World Generation -----------------------------------------------*/


	//create graphical components
	EntityId tileIds[4];
	EntitySystem::GenEntities(4, &tileIds[0]);
	EntitySystem::MakeComps<ImageGC>(4, &tileIds[0]);


	EntitySystem::GetComp<ImageGC>(tileIds[0])->loadImageFromFile("images/floor_rock_drk.png");
	EntitySystem::GetComp<ImageGC>(tileIds[1])->loadImageFromFile("images/floor_rock_drk_fill.png");
	EntitySystem::GetComp<ImageGC>(tileIds[2])->loadImageFromFile("images/sand.png");
	EntitySystem::GetComp<ImageGC>(tileIds[3])->loadImageFromFile("images/sand_fill.png");

	World world;
	world.createBiomeType<FlatBiomeGenerator>(0, 0, BiomeGraphicData{ 0, 0, std::array<EntityId, 2>{tileIds[2], tileIds[3]} });
	world.createBiomeType<TestBiomeGenerator>(1, 0, BiomeGraphicData{ 1, 0, std::array<EntityId, 2>{tileIds[0], tileIds[1]} });
	world.generate(WorldData{
		{
			//supertype, start, end
			{0, {0, 0}, {99, 0}},			//flat
			{1, {100, 0}, {199, 0}},			//flat
		}
		});

	GLRenderer::SetShader(shaderId[0]);
	world.bufferRender();

	double gfxDelay{ 1.0 / 60 };
	double logDelay{ 1.0 / 120 };
	Uint64 currentLog = SDL_GetPerformanceCounter();
	Uint64 currentGfx = SDL_GetPerformanceCounter();

	/*--------------------------------------------- GAME LOOP -----------------------------------------------*/
	bool lockFPS{ true }, lockUPS{ true };

	Sprite dot{"images/dot.png"};
	Sprite dot2{ "images/dot.png" };
	bool running{ true };
	while (running) {
		Uint64 now = SDL_GetPerformanceCounter();
		//logic
		bool updateLogic{false};
		if (lockUPS)
			updateLogic = static_cast<double>(now - currentLog) / SDL_GetPerformanceFrequency() >= logDelay;
		else
			updateLogic = true;

		if (updateLogic) {
			double ups = 1.0 / (static_cast<double>(now - currentLog) / SDL_GetPerformanceFrequency());
			DebugIO::setLine(0, "UPS: " + std::to_string(int(round(ups))));

			currentLog = now;

			/*--------------------------- Handle Input ----------------------------*/

			int mouseScroll{ 0 };
			//wait till after polling events to toggle debug, so we don't output the opening char
			bool toggleDebug{ false };
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_CLOSE)
						running = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (e.button.button == SDL_BUTTON_RIGHT)
						controller.on(ControllerBits::BUTTON_2);
					break;
				case SDL_MOUSEBUTTONUP:
					if (e.button.button == SDL_BUTTON_RIGHT)
						controller.off(ControllerBits::BUTTON_2);
					break;
				case SDL_MOUSEWHEEL:
					if (e.wheel.y > 0)
						++mouseScroll;
					else if (e.wheel.y < 0)
						--mouseScroll;
					break;
				case SDL_TEXTINPUT:
					if (DebugIO::getOpen())
						DebugIO::addInput(e.text.text);
					break;
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_BACKQUOTE)
						toggleDebug = true;
					else if (e.key.keysym.sym == SDLK_BACKSPACE)
						DebugIO::backspace();
					else if (e.key.keysym.sym == SDLK_RETURN)
						DebugIO::enterInput();
					break;
				}
			}
			if(toggleDebug)
				DebugIO::toggleDebug();

			//reset / set scroll
			controller.setMouseScroll(mouseScroll);

			const Uint8 *state = SDL_GetKeyboardState(NULL);

			if (!DebugIO::getOpen()) {
				//update controllers

				controller.set(ControllerBits::UP, state[SDL_SCANCODE_W]);
				controller.set(ControllerBits::DOWN, state[SDL_SCANCODE_S]);
				controller.set(ControllerBits::LEFT, state[SDL_SCANCODE_A]);
				controller.set(ControllerBits::RIGHT, state[SDL_SCANCODE_D]);
				controller.set(ControllerBits::BUTTON_1, state[SDL_SCANCODE_SPACE]);
				controller.set(ControllerBits::BUTTON_3, state[SDL_SCANCODE_LSHIFT]);
			}
			else {
				controller.off(ControllerBits::ALL);
			}

			EntitySystem::GetComp<PlayerMechLC>(mechId)->update(logDelay, controller);
			EntitySystem::GetComp<SwimmerLC>(playerId)->update(logDelay, controller);
			//PlayerMechLC::updateAll(logDelay, controller);

			playerCam.update<SwimmerLC, SwimmerGC>(state, playerId, controller);
		}

		//gfx

		bool updateGFX{ false };
		if (lockFPS)
			updateGFX = static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency() >= gfxDelay;
		else
			updateGFX = true;
		if (updateGFX) {
			double fps = 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency());
			DebugIO::setLine(1, "FPS: " + std::to_string(int(round(fps))));
			//std::cout << 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency()) << std::endl;
			currentGfx = now;

			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			GLRenderer::SetShader(shaderId[0]);
			unsigned int tex[2];
			tex[0] = EntitySystem::GetComp<PlayerMechGC>(mechId)->getRenderBuffer();
			tex[1] = EntitySystem::GetComp<SwimmerGC>(playerId)->getRenderBuffer();
			GLRenderer::ClearRenderBufs(GLRenderer::include, 2, &tex[0]);

			//update camera
			const Uint8 *state = SDL_GetKeyboardState(NULL);


			EntitySystem::GetComp<PlayerMechGC>(mechId)->update(gfxDelay);
			EntitySystem::GetComp<SwimmerGC>(playerId)->update(gfxDelay);
			//PlayerMechGC::updateAll(gfxDelay);
			GLRenderer::SetBuffer(tex[0]);
			GLRenderer::Buffer(EntitySystem::GetComp<PlayerMechGC>(mechId)->getImgData());

			if (EntitySystem::GetComp<SwimmerLC>(playerId)->getState() != PlayerState::mounted) {
				GLRenderer::SetBuffer(tex[1]);
				GLRenderer::Buffer(EntitySystem::GetComp<SwimmerGC>(playerId)->getImgData());
			}

			GLRenderer::setCamera(camId);
			unsigned int debugTextBuffer = DebugIO::getRenderBuffer();
			GLRenderer::Draw(GLRenderer::exclude, 1, &debugTextBuffer);
			

			GLRenderer::SetShader(shaderId[1]);
			GLRenderer::setCamera(menuCamId);
			DebugIO::drawLines(windowHeight);
			GLRenderer::Swap();
		}
	}

	DebugIO::stopDebug();
	SDL_DestroyWindow(window);
	SDL_Quit();
    return 0;
}

/*
renderbuffers store:
textureId - opengl texture id
shaderId - shader as stored inside the GLRenderer
this inconsistency might be avoidable. Look into it
data - vector of ImgData to hold all the drawing information for draw

every texture should be assocaited with a render buffer
	No. Don't link these things, in case I want to do it later.
	They aren't linked right now, so no problem
only one texture should be associated with a renderbuffer
	True for now. Under what circumstnaces would I want to link a texture to other data buffers
		If I wanted to redraw the same thing multiple times with different textures -> animation of some kind
only one shader is associated with a renderbuffer
	true, thats the whole point

should sprites always have a renderbuffer
	A sprite is meant to be an abstraction of a drawable. Since something always needs a renderbuffer to be draw,
	a sprite should keep track of its renderbuffer in order to abstract the drawing process
should textures need a renderbuffer to be used
	yes, an assocation with data and with a shader is necessary to be drawn
*/

//in the future, perhaps everything will be buffered into a single large buffer, with sizes of data and the first member stored. wouldn't that be fancy.

/*
add map systems, containers to hold 2d position data.
change all pointers to references for the getComponent, and add error throwing
*/

/*
refactor the world system code
	focus on easy addition of biome types
new generations
new palletes
resize biomes
figure out how to load/unload pieces properly.
random biome selection
*/