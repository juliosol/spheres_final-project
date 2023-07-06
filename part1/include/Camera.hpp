/**
 * @file Camera.hpp
 * @author Julio Soldevilla
 * @brief Sets up an OpenGL camera
 * 
 * Sets up an OpenGL Camera. The camera is
 * what sets up our 'view' matrix.
 * 
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"

class Camera{
    public:
        // Constructor to create a camera
        Camera();
        // Return a 'view' matrix with our 
        // camera transformation applied
        glm::mat4 GetWorldToViewmatrix() const;
        // Move Camera around
        void MouseLook(int mouseX, int mouseY);
        void MoveForward(float speed);
        void MoveBackward(float speed);
        void MoveLeft(float speed);
        void MoveRight(float speed);
        void MoveUp(float speed);
        void MoveDown(float speed);
        // Set the position for the camera
        void SetCameraEyePosition(float x, float y, float z);
        // Returns the Camera X Position where the eye is
        float GetEyeXPosition();
        // Returns the Camer Y Position where the eye is
        float GetEyeYPosition();
        // Returns the Camera Z Position where the eye is
        float GetEyeZPosition();
        // Returns the X 'view' direction
        float GetViewXDirection();
        // Returns the Y 'view' direction
        float GetViewYDirection();
        // Returns the Z 'view' direction
        float GetViewZDirection();

    private:
        // Track the old mouse position
        glm::vec2 m_oldMousePosition;
        // Where is the camera positioned
        glm::vec3 m_eyePosition;
        // What direction is the camera looking
        glm::vec3 m_viewDirection;
        // Which direction is 'up' in our world
        glm::vec3 m_upVector;
};

#endif