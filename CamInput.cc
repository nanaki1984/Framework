#include "CamInput.h"
#include "Core/Time/TimeServer.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Math/Math.h"
#include "Core/Log.h"
#include "GLFW/glfw3.h"
#include "Managers/TransformsManager.h"
#include "imgui.h"

DefineClassInfo(CamInput, Framework::Component);
DefineComponent(CamInput, 0);

using namespace Framework;
using namespace Framework::Math;

CamInput::CamInput()
: Component()
{ }

CamInput::CamInput(const CamInput &other)
: Component(other)
{ }

CamInput::CamInput(CamInput &&other)
: Component(std::forward<Component>(other))
{ }

CamInput::~CamInput()
{ }

void
CamInput::Update()
{
    auto transform = entity->GetTransform();

    Vector3 p = transform->GetWorldPosition();
    Quaternion q = transform->GetWorldRotation();

    Vector3 forward = -q.Rotate(Vector3::ZAxis),
            up      =  q.Rotate(Vector3::YAxis),
            right   =  q.Rotate(Vector3::XAxis);

    float dt = TimeServer::Instance()->GetDeltaTime();

	GLFWwindow *window = Application::Instance()->GetRenderingContext();

	Vector3 delta = Vector3::Zero;
    if (GLFW_PRESS == glfwGetKey(window, 'W'))
        delta += forward * dt;
    if (GLFW_PRESS == glfwGetKey(window, 'S'))
        delta -= forward * dt;
    if (GLFW_PRESS == glfwGetKey(window, 'A'))
        delta -= right * dt;
    if (GLFW_PRESS == glfwGetKey(window, 'D'))
        delta += right * dt;
    if (GLFW_PRESS == glfwGetKey(window, 'E'))
        delta -= up * dt;
    if (GLFW_PRESS == glfwGetKey(window, 'Q'))
        delta += up * dt;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
        forward -= right * dt;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
        forward += right * dt;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
        forward += up * dt;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
        forward -= up * dt;
    if (GLFW_PRESS == glfwGetKey(window, 'R'))
    {
        auto app = Application::Instance();
        auto trMng = GetManager<TransformsManager>();
        app->BeginSerialize("test.scene");
        for (auto it = trMng->Begin(), end = trMng->End(); it != end; ++it)
            app->SerializeEntity((*it)->GetEntity());
        app->EndSerialize();
    }

	QuaternionLookAt(forward, up, q);

    transform->SetWorldPosition(p + delta * 2.0f);
    transform->SetWorldRotation(q);

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    /*{
        static ImVec4 clear_color = ImColor(114, 144, 154);
        static float f = 0.0f;
        static bool show_window = true;

        ImGui::Begin("Test window", &show_window);
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        //if (ImGui::Button("Test Window")) show_test_window ^= 1;
        //if (ImGui::Button("Another Window")) show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        static bool show_metrics = true;
        ImGui::ShowMetricsWindow(&show_metrics);
    }*/
}
