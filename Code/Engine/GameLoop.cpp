#include <Engine/EngineProperties.h>
#define STB_IMAGE_IMPLEMENTATION
#include <Utility/stb_image.h>
#include <Engine/Log.h>
#include <GL/glew.h>
#include "Timer.h"
#include <cmath>
#include <Rendering/Utility/Framebuffer.h>
#include "Math/Math.h"
#if IS_IN_EDITOR
#include "UI/EditorUI/EditorUI.h"
#endif
#include <UI/Default/ScrollObject.h>
#include "Scene.h"
#include <UI/Default/TextRenderer.h>
#include "Rendering/Mesh/Model.h"
#include "Input.h"
#include <Sound/Sound.h>
#include "Rendering/Utility/CSM.h"
#include <Rendering/Utility/ShaderManager.h>
#include <Engine/Script.h>
#include <Engine/Console.h>
#include <Rendering/Utility/SSAO.h>
#include <Rendering/Camera/CameraShake.h>
#include <UI/Default/UICanvas.h>
#include <Engine/Save.h>
#include <Rendering/Camera/FrustumCulling.h>
#include <Objects/Objects.h>
#include <Engine/OS.h>
#include <Rendering/Particle.h>
#include <cmath>

#include <World/Assets.h>
#include <World/Graphics.h>
#include <World/Stats.h>

namespace Input
{
	extern bool Keys[351];
}

class InitError : public std::exception
{
public:
	InitError(std::string Error)
	{
		Exception = "Error while initializing: " + Error;
	}

	virtual const char* what() const throw()
	{
		return Exception.c_str();
	}

	std::string Exception;
};

SDL_Window* Window;
std::vector<ButtonEvent> ButtonEvents;
Vector2 GetMousePosition()
{
	int x;
	int y;
	SDL_GetMouseState(&x, &y);
	return Vector2((x / Graphics::WindowResolution.X - 0.5f) * 2, 1 - (y / Graphics::WindowResolution.Y * 2));
}
bool ShouldClose = false;
namespace Application
{
	void Quit()
	{
		ShouldClose = true;
	}
}

bool ShouldIgnoreErrors = false;
void GLAPIENTRY
MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
)
{
	if ((type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR || type == GL_DEBUG_TYPE_PORTABILITY) && !ShouldIgnoreErrors)
	{
		Log::CreateNewLogMessage(message + std::string(" Status: ") + Debugging::EngineStatus);
	}
}

