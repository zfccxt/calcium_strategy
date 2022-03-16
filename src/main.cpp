#include <algorithm>
#include <chrono>
#include <iostream>

#include <calcium.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
  auto context = cl::Context::CreateContext(cl::Backend::kVulkan);

  cl::WindowCreateInfo window_info;
  window_info.enable_vsync = false;
  auto window = context->CreateWindow(window_info);

  auto earth_shader = context->CreateShader("res/shaders/earth.vert.spv", "res/shaders/earth.frag.spv");
  auto earth_model = context->CreateMesh("res/models/earth/Earth.obj");
  auto earth_texture = context->CreateTexture("res/models/earth/Diffuse_2K.png");
  auto night_texture = context->CreateTexture("res/models/earth/Night_lights_2K.png");
  earth_shader->BindTexture("u_diffuse_texture", earth_texture);
  earth_shader->BindTexture("u_night_texture", night_texture);

  glm::mat4 projection = glm::mat4(1);
  auto calc_proj = [&](){ projection = glm::perspective(glm::radians(45.0f), window->GetAspectRatio(), 0.1f, 1000.0f); };
  window->SetResizeCallback(calc_proj);
  calc_proj();

  glm::vec3 position = glm::vec3(0, 0, -10.0f);
  glm::vec3 pos_momentum = glm::vec3(0, 0, 0);
  glm::vec3 rotation = glm::vec3(0, 0, 0);
  glm::vec3 rot_momentum = glm::vec3(0, 0, 0);
  window->SetMouseWheelCallback([&](float x, float y){ pos_momentum.z = y * 0.1f; });

  bool rmb_down = false;
  window->SetMouseButtonPressCallback(cl::MouseButton::kRightClick, [&](){ rmb_down = true; });
  window->SetMouseButtonReleaseCallback(cl::MouseButton::kRightClick, [&](){ rmb_down = false; });
  window->SetMouseMoveCallback([&](float dx, float dy){
    if (rmb_down) {
      rot_momentum.x += dx * 0.0003f;
      rot_momentum.y -= dy * 0.0003f;
    }
  });

  glm::mat4 model_matrix = glm::mat4(1);

  uint64_t num_frames = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  float elapsed_seconds = 0.0f;
  float total_seconds = 20.0f;

  while (window->IsOpen()) {
    window->PollEvents();

    position += pos_momentum;
    position.z = std::clamp(position.z, -33.5f, -3.5f);
    pos_momentum *= 0.9f;
    rotation += rot_momentum;
    rotation.y = std::clamp(rotation.y, -1.6f, 1.6f);
    rot_momentum *= 0.9f;

    glm::mat4 viewproj = projection * glm::translate(glm::mat4(1), position) 
      * glm::rotate(glm::mat4(1), rotation.y, glm::vec3(1, 0, 0))
      * glm::rotate(glm::mat4(1), rotation.x, glm::vec3(0, 1, 0));
    earth_shader->UploadUniform(0, glm::value_ptr(viewproj));

    auto current_time = std::chrono::high_resolution_clock::now();
    elapsed_seconds = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
    model_matrix = glm::rotate(glm::mat4(1), elapsed_seconds * 0.2f, glm::vec3(0, 1, 0));
    earth_shader->UploadUniform(1, glm::value_ptr(model_matrix));

    context->BeginFrame();
      earth_shader->Bind();
      earth_model->Draw();
    context->EndFrame();

    ++num_frames;
    if (elapsed_seconds > total_seconds) {
      window->Close();
    }
  }

  std::cout << "       Elapsed time: " << elapsed_seconds << " seconds" << "\n";
  std::cout << "Num frames rendered: " << num_frames << "\n";
  float framerate = (float)num_frames / elapsed_seconds;
  std::cout << "      Avg framerate: " << framerate << " fps" << "\n";
  std::cin.get();
}
