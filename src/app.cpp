#include "app.h"
#include "Material.h"
#include "Entity.h"

#include "glm/gtc/matrix_transform.hpp"

namespace {
#include "built_in_shaders.inl"
}

Application::Application(grassland::graphics::BackendAPI api) {
    grassland::graphics::CreateCore(api, grassland::graphics::Core::Settings{}, &core_);
    core_->InitializeLogicalDeviceAutoSelect(true);

    grassland::LogInfo("Device Name: {}", core_->DeviceName());
    grassland::LogInfo("- Ray Tracing Support: {}", core_->DeviceRayTracingSupport());
}

Application::~Application() {
    core_.reset();
}

// Event handler for keyboard input
// We assume GLFW-like action codes: 1 for PRESS, 0 for RELEASE.
// We assume ASCII-like key codes: 87 for W, 83 for S, 65 for A, 68 for D.
void Application::ProcessInput() {
    // Only process input if camera is enabled
    if (!camera_enabled_) {
        return;
    }

    // Move forward
    if (is_w_pressed_) {
        camera_pos_ += camera_speed_ * camera_front_;
    }
    // Move backward
    if (is_s_pressed_) {
        camera_pos_ -= camera_speed_ * camera_front_;
    }
    // Strafe left
    if (is_a_pressed_) {
        camera_pos_ -= glm::normalize(glm::cross(camera_front_, camera_up_)) * camera_speed_;
    }
    // Strafe right
    if (is_d_pressed_) {
        camera_pos_ += glm::normalize(glm::cross(camera_front_, camera_up_)) * camera_speed_;
    }
    // Move up (Space)
    if (is_space_pressed_) {
        camera_pos_ += camera_speed_ * camera_up_;
    }
    // Move down (Shift)
    if (is_shift_pressed_) {
        camera_pos_ -= camera_speed_ * camera_up_;
    }
}

// Event handler for mouse movement
void Application::OnMouseMove(double xpos, double ypos) {
    // Only process mouse movement if camera is enabled
    if (!camera_enabled_) {
        return;
    }

    if (first_mouse_) {
        last_x_ = (float)xpos;
        last_y_ = (float)ypos;
        first_mouse_ = false;
        return;
    }

    float xoffset = (float)xpos - last_x_;
    float yoffset = last_y_ - (float)ypos; // Reversed since y-coordinates go from bottom to top
    last_x_ = (float)xpos;
    last_y_ = (float)ypos;

    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    // Constrain pitch to avoid flipping
    if (pitch_ > 89.0f)
        pitch_ = 89.0f;
    if (pitch_ < -89.0f)
        pitch_ = -89.0f;

    // Recalculate the camera_front_ vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    camera_front_ = glm::normalize(front);
}

