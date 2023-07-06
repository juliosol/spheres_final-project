#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "ShaderManager.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

void update_scene(scene_container& scene, float deltaTime, float time);

namespace update{
    int redSphere = -1;
}

int redSphereIndex = -1;
const glm::quat redSphere_pitch = glm::quat(glm::vec3(glm::radians(15.f), 0,0));

// Initializatin function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h){
    // Initialization flag
    bool success = true;
    // String to hold any errors that occur
    std::stringstream errorStream;
    // The window we'll be rendering to
    m_window = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        success = false;
    } else {
        //Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                w,
                                h,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

        // Check if Window did not create
        if (m_window == NULL){
            errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }

        // Create an OpenGL Graphics Context
        m_openGLContext = SDL_GL_CreateContext( m_window );
        if ( m_openGLContext == NULL){
            errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }

        // Initialize GLAD Library
        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            errorStream << "Failed to initialize GLAD\n";
            success = false;
        }

        // Initialize OpenGL
        if (!InitGL()){
            errorStream << "Unable to initialize OpenGL!\n";
            success = false;
        }
    }

    // If initialization did not work, then print out a list of errors in the constructor.
    if (!success) {
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n", errors.c_str());
    } else {
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

    // SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
    GetOpenGLVersionInfo();

    // Setup our Renderer
    m_renderer = new Renderer(w,h);

    // Initializing shader manager
    m_shaderMng = new ShaderManager(w, h, false);
    m_shaderMng->init_window();
}

// Shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    if (m_renderer!=nullptr){
        delete m_renderer;
    }

    // Destroy window 
    SDL_DestroyWindow( m_window );
    // Point m_window to NULL to ensure it points to nothing.
    m_window = nullptr;
    // Quit SDL subsystems
    SDL_Quit();
}

// Initialize OpenGL
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	return success;
}

// Loops forever!
void SDLGraphicsProgram::Loop(){

    // Set a default position for our camera
    m_renderer->GetCamera(0)->SetCameraEyePosition(0.0f,0.0f,20.0f);

    scene_container scene = {};
    
    scene.scene = SceneManager::create_scene(m_renderer->m_screenWidth, m_renderer->m_screenHeight);
    scene.scene.camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    scene.shadow_ambient = glm::vec3{0.1, 0.1, 0.1};
    scene.ambient_color = glm::vec3{0.025, 0.025, 0.025};

    // lights
	scene.lights_point.push_back(SceneManager::create_light_point({ 3, 5, 0, 0.1 }, { 1, 1, 1 }, 25.5));
	scene.lights_direct.push_back(SceneManager::create_light_direct({ 3, -1, 1 }, { 1, 1, 1 }, 1.5));

    // small sphere 1
	scene.spheres.push_back(SceneManager::create_sphere({ -2, 0, 1 }, 0.5,
		SceneManager::create_material({ 0, 0.87, 1 }, 100, 0.95)));
    // small sphere 2
	scene.spheres.push_back(SceneManager::create_sphere({ 5, 0, -1 }, 3,
		SceneManager::create_material({ 1, 0, 0.45 }, 50, 0.35)));
	// blue sphere
	scene.spheres.push_back(SceneManager::create_sphere({ 2, 0, 6 }, 1,
		SceneManager::create_material({ 0, 0, 1 }, 50, 0.35)));
    // red sphere
	scene.spheres.push_back(SceneManager::create_sphere({ -1, 0, 6 }, 1,
		SceneManager::create_material({ 1, 0, 0 }, 100, 0.1), true));
    // transparent sphere
	scene.spheres.push_back(SceneManager::create_sphere({ 0.5, 2, 6 }, 1,
		SceneManager::create_material({ 1, 1, 1 }, 200, 0.1, 1.125, { 1, 0, 2 }, 1), true)); // 
    // New sphere
    scene.spheres.push_back(SceneManager::create_sphere({ -5, 1, 10 }, 2,
		SceneManager::create_material({ 1, 1, 0 }, 50, 0.1), true)); // , 1.125, { 1, 0, 2 }, 1

    rt_defines defines = scene.get_defines();

    m_shaderMng->init_shaders(defines);

    std::vector<std::string> faces =
	{
		"./textures/skybox/GalaxyTex_PositiveX.jpg",
		"./textures/skybox/GalaxyTex_NegativeX.jpg",
		"./textures/skybox/GalaxyTex_PositiveY.jpg",
		"./textures/skybox/GalaxyTex_NegativeY.jpg",
		"./textures/skybox/GalaxyTex_PositiveZ.jpg",
		"./textures/skybox/GalaxyTex_NegativeZ.jpg"
	};

    unsigned cubeMapId = m_shaderMng->load_cubemap(faces, false);
    m_shaderMng->set_background(cubeMapId);

    SceneManager scene_manager(this->m_renderer->m_screenWidth, this->m_renderer->m_screenHeight, &scene, m_shaderMng);
	scene_manager.init();

    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 5.0f;

    float currentTime = static_cast<float>(SDL_GetTicks());
	float lastFramesPrint = currentTime;
	float framesCount = 0;

    // While application is running
    while(!quit){

        //Handle events on queue
        while(SDL_PollEvent( &e ) != 0){
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE){
                quit = true;
            }
            // Handle keyboard input for the camera class
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                scene_manager.updateMouse(e.motion.x, e.motion.y);
            }
            switch(e.type){
                // Handle keyboard presses
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                        case SDLK_LEFT:
                            //m_renderer->GetCamera(0)->MoveLeft(cameraSpeed);
                            scene_manager.left_pressed = true;
                            break;
                        case SDLK_RIGHT:
                            //m_renderer->GetCamera(0)->MoveRight(cameraSpeed);
                            scene_manager.right_pressed = true;
                            break;
                        case SDLK_UP:
                            //m_renderer->GetCamera(0)->MoveForward(cameraSpeed);
                            scene_manager.up_pressed = true;
                            break;
                        case SDLK_DOWN:
                            //m_renderer->GetCamera(0)->MoveBackward(cameraSpeed);
                            scene_manager.down_pressed = true;
                            break;
                    }
                break;
            }
        } // End SDL_PollEvent loop.

        framesCount++;
		float newTime = static_cast<float>(SDL_GetTicks());
        float deltaTime = newTime - currentTime;
		currentTime = newTime;

		if (newTime - lastFramesPrint > 1200.0f)
		{
			std::cout << "FPS: " << framesCount << std::endl;
			lastFramesPrint = newTime;
			framesCount = 0;
		}

        //update_scene(scene, deltaTime, newTime);
		scene_manager.update(deltaTime);
        m_shaderMng->draw();
        scene_manager.reset_directions();
        // Delay to slow things down just a bit!
        //SDL_Delay(25);  // : You can change this or implement a frame
                        // independent movement method if you like.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    //Disable text input
    SDL_StopTextInput();
}

void update_scene(scene_container& scene, float deltaTime, float time){
    if (redSphereIndex != -1){
        rt_sphere* redSphere = &scene.spheres[redSphereIndex];
        const float redSphereSpeed = 0.03;
        redSphere->obj.x = cos(time*redSphereSpeed) * 20000;
        redSphere->obj.z = cos(time*redSphereSpeed) * 20000;
        redSphere->quat_rotation *= glm::angleAxis(deltaTime / 15, glm::vec3(0,1,0));
    }
}

// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
    return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
