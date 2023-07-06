/** @file Renderer.hpp 
 *  @brief Renderer is responsible for drawing.
 * 
 *  Renderer is responsible for drawing everything. It
 *  contains a scenegraph node and a camera. We could 
 *  possibly have multiple renderers (if we had multiple
 *  windows for example).
 * 
 *  Each renderer thus has it's own camera.
 * 
 *  @author Julio Soldevilla (Based on Prof. Shah work for assignments)
 * 
 */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>

#include <vector>
#include <cstdio>

#include "Camera.hpp"
//#include "SceneNode.hpp"

class Renderer{
    public:
        // The constructor
        // Sets the width and height of the renderer draws to
        Renderer(unsigned int w, unsigned int h);
        // Destructor
        ~Renderer();
        // Update the scene
        void Update();
        // Render the scene
        void Render();
        // Sets the root of our renderer to some node to
        // draw an entire scene graph
        //void setRoot(SceneNode* startingNode);
        // Returns camera at an index
        Camera*& GetCamera(unsigned int index){
            if (index > m_cameras.size()-1){
                printf("Index out of camera list");
            }
            return m_cameras[index];
        }

        // Screen dimension constants
        int m_screenHeight;
        int m_screenWidth;
    
    protected:
        // One or more cameras camera per Renderer
        std::vector<Camera*> m_cameras;
        // Root scene node
        //SceneNode* m_root;
        // Store the projection matrix for our camera.
        glm::mat4 m_projectionMatrix;

};

#endif
