#ifndef EMSCRIPTEN
#   include "GL/glew.h"
#endif
#include "GL/glfw.h"

#include "Core/Application.h"
#include "Core/Time/TimeServer.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/LinearAllocator.h"
#include "Core/Collections/Array.h"

#include "Managers/ComponentsManager.h"
#include "Managers/GetManager.h"

#ifdef EMSCRIPTEN
#   include <emscripten.h>
#endif

#include <imgui.h>
#include "imgui_impl_glfw.h"

namespace Framework {

Application *Application::instance = nullptr;

Application::Application(const char *_name)
: name(_name),
  active(false),
  screenWidth(0),
  screenHeight(0),
  managers(Memory::GetAllocator<MallocAllocator>())
//#if defined(EDITOR)
, serializeStream(Memory::GetAllocator<MallocAllocator>()),
  inBeginSerialize(false)
//#endif
{
    assert(nullptr == instance);
    instance = this;

    log = SmartPtr<Log>::MakeNew<LinearAllocator>();
    stringsTable = SmartPtr<StringsTable>::MakeNew<LinearAllocator>(&Memory::GetAllocator<MallocAllocator>());
    fileServer = SmartPtr<FileServer>::MakeNew<LinearAllocator>();
    resServer = SmartPtr<ResourceServer>::MakeNew<LinearAllocator>();
    serializationServer = SmartPtr<SerializationServer>::MakeNew<LinearAllocator>();

    timeServer = SmartPtr<TimeServer>::MakeNew<LinearAllocator>();
    timeServer->Pause();
}

Application::~Application()
{
	assert(this == instance);
    instance = nullptr;
}

bool
Application::Initialize(int width, int height)
{
    ClassInfoUtils::Instance()->Initialize();

    screenWidth = width;
    screenHeight = height;

    if (!glfwInit())
        return false;

    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, 1);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);

    if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 16, 0, GLFW_WINDOW))
    {
        glfwTerminate();
        return false;
    }
#ifndef EMSCRIPTEN
    glewInit();
#endif
    glfwSetWindowTitle(name);
    glfwEnable(GLFW_KEY_REPEAT);
    glfwSwapInterval(0);// 1);

    ImGui_ImplGlfwGL2_Init(true);

    renderQueue = SmartPtr<RenderQueue>::MakeNew<LinearAllocator>();

	return true;
}

void
Application::Run()
{
	active = true;

    timeServer->Resume();

#ifndef EMSCRIPTEN
    while (active)
    {
        glfwPollEvents();

        if (!glfwGetWindowParam(GLFW_OPENED))
        {
            this->RequestQuit();
            break;
        }

        ImGui_ImplGlfwGL2_NewFrame();

        this->Tick();

        // render
	    renderQueue->RenderFrame();

        glfwSwapBuffers();
    }
#else
    emscripten_set_main_loop([] { Application::Instance()->Tick(); }, 0, 1);
#endif
}

void
Application::Tick()
{
#if !defined EMSCRIPTEN
    float t0 = TimeServer::Instance()->GetMilliseconds();
#endif
    timeServer->Tick();

    // physics

    auto it = managers.Begin(), end = managers.End();
    for (; it != end; ++it)
        (*it)->OnUpdate();

    // update scenegraph, animations, ecc...

    for (it = managers.Begin(); it != end; ++it)
        (*it)->OnLateUpdate();

	renderQueue->BeginFrameCommands();
	for (it = managers.Begin(); it != end; ++it)
		(*it)->OnRender();
	renderQueue->EndFrameCommands();

    RefCounted::GC.Collect();

#if !defined EMSCRIPTEN
    float t1 = TimeServer::Instance()->GetMilliseconds();
    Log::Instance()->Write(Log::Info, "frame time: %f", (t1 - t0));
#endif
}

const SmartPtr<BaseManager>&
Application::GetManager(const ClassInfo *classInfo)
{
    for (auto it = managers.Begin(), end = managers.End(); it < end; ++it)
    {
        if ((*it)->GetRTTI() == classInfo)
            return *const_cast<const SmartPtr<BaseManager>*>(it);
    }

    auto newMng = SmartPtr<BaseManager>::CastFrom(classInfo->Create(&Memory::GetAllocator<LinearAllocator>()));

    int index = BinarySearch<SmartPtr<BaseManager>, SmartPtr<BaseManager>>(managers, 0, managers.Count(), newMng,
    [] (const SmartPtr<BaseManager> &a,
        const SmartPtr<BaseManager> &b)
    {
        int execOrderA = a->GetExecutionOrder(),
            execOrderB = b->GetExecutionOrder();
        if (execOrderA > execOrderB)
            return 1;
        else if (execOrderA < execOrderB)
            return -1;
        else
            return 0;
    });

    if (index < 0)
        index = ~index;

    managers.Insert(index, newMng);

    if (timeServer->IsPaused())
        newMng->OnPause();

    return managers[index];
}

void
Application::RequestPause()
{
    if (!timeServer->IsPaused()) {
        timeServer->Pause();

        auto it = managers.Begin(), end = managers.End();
        for (; it != end; ++it)
            (*it)->OnPause();
    }
}

void
Application::RequestResume()
{
    if (timeServer->IsPaused()) {
    	timeServer->Resume();

        auto it = managers.Begin(), end = managers.End();
        for (; it != end; ++it)
            (*it)->OnResume();
    }
}

void
Application::RequestQuit()
{
    auto it = managers.Begin(), end = managers.End();
    for (; it != end; ++it)
        (*it)->OnQuit();

    managers = Array<SmartPtr<BaseManager>>(Memory::GetAllocator<MallocAllocator>());

    serializationServer.Reset();
    resServer.Reset();

	RefCounted::GC.Collect();

    timeServer.Reset();
    fileServer.Reset();
    renderQueue.Reset();
    stringsTable.Reset();
    log.Reset();

	RefCounted::GC.Collect();

    ClassInfoUtils::Destroy();

    ImGui_ImplGlfwGL2_Shutdown();
    glfwTerminate();

    active = false;
}

void
Application::DeserializeEntities(const char *filename)
{
    BitStream stream(Memory::GetAllocator<MallocAllocator>());
    if (0 == fileServer->ReadOnly(filename, stream))
        serializationServer->DeserializeEntities(stream);
    else
        log->Write(Log::Warning, "Cound't open file \"%s\"", filename);
}

//#if defined(EDITOR)
void
Application::BeginSerialize(const char *filename)
{
    assert(!inBeginSerialize);
    inBeginSerialize = true;
    serializeFilepath = filename;
    serializeStream.Reset();
}

void
Application::SerializeEntity(const Handle<Entity> &entity)
{
    assert(inBeginSerialize);
    serializationServer->MarkEntityForSerialization(entity);
}

void
Application::EndSerialize()
{
    assert(inBeginSerialize);
    inBeginSerialize = false;

    serializationServer->SerializeMarkedEntities(serializeStream);
    Framework::GetManager<ComponentsManager>()->SerializeMarkedComponents(serializeStream);
    serializationServer->CompleteSerialization();

    fileServer->WriteOnly(serializeFilepath.AsCString(), serializeStream);
}
//#endif

} // namespace Framework
