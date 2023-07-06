/**
 * @file SDLGraphicsProgram.hpp
 * @author Julio Soldevilla
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * Class used for initialization of SDL
 */
#ifndef SDLGRAPHICSPROGRAM_HPP
#define SDLGRAPHICSPROGRAM_HPP

// ================ Libraries ===================
#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL2/SDL.h>
#endif

#include "Renderer.hpp"
#include "ShaderManager.hpp"

// Class that sets up a full graphics program using SDL
class SDLGraphicsProgram{
    public:
        // Constructor
        SDLGraphicsProgram(int w, int h);
        // Destructor
        ~SDLGraphicsProgram();
        // Set OpenGL
        bool InitGL();
        // Loop that runs forever
        void Loop();
        // Get Pointer to Window
        SDL_Window* GetSDLWindow();
        // Helper Function to Query OpenGL information.
        void GetOpenGLVersionInfo();
    
    private:
        // The Renderer responsible for drawing objects
        // in openGL (Or whatever Renderer you choose!)
        Renderer* m_renderer;
        // The window we'll be rendering to
        SDL_Window* m_window;
        // OpenGL context
        SDL_GLContext m_openGLContext;

        // Variabls for buffers
        GLuint quadVAO, quadVBO;
        // Shader manager
        ShaderManager* m_shaderMng;
};

#endif