int Start(int argc, char** argv)
{
	OS::SetConsoleWindowVisible(true);
	Assets::ScanForAssets();
	std::cout << "Starting\n";
	Debugging::EngineStatus = "Initializing";
	Uint64 PerfCounterFrequency = SDL_GetPerformanceFrequency();
	Uint64 LastCounter = SDL_GetPerformanceCounter();
	std::cout << "*Loading Configs\n";
	if (IS_IN_EDITOR)
	{
		Console::ExecuteConsoleCommand("define IS_IN_EDITOR = true");
	}
	else
	{
		Console::ExecuteConsoleCommand("define IS_IN_EDITOR = false");
	}
	Config::LoadConfigs();
	Console::ExecuteConsoleCommand("endif");
	std::cout << "*Starting SDL2 - ";

	int Errstring = SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	int flags;

	if (Errstring != 0)
	{
		std::cout << "Could not start SDL2 (" << SDL_GetError() << ")\n";
	}
	else
		std::cout << "SDL2 started (No error)\n";
	if (IsInEditor)
	{
		flags = SDL_WINDOW_OPENGL;
	}
	else
	{
		flags = SDL_WINDOW_OPENGL;
	}
	Window = SDL_CreateWindow((IS_IN_EDITOR ? ProjectName + std::string(" Editor") : ProjectName).c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Graphics::WindowResolution.X, Graphics::WindowResolution.Y, flags);
	SDL_GLContext GLContext = SDL_GL_CreateContext(Window);
	SDL_SetWindowResizable(Window, SDL_TRUE);
	std::cout << "*Starting GLEW - ";
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Init Error:\n" << glewGetErrorString(glewInit());
		//SDL_DestroyWindow(Window);
		std::cout << "\n-Press Enter to continue-";
		std::cin.get();
		return -1;
	}
	if (!glewIsSupported(OPENGL_MIN_REQUIRED_VERSION))
	{
		throw InitError(std::string("OpenGL version ")
			+ std::string((const char*)glGetString(GL_VERSION))
			+ std::string(" is not supported. Minimum: ") + OPENGL_MIN_REQUIRED_VERSION);
	}


	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	std::cout << "GLEW started (" << glewGetErrorString(glewInit()) << ")\n";
	Debugging::EngineStatus = "Starting OpenAL";
	std::cout << "*Starting OpenAL\n";
	Sound::Init();
	std::cout << "*Compiling default shaders";
	Debugging::EngineStatus = "Compiling Shaders";
	Shader MainShader("Shaders/basic.vert", "Shaders/basic.frag");
	MainShader.Bind();
	std::cout << ".";
	Graphics::MainShader = &MainShader;
	Shader ShadowShader("Shaders/shadow.vert", "Shaders/shadow.frag", "Shaders/shadow.geom");
	Graphics::ShadowShader = &ShadowShader;
	std::cout << ".";

	Shader TextShader("Shaders/text.vert", "Shaders/text.frag");
	std::cout << ".";
	Graphics::TextShader = &TextShader;


	Shader PostProcessShader("Shaders/postprocess.vert", "Shaders/postprocess.frag");
	std::cout << ".";
	Shader UIShader("Shaders/uishader.vert", "Shaders/uishader.frag");
	std::cout << ".";
	Graphics::UIShader = &UIShader;
	Shader BloomShader("Shaders/postprocess.vert", "Shaders/bloom.frag");
	std::cout << ".";
	std::cout << " done!";
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	std::cout << "\n*Starting Logic";
	Debugging::EngineStatus = "Starting Logic";
	std::cout << ".";

	for (int i = 0; i < 322; i++) // Array of keys, should be 'false' in the beginning
	{
		Input::Keys[i] = false;
	}
	std::cout << ".";
	Graphics::MainCamera = World::DefaultCamera;

	bool ButtonW = false;
	bool ButtonA = false;
	bool ButtonS = false;
	bool ButtonD = false;
	bool ButtonQ = false;
	bool ButtonE = false;
	bool RMouseLock = false;
	std::cout << ".";

	Uint64 PerfCounterFrequencyLastCounterLogic = SDL_GetPerformanceFrequency();
	Uint64 LastCounterLogic = SDL_GetPerformanceCounter();

	Uint64 PerfCounterFrequencyLastCounterRender = SDL_GetPerformanceFrequency();
	Uint64 LastCounterRender = SDL_GetPerformanceCounter();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::cout << ".";


	SDL_SetRelativeMouseMode(SDL_TRUE);
	std::cout << "              done!\n";
	std::cout << "*Generating Frame Buffers";
	Debugging::EngineStatus = "Generating Frame Buffers";
	std::cout << ".";
	Graphics::MainFramebuffer = new FramebufferObject();
	Graphics::MainFramebuffer->FramebufferCamera = Graphics::MainCamera;
	std::cout << ".";
	Framebuffer StencilFrameBuffer(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
	{
		std::cout << ".";
		Debugging::EngineStatus = "Generating Frame Buffers: SSAO: pos";

		glBindFramebuffer(GL_FRAMEBUFFER, Graphics::FBO::ssaoFBO);

		// - position color buffer
		glGenTextures(1, &Graphics::FBO::SSAOBuffers[1]);
		glBindTexture(GL_TEXTURE_2D, Graphics::FBO::SSAOBuffers[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Graphics::WindowResolution.X, Graphics::WindowResolution.Y,
			0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		Graphics::MainFramebuffer->GetBuffer()->AttachFramebuffer(Graphics::FBO::SSAOBuffers[1], GL_COLOR_ATTACHMENT3);
		Debugging::EngineStatus = "Generating Frame Buffers: SSAO: norm";

		// - normal color buffer
		glGenTextures(1, &Graphics::FBO::SSAOBuffers[2]);
		glBindTexture(GL_TEXTURE_2D, Graphics::FBO::SSAOBuffers[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Graphics::WindowResolution.X, Graphics::WindowResolution.Y,
			0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Graphics::FBO::SSAOBuffers[2], 0);
		Graphics::MainFramebuffer->GetBuffer()->AttachFramebuffer(Graphics::FBO::SSAOBuffers[2], GL_COLOR_ATTACHMENT2);

		Debugging::EngineStatus = "Generating Frame Buffers: SSAO - Stage 2";
		std::cout << ".";
		SSAO::Init();
	}
	Debugging::EngineStatus = "Generating Frame Buffers: CSM";
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
	glGenTextures(2, pingpongBuffer);
	std::cout << ".";
	CSM::Init();
	std::cout << ".";
	bool IsWindowFullscreen = false;
	unsigned int matricesUBO;
	glGenBuffers(1, &matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	Debugging::EngineStatus = "Generating Frame Buffers: Bloom";
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, Graphics::WindowResolution.X, Graphics::WindowResolution.Y, 0,
			GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}
	std::cout << ".";
	int w, h;
	SDL_GetWindowSize(Window, &w, &h);
	Graphics::SetWindowResolution(Vector2(w, h));
	Graphics::MainFramebuffer->GetBuffer()->ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
	StencilFrameBuffer.ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);

#if IS_IN_EDITOR
	Debugging::EngineStatus = "Generating Frame Buffers: EditorSpecific";

	Framebuffer ArrowFrameBuffer(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
	Model* ArrowsModel = new Model(std::string("EngineContent/Models/Arrows.jsm"));
	ArrowFrameBuffer.ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
#endif
#if ENGINE_DEBUG
	TextRenderer DebugTextRenderer = TextRenderer("Fonts/Font.ttf", 60);
#endif //ENGINE_DEBUG

	std::cout << " done!\n";
#if IS_IN_EDITOR
	EngineUI* EditorUI = new EngineUI();
	Graphics::UIToRender.push_back(EditorUI);
#endif

	std::cout << "*Loading Startup Map\n";
	Debugging::EngineStatus = "Loading Startup Map";
	if (StartupMap != std::string(""))
	{
#if IS_IN_EDITOR
		World::LoadNewScene(StartupMap);
		EditorUI->UpdateContentBrowser();
#else
		SaveGame MainSave = std::string("Main");
		if (MainSave.SaveGameIsNew())
		{
			World::LoadNewScene(StartupMap);
		}
		else
		{
			World::LoadNewScene(MainSave.GetPropterty("CurrentMap").Value);
		}
#endif

	}

	Console::ExecuteConsoleCommand("info");
	Uint64 EndCounter = SDL_GetPerformanceCounter();
	Uint64 counterElapsed = EndCounter - LastCounter;
	float LoadTime = ((float)counterElapsed) / ((float)PerfCounterFrequency);
	Log::CreateNewLogMessage(std::string("Finished loading. (").append(std::to_string(LoadTime).append(" seconds)")), Vector3(1.f, 0.75f, 0.f));
	EndCounter = SDL_GetPerformanceCounter();
	counterElapsed = 0;
	Performance::FPS = 60;
	Performance::DeltaTime = 0.016;
	LastCounter = EndCounter;
	Stats::Time = 0;
	ShouldIgnoreErrors = false;
	bool SlowMode = false;
	bool FastMode = false;
	if (!(ENGINE_DEBUG || IS_IN_EDITOR))
	{
		OS::SetConsoleWindowVisible(false);
	}
	//Main Loop
	while (!ShouldClose)
	{
		Performance::DrawCalls = 0;
		Uint64 LastCounterLogic = SDL_GetPerformanceCounter();
		Debugging::EngineStatus = "Polling for input";
		bool bMouseMoved = false;
		Input::MouseLocation = GetMousePosition();
		SDL_Event event;
		//Poll Events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
#if !IS_IN_EDITOR
				for (WorldObject* o : Objects::AllObjects)
				{
					o->Destroy();
					delete o;
				}
				Objects::AllObjects.clear();
				ShouldClose = true;
#else
				EditorUI->OnLeave(&Application::Quit);
#endif
				}
			else if (event.type == SDL_KEYDOWN)
			{
				std::vector<int> Indices;

				if (event.key.keysym.sym < 128)
					Input::Keys[event.key.keysym.sym] = true;
				else
				{
					int sym = event.key.keysym.sym;
					sym -= 1073741755;
					if (sym > 0)
						Input::Keys[sym] = true;
				}
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					ButtonW = true;
					break;
				case SDLK_a:
					ButtonA = true;
					break;
				case SDLK_s:
					ButtonS = true;
					break;
				case SDLK_d:
					ButtonD = true;
					break;
				case SDLK_q:
					ButtonQ = true;
					break;
				case SDLK_e:
					ButtonE = true;
					break;
				case SDLK_BACKSPACE:
					if (TextInput::PollForText && TextInput::Text.length() > 0)
					{
						TextInput::Text.pop_back();
					}
					break;
				case SDLK_DELETE:
					for (int i = 0; i < Objects::AllObjects.size(); i++)
					{
						if (Objects::AllObjects.at(i)->IsSelected)
						{
							Objects::DestroyObject(Objects::AllObjects[i]);

						}
					}
				case SDLK_ESCAPE:
					for (int i = 0; i < Objects::AllObjects.size(); i++)
					{
						Objects::AllObjects.at(i)->IsSelected = false;
					}
					TextInput::PollForText = false;
					break;
				case SDLK_LSHIFT:
					FastMode = true;
					break;
				case SDLK_LCTRL:
					SlowMode = true;
					break;
				case SDLK_RETURN:
					TextInput::PollForText = false;
					break;
				case SDLK_F11:
					if (IsInEditor)
					{
						if (!IsWindowFullscreen)
							SDL_MaximizeWindow(Window);
						else
							SDL_RestoreWindow(Window);
						IsWindowFullscreen = !IsWindowFullscreen;
					}
					else
					{
						if (!IsWindowFullscreen) SDL_SetWindowFullscreen(Window, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
						else SDL_SetWindowFullscreen(Window, SDL_WINDOW_OPENGL);
						int w, h;
						SDL_GetWindowSize(Window, &w, &h);
						Graphics::SetWindowResolution(Vector2(w, h));
						StencilFrameBuffer.ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
						glDeleteFramebuffers(2, pingpongFBO);
						glDeleteTextures(2, pingpongBuffer);
						glGenFramebuffers(2, pingpongFBO);
						glGenTextures(2, pingpongBuffer);
						for (unsigned int i = 0; i < 2; i++)
						{
							glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
							glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
							glTexImage2D(
								GL_TEXTURE_2D, 0, GL_RGBA16F, Graphics::WindowResolution.X, Graphics::WindowResolution.Y, 0,
								GL_RGBA, GL_FLOAT, NULL
							);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							glFramebufferTexture2D(
								GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
							);
						}
						IsWindowFullscreen = !IsWindowFullscreen;
					}
					break;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
				switch (event.button.button)
				{
				case SDL_BUTTON_RIGHT:
					Input::IsRMBDown = true;
					TextInput::PollForText = false;
					break;
				case SDL_BUTTON_LEFT:
					Input::IsLMBDown = true;
					break;
				}
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym < 128)
					Input::Keys[event.key.keysym.sym] = false;
				else
				{
					int sym = event.key.keysym.sym;
					sym -= 1073741755;
					if (sym > 0)
						Input::Keys[sym] = false;
				}
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					ButtonW = false;
					break;
				case SDLK_a:
					ButtonA = false;
					break;
				case SDLK_s:
					ButtonS = false;
					break;
				case SDLK_d:
					ButtonD = false;
					break;
				case SDLK_q:
					ButtonQ = false;
					break;
				case SDLK_e:
					ButtonE = false;
					break;
				case SDLK_LSHIFT:
					FastMode = false;
					break;
				case SDLK_LCTRL:
					SlowMode = false;
					break;
				}

			}
			else if (event.type == SDL_MOUSEBUTTONUP)
				switch (event.button.button)
				{
				case SDL_BUTTON_RIGHT:
					Input::IsRMBDown = false;
					break;
				case SDL_BUTTON_LEFT:
					Input::IsLMBDown = false;
					break;
				}
			else if (event.type == SDL_MOUSEMOTION)
			{
				if (RMouseLock)
				{
					Graphics::MainCamera->OnMouseMoved(event.motion.xrel, event.motion.yrel);
				}
				Input::MouseMovement = Vector2(event.motion.xrel * 0.1f, -event.motion.yrel * 0.1f);
				bMouseMoved = true;
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					Graphics::SetWindowResolution(Vector2(event.window.data1, event.window.data2));

#if IS_IN_EDITOR
					ArrowFrameBuffer.ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
#endif
					int w, h;
					SDL_GetWindowSize(Window, &w, &h);
					Graphics::SetWindowResolution(Vector2(w, h));
					StencilFrameBuffer.ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
					glDeleteFramebuffers(2, pingpongFBO);
					glDeleteTextures(2, pingpongBuffer);
					glGenFramebuffers(2, pingpongFBO);
					glGenTextures(2, pingpongBuffer);
					for (unsigned int i = 0; i < 2; i++)
					{
						glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
						glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
						glTexImage2D(
							GL_TEXTURE_2D, 0, GL_RGBA16F, Graphics::WindowResolution.X, Graphics::WindowResolution.Y, 0,
							GL_RGBA, GL_FLOAT, NULL
						);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glFramebufferTexture2D(
							GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
						);
					}

				}
			}
			else if (event.type == SDL_TEXTINPUT)
			{
				if (TextInput::PollForText &&
					!(SDL_GetModState() & KMOD_CTRL &&
						(event.text.text[0] == 'c' || event.text.text[0] == 'C' || event.text.text[0] == 'v' || event.text.text[0] == 'V')))
				{
					if (event.text.text[0] >= 32 && event.text.text[0] <= 128)
						TextInput::Text.append(event.text.text);
				}
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				for (ScrollObject* s : Graphics::UI::ScrollObjects)
				{
					if (event.wheel.y < 0)
						s->ScrollUp();
					else
						s->ScrollDown();
				}
			}
		}

		if (!bMouseMoved)
		{
			Input::MouseMovement = Vector2();
		}
		//Input
		if (IsInEditor)
		{
			if (Input::IsRMBDown && !RMouseLock && Maths::IsPointIn2DBox(Vector2(0.7f, 0.8f), Vector2(-0.7f, -0.6f), GetMousePosition()))
				RMouseLock = true;
			if (!Input::IsRMBDown)
				RMouseLock = false;

			if (RMouseLock)
				SDL_SetRelativeMouseMode(SDL_TRUE);
			else
				if (Input::CursorVisible)
					SDL_SetRelativeMouseMode(SDL_FALSE);
				else
					SDL_SetRelativeMouseMode(SDL_TRUE);

			if (RMouseLock)
			{
				float Speed = FastMode ? 175 : (SlowMode ? 15 : 50);
				if (ButtonW)
				{
					Graphics::MainCamera->MoveForward(Performance::DeltaTime * Speed);
				}
				if (ButtonS)
				{
					Graphics::MainCamera->MoveForward(Performance::DeltaTime * -Speed);
				}
				if (ButtonA)
				{
					Graphics::MainCamera->MoveRight(Performance::DeltaTime * -Speed);
				}
				if (ButtonD)
				{
					Graphics::MainCamera->MoveRight(Performance::DeltaTime * Speed);
				}
				if (ButtonQ)
				{
					Graphics::MainCamera->MoveUp(Performance::DeltaTime * -Speed);
				}
				if (ButtonE)
				{
					Graphics::MainCamera->MoveUp(Performance::DeltaTime * Speed);
				}
			}
		}
		else
		{
			if (Input::CursorVisible)
			{
				SDL_SetRelativeMouseMode(SDL_FALSE);
			}
			else
			{
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}

		if (IsInEditor)
		{
			Graphics::MainCamera->Update();
		}

		Debugging::EngineStatus = "Ticking objects";
		Sound::Update();
		CameraShake::Tick();
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glClearColor(0.f, 0.f, 0.f, 1.f);		//Clear color black
		Uint64 EndCounter = SDL_GetPerformanceCounter();
		Uint64 counterElapsed = EndCounter - LastCounterLogic;
		std::vector<Model*> SelectedModels;
		std::vector<int> FinishedTimers;
		for (int i = 0; i < Timer::Internal::Timers.size(); i++)
		{
			if (Timer::Internal::Timers.at(i).RemainingTime <= 0)
			{
				Timer::Internal::Timers.at(i).Function();
				FinishedTimers.push_back(i);
			}
			Timer::Internal::Timers.at(i).RemainingTime -= Performance::DeltaTime;

		}
		for (int i = 0; i < FinishedTimers.size(); i++)
		{
			Timer::Internal::Timers.erase(Timer::Internal::Timers.begin() + FinishedTimers.at(i));
		}

		for (FramebufferObject* b : Graphics::AllFramebuffers)
		{
			for (auto* p : b->ParticleEmitters)
			{
				p->Update(b->FramebufferCamera);
			}
		}

		WorldObject* SelectedObject = nullptr;
		if (!ShouldClose)
		{
			for (int i = 0; i < Objects::AllObjects.size(); i++)
			{
				WorldObject* o = nullptr;
				if (SelectedObject == nullptr && Objects::AllObjects[i]->IsSelected)
				{
					SelectedObject = Objects::AllObjects[i];
				}
				try
				{
					o = Objects::AllObjects.at(i);
					o->Tick();
					if (o)
					{
						o->TickComponents();
					}
				}
				catch (std::exception& e)
				{
					if (o)
						Log::CreateNewLogMessage("Ticking Objects: " + o->GetName() + e.what());
				}
#if IS_IN_EDITOR
				if (Objects::AllObjects.at(i)->IsSelected)
				{
					for (int j = 0; j < Objects::AllObjects.at(i)->GetComponents().size(); j++)
					{
						if (dynamic_cast<MeshComponent*>(Objects::AllObjects.at(i)->GetComponents().at(j)) != nullptr)
						{
							SelectedModels.push_back(dynamic_cast<MeshComponent*>(Objects::AllObjects.at(i)->GetComponents().at(j))->GetModel());
						}
					}
				}
#endif
			}
		}
		for (FramebufferObject* b : Graphics::AllFramebuffers)
		{
			Debugging::EngineStatus = "Rendering (Shadows)";

			FrustumCulling::Active = false;

			glViewport(0, 0, Graphics::ShadowResolution, Graphics::ShadowResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			const auto LightSpaceMatrices = CSM::getLightSpaceMatrices();
			glEnable(GL_DEPTH_TEST);

			glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
			for (size_t i = 0; i < LightSpaceMatrices.size(); ++i)
			{
				glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &LightSpaceMatrices[i]);
			}

			Graphics::IsRenderingShadows = true;
			{
				glBindFramebuffer(GL_FRAMEBUFFER, CSM::LightFBO);
				glClear(GL_DEPTH_BUFFER_BIT);
				for (int j = 0; j < b->Renderables.size(); j++)
				{
					if (b->Renderables.at(j) != nullptr)
					{
						if(b->Renderables[j]->CastShadow)
							b->Renderables.at(j)->SimpleRender(Graphics::ShadowShader);
					}
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			Graphics::IsRenderingShadows = false;
			Debugging::EngineStatus = "Rendering (Main Pass)";
			glEnable(GL_BLEND);

			FrustumCulling::Active = true;
			FrustumCulling::CurrentCameraFrustum = FrustumCulling::createFrustumFromCamera(*Graphics::MainFramebuffer->FramebufferCamera);

			glViewport(0, 0, Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
			b->GetBuffer()->Bind();
			glm::vec4 TransposedSunDirection = glm::vec4((glm::vec3)Graphics::LightRotation, 1.f);
			for (std::pair<ShaderDescription, ShaderElement> s : Shaders)
			{
				if (s.second.Shader != nullptr)
				{
					s.second.Shader->Bind();
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D_ARRAY, CSM::ShadowMaps);
					glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), "farPlane"), CSM::cameraFarPlane);
					glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), "cascadeCount"), CSM::shadowCascadeLevels.size());
					glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_textureres"), Graphics::ShadowResolution);
					glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), "shadowMap"), 1);
					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_biasmodifier"), (Vector3::Dot(
						Vector3::GetForwardVector(Graphics::MainCamera->Rotation),
						Graphics::LightRotation.Normalize())));
					Vector3 CameraForward = Vector3::GetForwardVector(Graphics::MainCamera->Rotation);
					glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_cameraforward"), 1, &CameraForward.X);
					glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_cameraposition"), 1, &Graphics::MainFramebuffer->FramebufferCamera->Position.x);
					glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_shadowQuality"), Graphics::PCFQuality);
					glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_shadows"), 1);
					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "FogFalloff"), Graphics::WorldFog.Falloff);
					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "FogDistance"), Graphics::WorldFog.Distance);
					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "FogMaxDensity"), Graphics::WorldFog.MaxDensity);
					glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), "FogColor"), 1, &Graphics::WorldFog.FogColor.X);

					glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_directionallight.Direction"), 1, &Graphics::WorldSun.Direction.X);
					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_directionallight.Intensity"), Graphics::WorldSun.Intensity);
					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_directionallight.AmbientIntensity"), Graphics::WorldSun.AmbientIntensity);
					glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_directionallight.SunColor"), 1, &Graphics::WorldSun.SunColor.X);
					glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_directionallight.AmbientColor"), 1, &Graphics::WorldSun.AmbientColor.X);

					glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), "u_time"), Stats::Time);
					for (size_t i = 0; i < LightSpaceMatrices.size(); ++i)
					{
						glUniformMatrix4fv(glGetUniformLocation(s.second.Shader->GetShaderID(),
							((std::string("lightSpaceMatrices[") + std::to_string(i)) + "]").c_str()), 1, GL_FALSE, &LightSpaceMatrices.at(i)[0][0]);
					}
					for (size_t i = 0; i < CSM::shadowCascadeLevels.size(); ++i)
					{
						glUniform1fv(glGetUniformLocation(s.second.Shader->GetShaderID(),
							((std::string("cascadePlaneDistances[") + std::to_string(i)) + "]").c_str()), 1, &CSM::shadowCascadeLevels[i]);
					}

					for (int i = 0; i < 16; i++)
					{
						std::string CurrentLight = "u_lights[" + std::to_string(i) + "]";
						if (i < Graphics::Lights.size())
						{
							glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), (CurrentLight + ".Position").c_str()), 1, &Graphics::Lights[i].Position.X);
							glUniform3fv(glGetUniformLocation(s.second.Shader->GetShaderID(), (CurrentLight + ".Color").c_str()), 1, &Graphics::Lights[i].Color.X);
							glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), (CurrentLight + ".Falloff").c_str()), Graphics::Lights[i].Falloff);
							glUniform1f(glGetUniformLocation(s.second.Shader->GetShaderID(), (CurrentLight + ".Intensity").c_str()), Graphics::Lights[i].Intensity);

							glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), (CurrentLight + ".Active").c_str()), 1);
						}
						else
						{
							glUniform1i(glGetUniformLocation(s.second.Shader->GetShaderID(), (CurrentLight + ".Active").c_str()), 0);
						}
					}
				}
			}
			glClearColor(0.0f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (unsigned int i = 0; i < b->Renderables.size(); i++)
			{
				b->Renderables[i]->Render(b->FramebufferCamera);
			}
			Debugging::EngineStatus = "Rendering (Particles)";
			b->GetBuffer()->Bind();
			for (auto* p : b->ParticleEmitters)
			{
				p->Draw(b->FramebufferCamera, b == Graphics::MainFramebuffer);
			}
		}

