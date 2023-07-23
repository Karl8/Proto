#include <iostream>
#include <GLFW/glfw3.h>
// #include <imgui.h>

#include "editor.h"
#include "common.h"

Editor::Editor()
{
}

Editor::~Editor()
{
}

void Editor::start()
{
    LOG(INFO, "Editor Start.");

    // if (!glfwInit())
    // {
    //     LOG(ERROR, "glfwInit() failed.");
    //     return;
    // }

    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // GLFWwindow* window = glfwCreateWindow(1280, 960, "proto_editor", 0, 0);

    // ImGui::CreateContext();

    // bool isWindowActive;
    // ImGui::NewFrame();
    // ImGui::Begin("proto_editor_imgui", &isWindowActive, 0);
     
}

void Editor::shutdown()
{
    LOG(INFO, "Editor Shutdown.");
}