// Event handler for mouse button clicks
void Application::OnMouseButton(int button, int action, int mods, double xpos, double ypos) {
    const int BUTTON_RIGHT = 1; // Right mouse button
    const int ACTION_PRESS = 1;

    if (button == BUTTON_RIGHT && action == ACTION_PRESS) {
        // Toggle camera mode
        camera_enabled_ = !camera_enabled_;
        
        if (camera_enabled_) {
            // Entering camera mode - hide cursor and grab it
            glfwSetInputMode(window_->GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            first_mouse_ = true; // Reset to prevent jump
            grassland::LogInfo("Camera mode enabled");
        } else {
            // Exiting camera mode - show cursor
            glfwSetInputMode(window_->GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            grassland::LogInfo("Camera mode disabled - cursor visible");
        }
    }
}

// Process keyboard input each frame
void Application::OnKeyEvent(int key, int scancode, int action, int mods) {
    const int ACTION_PRESS = 1;
    const int ACTION_RELEASE = 0;

    const int KEY_W = 87;     // W
    const int KEY_S = 83;     // S
    const int KEY_A = 65;     // A
    const int KEY_D = 68;     // D
    const int KEY_SPACE = 32; // Space
    const int KEY_SHIFT = 340; // Left Shift (GLFW_KEY_LEFT_SHIFT)

    if (key == KEY_W) {
        if (action == ACTION_PRESS) {
            is_w_pressed_ = true;
        }
        else if (action == ACTION_RELEASE) {
            is_w_pressed_ = false;
        }
    }
    else if (key == KEY_S) {
        if (action == ACTION_PRESS) {
            is_s_pressed_ = true;
        }
        else if (action == ACTION_RELEASE) {
            is_s_pressed_ = false;
        }
    }
    else if (key == KEY_A) {
        if (action == ACTION_PRESS) {
            is_a_pressed_ = true;
        }
        else if (action == ACTION_RELEASE) {
            is_a_pressed_ = false;
        }
    }
    else if (key == KEY_D) {
        if (action == ACTION_PRESS) {
            is_d_pressed_ = true;
        }
        else if (action == ACTION_RELEASE) {
            is_d_pressed_ = false;
        }
    }
    else if (key == KEY_SPACE) {
        if (action == ACTION_PRESS) {
            is_space_pressed_ = true;
        }
        else if (action == ACTION_RELEASE) {
            is_space_pressed_ = false;
        }
    }
    else if (key == KEY_SHIFT) {
        if (action == ACTION_PRESS) {
            is_shift_pressed_ = true;
        }
        else if (action == ACTION_RELEASE) {
            is_shift_pressed_ = false;
        }
    }
}



void Application::OnInit() {
    alive_ = true;
    core_->CreateWindowObject(1280, 720,
        ((core_->API() == grassland::graphics::BACKEND_API_VULKAN) ? "[Vulkan]" : "[D3D12]") +
        std::string(" Ray Tracing Scene Demo"),
        &window_);

    // Register the key event handler
    window_->KeyEvent().RegisterCallback(
        [this](int key, int scancode, int action, int mods) {
            this->OnKeyEvent(key, scancode, action, mods);
        }
    );
    // Register the mouse move event handler
    window_->MouseMoveEvent().RegisterCallback(
        [this](double xpos, double ypos) {
            this->OnMouseMove(xpos, ypos);
        }
    );
    // Register the mouse button event handler
    window_->MouseButtonEvent().RegisterCallback(
        [this](int button, int action, int mods, double xpos, double ypos) {
            this->OnMouseButton(button, action, mods, xpos, ypos);
        }
    );

    // Initialize camera as enabled and hide cursor
    camera_enabled_ = true;
    glfwSetInputMode(window_->GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create scene
    scene_ = std::make_unique<Scene>(core_.get());

    // Add entities to the scene
    // Ground plane - a cube scaled to be flat
    {
        auto ground = std::make_shared<Entity>(
            "meshes/cube.obj",
            Material(glm::vec3(0.8f, 0.8f, 0.8f), 0.8f, 0.0f),
            glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), 
                      glm::vec3(10.0f, 0.1f, 10.0f))
        );
        scene_->AddEntity(ground);
    }

    // Red sphere (using octahedron as sphere substitute)
    {
        auto red_sphere = std::make_shared<Entity>(
            "meshes/octahedron.obj",
            Material(glm::vec3(1.0f, 0.2f, 0.2f), 0.3f, 0.0f),
            glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.5f, 0.0f))
        );
        scene_->AddEntity(red_sphere);
    }

    // Green metallic sphere
    {
        auto green_sphere = std::make_shared<Entity>(
            "meshes/octahedron.obj",
            Material(glm::vec3(0.2f, 1.0f, 0.2f), 0.2f, 0.8f),
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f))
        );
        scene_->AddEntity(green_sphere);
    }

    // Blue cube
    {
        auto blue_cube = std::make_shared<Entity>(
            "meshes/cube.obj",
            Material(glm::vec3(0.2f, 0.2f, 1.0f), 0.5f, 0.0f),
            glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.5f, 0.0f))
        );
        scene_->AddEntity(blue_cube);
    }

    // Build acceleration structures
    scene_->BuildAccelerationStructures();

    core_->CreateBuffer(sizeof(CameraObject), grassland::graphics::BUFFER_TYPE_DYNAMIC, &camera_object_buffer_);

    // Initialize camera state member variables
    camera_pos_ = glm::vec3{ 0.0f, 1.0f, 5.0f };
    camera_up_ = glm::vec3{ 0.0f, 1.0f, 0.0f }; // World up
    camera_speed_ = 0.01f;

    // Initialize new mouse/view variables
    yaw_ = -90.0f; // Point down -Z
    pitch_ = 0.0f;
    last_x_ = (float)window_->GetWidth() / 2.0f;
    last_y_ = (float)window_->GetHeight() / 2.0f;
    mouse_sensitivity_ = 0.1f;
    first_mouse_ = true;

    // Calculate initial camera_front_ based on yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    camera_front_ = glm::normalize(front);

    // Set initial camera buffer data
    CameraObject camera_object{};
    camera_object.screen_to_camera = glm::inverse(
        glm::perspective(glm::radians(60.0f), (float)window_->GetWidth() / (float)window_->GetHeight(), 0.1f, 10.0f));
    camera_object.camera_to_world =
        glm::inverse(glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_));
    camera_object_buffer_->UploadData(&camera_object, sizeof(CameraObject));

    core_->CreateImage(window_->GetWidth(), window_->GetHeight(), grassland::graphics::IMAGE_FORMAT_R32G32B32A32_SFLOAT,
        &color_image_);

    core_->CreateShader(GetShaderCode("shaders/shader.hlsl"), "RayGenMain", "lib_6_3", &raygen_shader_);
    core_->CreateShader(GetShaderCode("shaders/shader.hlsl"), "MissMain", "lib_6_3", &miss_shader_);
    core_->CreateShader(GetShaderCode("shaders/shader.hlsl"), "ClosestHitMain", "lib_6_3", &closest_hit_shader_);
    grassland::LogInfo("Shader compiled successfully");

    core_->CreateRayTracingProgram(raygen_shader_.get(), miss_shader_.get(), closest_hit_shader_.get(), &program_);
    program_->AddResourceBinding(grassland::graphics::RESOURCE_TYPE_ACCELERATION_STRUCTURE, 1);  // space0
    program_->AddResourceBinding(grassland::graphics::RESOURCE_TYPE_WRITABLE_IMAGE, 1);          // space1
    program_->AddResourceBinding(grassland::graphics::RESOURCE_TYPE_UNIFORM_BUFFER, 1);          // space2
    program_->AddResourceBinding(grassland::graphics::RESOURCE_TYPE_STORAGE_BUFFER, 1);          // space3 - materials
    program_->Finalize();
}

