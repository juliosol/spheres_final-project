/**
 * @file SceneManager.hpp
 * @author Julio Soldevilla
 * @brief File used to manage a scene
 * 
 */

#include "Scene.hpp"
#include "ShaderManager.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

class SceneManager{
    public: 
    
        SceneManager(int wind_wdith, int wind_height, scene_container* scene, ShaderManager* shaderMng);

        void init();
        void update(float frameRate);

        static rt_material create_material(glm::vec3 color, int specular, float reflect, float refract = 0.0, glm::vec3 absorb = {}, float diffuse = 0.7, float kd = 0.8, float ks = 0.2);
        static rt_sphere create_sphere(glm::vec3 center, float radius, rt_material material, bool hollow = false);
        static rt_light_point create_light_point(glm::vec4 position, glm::vec3 color, float intensity, float linear_k = 0.22f, float quadratic_k = 0.2f);
        static rt_light_direct create_light_direct(glm::vec3 direction, glm::vec3 color, float intensity);
        static rt_scene create_scene(int width, int height);

        // Camera position values we want to change
        bool up_pressed = false;
        bool left_pressed = false;
        bool down_pressed = false;
        bool right_pressed = false;

        float lastX = 0;
        float lastY = 0;

        bool firstMouse = true;
        
        void reset_directions(){
            up_pressed = false;
            left_pressed = false;
            down_pressed = false;
            right_pressed = false;
        }
        
        void updateMouse(float newX, float newY);

    private:
        scene_container* scene;
        ShaderManager* shaderMng;

        int wind_width;
        int wind_height;

        // Camera Attributes
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 world_up = glm::vec3(0, 1, 0);
        // Euler Angles
        float yaw = 0;
        float pitch = 0;

        GLuint sceneUbo = 0;
        GLuint sphereUbo = 0;
        GLuint lightPointUbo = 0;
        GLuint lightDirectUbo = 0;

        void update_scene(float deltaTime);
        void init_buffers();
        void update_buffers() const;
        glm::vec3 get_color(float r, float g, float b);

        template<typename T>
        void init_buffer(GLuint* ubo, const char* name, int bindingPoint, std::vector<T>& v);
        template<typename T>
        void update_buffer(GLuint ubo, std::vector<T>& v) const;
};


