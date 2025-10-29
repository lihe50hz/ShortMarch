#pragma once
#include "long_march.h"
#include "Scene.h"
#include <memory>

struct CameraObject {
    glm::mat4 screen_to_camera;
    glm::mat4 camera_to_world;
};

class Application {
public:
    Application(grassland::graphics::BackendAPI api = grassland::graphics::BACKEND_API_DEFAULT);

    ~Application();

    void OnInit();
    void OnClose();
    void OnUpdate();
    void OnRender();

    bool IsAlive() const {
        return alive_;
    }

private:
    // Core graphics objects
    std::shared_ptr<grassland::graphics::Core> core_;
    std::unique_ptr<grassland::graphics::Window> window_;

    // Scene management
    std::unique_ptr<Scene> scene_;

    // Camera
    std::unique_ptr<grassland::graphics::Buffer> camera_object_buffer_;

    // Shaders
    std::unique_ptr<grassland::graphics::Shader> raygen_shader_;
    std::unique_ptr<grassland::graphics::Shader> miss_shader_;
    std::unique_ptr<grassland::graphics::Shader> closest_hit_shader_;

    // Rendering
    std::unique_ptr<grassland::graphics::Image> color_image_;
    std::unique_ptr<grassland::graphics::RayTracingProgram> program_;
    bool alive_{ false };

    // +++ MODIFIED FOR CAMERA CONTROLS +++
    void ProcessInput(); // Helper function for keyboard input

    // Event handler for key presses
    void OnKeyEvent(int key, int scancode, int action, int mods);

    glm::vec3 camera_pos_;
    glm::vec3 camera_front_;
    glm::vec3 camera_up_;
    float camera_speed_;

    // Key state tracking
    bool is_w_pressed_{ false };
    bool is_s_pressed_{ false };
    bool is_a_pressed_{ false };
    bool is_d_pressed_{ false };
    bool is_space_pressed_{ false };
    bool is_shift_pressed_{ false };


    void OnMouseMove(double xpos, double ypos); // Mouse event handler
    void OnMouseButton(int button, int action, int mods, double xpos, double ypos); // Mouse button event handler

    float yaw_;
    float pitch_;
    float last_x_;
    float last_y_;
    float mouse_sensitivity_;
    bool first_mouse_; // Prevents camera jump on first mouse input
    bool camera_enabled_; // Whether camera movement is enabled
};
