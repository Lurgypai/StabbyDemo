// Suqua.cpp : Defines the entry point for the console application.
//

#include "pch.h"
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
#include "PlayerGC.h"
#include "AABB.h"
#include "Controller.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "PhysicsAABB.h"

#include "GLRenderer.h"
#include "PlayerCam.h"

#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "RenderComponent.h"
#include "ClientPlayerLC.h"
#include "ConnectCommand.h"
#include "OnlinePlayerLC.h."
#include "Stage.h"
#include "HeadParticleLC.h"
#include "PositionComponent.h"

#include "ServerClientData.h"
#include "RandomUtil.h"
#include "ZombieGC.h"
#include "StartCommand.h"
#include "SpawnZombieCommand.h"
#include "KillCommand.h"

const int windowWidth = 1920.0 / 1.5;
const int windowHeight = 1080.0 / 1.5;

const int viewWidth = 640;
const int viewHeight = 360;

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

/*
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
*/

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = NULL;
	window = SDL_CreateWindow("Stabby", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Failed.");
		return 1;
	}

	GLRenderer::Init(window, { windowWidth, windowHeight }, {viewWidth, viewHeight});
	RenderSystem render;

	DebugIO::startDebug("SuquaEng0.1/fonts/consolas_0.png", "SuquaEng0.1/fonts/consolas.fnt");
	int debugCamId;
	debugCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowWidth, windowHeight }, .5 });

	PlayerCam playerCam{ viewWidth, viewHeight };
	//playerCam.setZoom(.08);
	int playerCamId = GLRenderer::addCamera(playerCam);

	PartitionID blood = GLRenderer::GenParticleType("blood", 1, {"particles/blood.vert"});
	PartitionID test = GLRenderer::GenParticleType("test", 4, { "particles/test.vert" });
	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(MessageCallback, 0);

	
	EntityId title;
	EntitySystem::GenEntities(1, &title);
	EntitySystem::MakeComps<RenderComponent>(1, &title);
	RenderComponent * titleRender = EntitySystem::GetComp<RenderComponent>(title);
	titleRender->loadSprite<Sprite>("images/tempcover.png");
	EntitySystem::GetComp<PositionComponent>(title)->pos = {-titleRender->getImgRes().x / 2 , -300 };

	GLRenderer::getCamera(playerCamId).pos = { -titleRender->getImgRes().x / 2 , -300 };
	
	/*
	EntityId testComp;
	EntitySystem::GenEntities(1, &testComp);
	EntitySystem::MakeComps<RenderComponent>(1, &testComp);
	EntitySystem::GetComp<RenderComponent>(testComp)->loadSprite<Sprite>("images/redpixel.png");
	*/

	Game game{};
	PhysicsSystem & physics = game.physics;
	Client & client = game.client;

	game.stage.loadGraphics("images/stage.png");
	physics.setStage(&game.stage);

	DebugIO::getCommandManager().registerCommand<StartCommand>(StartCommand{ game });
	DebugIO::getCommandManager().registerCommand<SpawnZombieCommand>();
	DebugIO::getCommandManager().registerCommand<KillCommand>();
	Controller controller;
	/*--------------------------------------------- PostProcessing -------------------------------------------------*/
	Framebuffer fb{};
	unsigned int screenTex;
	fb.bind();
	screenTex = fb.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	fb.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer occlusionMap{};
	unsigned int occlusionTex;
	occlusionMap.bind();
	occlusionTex = occlusionMap.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	occlusionMap.finalizeFramebuffer();
	Framebuffer::unbind();


	/*--------------------------------------------- GAME LOOP -----------------------------------------------*/

	double gfxDelay{ 1.0 / 60 };
	Uint64 currentLog = SDL_GetPerformanceCounter();
	Uint64 currentGfx = SDL_GetPerformanceCounter();

	bool lockFPS{ true }, lockUPS{ true };

	bool running{ true };
	while (running) {
		Uint64 now = SDL_GetPerformanceCounter();
		//logic
		Time_t elapsedTime = now - currentLog;
		bool updateLogic{ false };
		if (lockUPS)
			updateLogic = static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency() >= CLIENT_TIME_STEP;
		else
			updateLogic = true;

		if (updateLogic) {
			game.time++;
			double ups = 1.0 / (static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency());
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
			if (toggleDebug)
				DebugIO::toggleDebug();

			//reset / set scroll
			controller.setMouseScroll(mouseScroll);

			const Uint8 *state = SDL_GetKeyboardState(NULL);

			if (!DebugIO::getOpen()) {
				//update controllers

				controller.set(ControllerBits::UP, state[SDL_SCANCODE_UP]);
				controller.set(ControllerBits::DOWN, state[SDL_SCANCODE_DOWN]);
				controller.set(ControllerBits::LEFT, state[SDL_SCANCODE_LEFT]);
				controller.set(ControllerBits::RIGHT, state[SDL_SCANCODE_RIGHT]);
				controller.set(ControllerBits::BUTTON_1, state[SDL_SCANCODE_Z]);
				controller.set(ControllerBits::BUTTON_2, state[SDL_SCANCODE_X]);
				controller.set(ControllerBits::BUTTON_3, state[SDL_SCANCODE_C]);
			}
			else {
				controller.off(ControllerBits::ALL);
			}

			if (EntitySystem::Contains<ClientPlayerLC>()) {
				ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(game.getPlayerId());
				player->update(game.time, CLIENT_TIME_STEP, controller);
			}

			if (client.getConnected()) {

				//this needs to stay correct even if the loop isn't running. Hence, this is run based off of elapsed times.
				client.progressTime((static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency()) / GAME_TIME_STEP);

				static ControllerPacket lastSent{};

				ControllerPacket state{};
				state.time = game.time;
				state.state = controller.getState();
				state.netId = client.getNetId();
				state.when = client.getTime();

				lastSent.when = client.getTime();
				lastSent.time = game.time;

				//the sent state is the controller state from after the timestamped update has run
				if (lastSent != state) {
					lastSent = state;
					client.send(state);
					//std::cout << "Sending update for time: " << lastSent.when << '\n';
				}

				client.service(game.time);

				if (client.isBehindServer()) {
					std::cout << "We're behind the server, pinging our time.\n";
					client.ping(game.time);
					client.resetBehindServer();
				}

				if (EntitySystem::Contains<OnlinePlayerLC>()) {
					for (auto& onlinePlayer : EntitySystem::GetPool<OnlinePlayerLC>()) {
						onlinePlayer.update(client.getTime());
					}
				}

				static int pingDelay = 0;
				//half a second
				int pingDelayMax = 60;

				if (pingDelay != pingDelayMax) {
					pingDelay++;
				}
				else {
					pingDelay = 0;
					client.ping(game.time);
				}

				DebugIO::setLine(3, "NetId: " + std::to_string(client.getNetId()));
				DebugIO::setLine(4, "Ping: " + std::to_string(client.getPing()));
			}
			else {
				//singleplayer physics / combat

				if (EntitySystem::Contains<ZombieLC>()) {
					for (auto& zombie : EntitySystem::GetPool<ZombieLC>()) {
						zombie.searchForTarget<ClientPlayerLC>();
						zombie.runLogic();
					}
				}
			}

			physics.runPhysics(CLIENT_TIME_STEP);

			game.combat.runAttackCheck<ClientPlayerLC, ZombieLC>();
			game.combat.runAttackCheck<ZombieLC, ClientPlayerLC>();

			if (EntitySystem::Contains<HeadParticleLC>()) {
				Pool<HeadParticleLC> & heads = EntitySystem::GetPool<HeadParticleLC>();
				for (auto& head : heads) {
						head.update(CLIENT_TIME_STEP);
				}
			}
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

			fb.bind();

			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			GLRenderer::ClearRenderBufs(GLRenderer::all);
			
			//update camera
			const Uint8 *state = SDL_GetKeyboardState(NULL);

			unsigned int debugTextBuffer = DebugIO::getRenderBuffer();
			GLRenderer::SetDefShader(ImageShader);
			GLRenderer::setCamera(playerCamId);
			static_cast<PlayerCam &>(GLRenderer::getCamera(playerCamId)).update(game.getPlayerId());

			/*
			RenderComponent * reddot = EntitySystem::GetComp<RenderComponent>(testComp);
			PositionComponent * reddotposition = EntitySystem::GetComp<PositionComponent>(testComp);
			if (EntitySystem::Contains<ZombieLC>()) {
				ZombieLC & zombie = EntitySystem::GetPool<ZombieLC>().front();
				const AABB * hitbox = zombie.getHurtboxes(nullptr);
				if (hitbox != nullptr) {
					reddot->setScale({hitbox->res.x, hitbox->res.y});
					reddotposition->pos = hitbox->pos;
				}
			}
			*/

			render.draw<RenderComponent>();
			render.draw<PlayerGC>();
			render.draw<ZombieGC>();

			GLRenderer::Draw(GLRenderer::exclude, 1, &debugTextBuffer);

			RenderComponent * stageRender = EntitySystem::GetComp<RenderComponent>(game.stage.getId());
			if (stageRender != nullptr) {
				unsigned int stageRenderBuffer = stageRender->getRenderBufferId();
				occlusionMap.bind();
				GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
				GLRenderer::Draw(GLRenderer::include, 1, &stageRenderBuffer);
			}
			fb.bind();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, occlusionMap.getTexture(0).id);

			GLRenderer::getComputeShader("blood").use();
			GLRenderer::getComputeShader("blood").uniform2f("camPos", GLRenderer::getCamera(playerCamId).pos.x, GLRenderer::getCamera(playerCamId).pos.y);
			GLRenderer::getComputeShader("blood").uniform1f("zoom", GLRenderer::getCamera(playerCamId).camScale);

			GLRenderer::UpdateAndDrawParticles();

			Framebuffer::unbind();
			GLRenderer::SetDefShader(FullscreenShader);
			GLRenderer::bindCurrShader();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			
			GLRenderer::DrawOverScreen(fb.getTexture(0).id);

			GLRenderer::setCamera(debugCamId);
			GLRenderer::SetDefShader(DebugShader);
			DebugIO::drawLines(windowHeight);

			GLRenderer::Swap();
			GLRenderer::ReadErrors();
		}
	}

	DebugIO::stopDebug();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}