void Application::OnClose() {
    program_.reset();
    raygen_shader_.reset();
    miss_shader_.reset();
    closest_hit_shader_.reset();

    scene_.reset();

    color_image_.reset();
    camera_object_buffer_.reset();
}

void Application::OnUpdate() {
    if (window_->ShouldClose()) {
        window_->CloseWindow();
        alive_ = false;
    }
    if (alive_) {
        // +++ ADDED SECTION +++
        // Process keyboard input to move camera
        ProcessInput();

        // Update the camera buffer with new position/orientation
        CameraObject camera_object{};
        camera_object.screen_to_camera = glm::inverse(
            glm::perspective(glm::radians(60.0f), (float)window_->GetWidth() / (float)window_->GetHeight(), 0.1f, 10.0f));
        camera_object.camera_to_world =
            glm::inverse(glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_));
        camera_object_buffer_->UploadData(&camera_object, sizeof(CameraObject));


        // Optional: Animate entities
        // For now, entities are static. You can update their transforms and call:
        // scene_->UpdateInstances();
    }
}

void Application::OnRender() {
    std::unique_ptr<grassland::graphics::CommandContext> command_context;
    core_->CreateCommandContext(&command_context);
    command_context->CmdClearImage(color_image_.get(), { {0.6, 0.7, 0.8, 1.0} });
    command_context->CmdBindRayTracingProgram(program_.get());
    command_context->CmdBindResources(0, scene_->GetTLAS(), grassland::graphics::BIND_POINT_RAYTRACING);
    command_context->CmdBindResources(1, { color_image_.get() }, grassland::graphics::BIND_POINT_RAYTRACING);
    command_context->CmdBindResources(2, { camera_object_buffer_.get() }, grassland::graphics::BIND_POINT_RAYTRACING);
    command_context->CmdBindResources(3, { scene_->GetMaterialsBuffer() }, grassland::graphics::BIND_POINT_RAYTRACING);
    command_context->CmdDispatchRays(window_->GetWidth(), window_->GetHeight(), 1);
    command_context->CmdPresent(window_.get(), color_image_.get());
    core_->SubmitCommandContext(command_context.get());
}
