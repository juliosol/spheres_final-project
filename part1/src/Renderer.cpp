#include "Renderer.hpp"

// Sets height and width of our renderer
Renderer::Renderer(unsigned int w, unsigned int h){
    m_screenWidth = w;
    m_screenHeight = h;

    // By default create one camera per render
    Camera* defaultCamera = new Camera();
    // Add our single camera
    m_cameras.push_back(defaultCamera);

    //m_root = nullptr;
}

// Sets the height and width of our renderer
Renderer::~Renderer(){
    // Delete all of our camera pointers
    for (int i=0; i < m_cameras.size(); i++){
        delete m_cameras[i];
    }
}

/*
// Update renderer
void Renderer::Update(){
    // Apply projection matrix which creates perspective
    // First argument is 'field of view'
    // Then perspective
    // Then the near and far clipping plane
    m_projectionMatrix = glm::perspective(45.0f, ((float)m_screenWidth)/((float)m_screenHeight), 0.1f, 512.0f);

    // Perform the update
    if (m_root != nullptr){
        m_root->Update(m_projectionMatrix, m_cameras[0]);
    }
}
*/

// Initialize clear color
// Setup our OpenGL State machine
// Then render the scene
void Renderer::Render(){
    // Tell OpenGL to create a depth (or Z-buffer) for us that is stored in every frame
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    // Background of the screen.
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor(1.0f, 0.0f, 0.2f, 1.f);
    // Clear color bufffer and Depth Buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    

    // Now render our objects from our scenegraph
    //if (m_root != nullptr){
    //    m_root->Draw();
    //}
}

// Determine what the root is of the renderer, so the 
// scene can be drawn.
//void Renderer::setRoot(SceneNode* startingNode){
//    m_root = startingNode;
//}