#if IS_IN_EDITOR

		ArrowFrameBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (SelectedObject)
		{
			ArrowsModel->ModelTransform.Location = SelectedObject->GetTransform().Location;
			ArrowsModel->ModelTransform.Scale =
				Vector3(0.025f * (SelectedObject->GetTransform().Location - Vector3::Vec3ToVector(Graphics::MainCamera->Position)).Length());
			ArrowsModel->ModelTransform.Rotation = Vector3(0, -1.57079633, 0);
			ArrowsModel->UpdateTransform();
			ArrowsModel->Render(Graphics::MainCamera);
		}
		ArrowFrameBuffer.Unbind();
#endif // IsInEditor
		StencilFrameBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if IS_IN_EDITOR

		for (int i = 0; i < SelectedModels.size(); i++)
		{
			if (SelectedModels.at(i) != nullptr)
			{
				glm::vec4 TransposedSunDirection = glm::transpose(glm::inverse(Graphics::MainCamera->getView())) * glm::vec4((glm::vec3)Graphics::LightRotation, 1.0f);
				SelectedModels.at(i)->Render(Graphics::MainCamera);
			}
		}

#endif // IsInEditor
		glDisable(GL_BLEND);

		StencilFrameBuffer.Unbind();
		glDisable(GL_DEPTH_TEST);

		Debugging::EngineStatus = "Rendering (Post process: Bloom)";
		//Blur bloom FBO
		if (Graphics::Bloom)
		{
			bool horizontal = true, first_iteration = true;
			int amount = 5;
			BloomShader.Bind();
			glUniform1i(glGetUniformLocation(BloomShader.GetShaderID(), "FullScreen"), 1);
			for (unsigned int i = 0; i < amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				glUniform1i(glGetUniformLocation(BloomShader.GetShaderID(), "horizontal"), horizontal);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(
					GL_TEXTURE_2D, first_iteration ? Graphics::MainFramebuffer->GetBuffer()->GetTextureID(2) : pingpongBuffer[!horizontal]
				);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		Debugging::EngineStatus = "Rendering (Post process: SSAO)";
		for (int i = 2; i < 5; i++)
		{
			Graphics::FBO::SSAOBuffers[i - 2] = Graphics::MainFramebuffer->GetBuffer()->GetTextureID(i);
		}
		unsigned int SSAOFBO = SSAO::Render();		//<- WARING: SSAO::Render Unbinds current Shader/Framebuffer/Texture
		Debugging::EngineStatus = "Rendering (Post process: Main)";
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Graphics::MainFramebuffer->GetBuffer()->GetTextureID(0));

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, StencilFrameBuffer.GetTextureID(1));
#if IS_IN_EDITOR
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ArrowFrameBuffer.GetTextureID(0));
#endif
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, SSAOFBO);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, Graphics::MainFramebuffer->GetBuffer()->GetTextureID(1));
		PostProcessShader.Bind();
		glUniform1f(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_gamma"), Graphics::Gamma);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "FullScreen"), !IsInEditor);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_texture"), 1);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_outlines"), 2);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_enginearrows"), 3);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_ssao"), 5);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_depth"), 6);

		glUniform1f(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_chrabbsize"), Graphics::ChrAbbSize);
		glUniform1f(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_vignette"), Graphics::Vignette);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_fxaa"), Graphics::FXAA);
		glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_bloom"), Graphics::Bloom);

		if (Graphics::Bloom)
		{
			glUniform1i(glGetUniformLocation(PostProcessShader.GetShaderID(), "u_bloomtexture"), 4);
		}
		glDrawArrays(GL_TRIANGLES, 0, 3);
		PostProcessShader.Unbind();
		UIShader.Bind();
		glEnable(GL_BLEND);
		if (!ShouldClose)
		{
			for (int i = 0; i < Graphics::UIToRender.size(); i++)
			{
				Debugging::EngineStatus = "Rendering (UI)";
				if (Graphics::UIToRender.size() > i)
					Graphics::UIToRender[i]->_RenderUIElements(&UIShader);
				if (Graphics::UIToRender.size() > i)
					Graphics::UIToRender[i]->Render(&UIShader);
			}
#if !IS_IN_EDITOR && ENGINE_DEBUG
			Debugging::EngineStatus = "Rendering (Debug Text)";

			DebugTextRenderer.RenderText("FPS: " + std::to_string((int)Performance::FPS), Vector2(-0.95, 0.9), 1, Vector3(1, 1, 0));
			DebugTextRenderer.RenderText("Delta: " + std::to_string((int)(Performance::DeltaTime * 1000)) + "ms", Vector2(-0.95, 0.85), 1, Vector3(1, 1, 0));
			DebugTextRenderer.RenderText("DrawCalls: " + std::to_string(Performance::DrawCalls), Vector2(-0.95, 0.8), 1, Vector3(1, 1, 0));
			DebugTextRenderer.RenderText("NumObjects: " + std::to_string(Objects::AllObjects.size()), Vector2(-0.95, 0.75), 1, Vector3(1, 1, 0));
			DebugTextRenderer.RenderText("CollisonMeshes: " + std::to_string(Collision::CollisionBoxes.size()), Vector2(-0.95, 0.7), 1, Vector3(1, 1, 0));


#endif

			if (Graphics::UIToRender.size() > 0)
				for (ButtonEvent b : ButtonEvents)
				{
					b.c->OnButtonClicked(b.Index);
				}
			ButtonEvents.clear();
			UIShader.Unbind();

			World::Tick();
		}
		Debugging::EngineStatus = "Rendering (Swapping framebuffers)";
		//SDL_GL_SetSwapInterval(0);
		SDL_GL_SwapWindow(Window);
		Debugging::EngineStatus = "Measuring Delta Time";
		//Measure DeltaTime and FPS
		EndCounter = SDL_GetPerformanceCounter();
		counterElapsed = EndCounter - LastCounter;
		Performance::DeltaTime = ((float)counterElapsed) / ((float)PerfCounterFrequency);
		Performance::DeltaTime *= Performance::TimeMultiplier;
		Performance::FPS = ((float)PerfCounterFrequency) / (float)counterElapsed;
		Performance::DeltaTime = std::min(Performance::DeltaTime, 0.1f);
		LastCounter = EndCounter;
		Stats::Time = Stats::Time + Performance::DeltaTime;
	}
	Sound::End();
	OS::SetConsoleWindowVisible(true);
	return 0;
}