#pragma once

#include "Core/SmartPtr.h"
#include "Core/Log.h"
#include "Core/Collections/Array_type.h"
#include "Core/Time/TimeServer.h"
#include "Core/StringsTable.h"
#include "Core/IO/FileServer.h"
#include "Core/Pool/Handle_type.h"
#include "Render/Resources/ResourceServer.h"
#include "Render/RenderQueue.h"
#include "Managers/BaseManager.h"
#include "Game/SerializationServer.h"

struct GLFWwindow;

namespace Framework {

struct Application {
protected:
    static Application *instance;

    const char *name;
    bool active;
	int screenWidth;
	int screenHeight;

    SmartPtr<Log> log;
    SmartPtr<TimeServer> timeServer;
    SmartPtr<StringsTable> stringsTable;
    SmartPtr<FileServer> fileServer;
    SmartPtr<ResourceServer> resServer;
    SmartPtr<SerializationServer> serializationServer;
    SmartPtr<RenderQueue> renderQueue;

    Array<SmartPtr<BaseManager>> managers;

	GLFWwindow *gameWindow;
	GLFWwindow *renderThreadWindow;

    void Tick();

//#if defined(EDITOR)
    String serializeFilepath;
    BitStream serializeStream;
    bool inBeginSerialize;
//#endif
public:
    Application(const char *_name);
    ~Application();

    bool Initialize(int width, int height);
    void Run();

    const SmartPtr<BaseManager>& GetManager(const ClassInfo *classInfo);

    void RequestPause();
    void RequestResume();
    void RequestQuit();

	int GetScreenWidth() const;
	int GetScreenHeight() const;

	GLFWwindow* GetGameWindow() const;
	GLFWwindow* GetRenderThreadWindow() const;

    void DeserializeEntities(const char *filename);

//#if defined(EDITOR)
    void BeginSerialize(const char *filename);
    void SerializeEntity(const Handle<Entity> &entity);
    void EndSerialize();
//#endif

    static Application* Instance();
};

inline Application*
Application::Instance()
{
    return Application::instance;
}

inline int
Application::GetScreenWidth() const
{
	return screenWidth;
}

inline int
Application::GetScreenHeight() const
{
	return screenHeight;
}

inline GLFWwindow*
Application::GetGameWindow() const
{
	return gameWindow;
}

inline GLFWwindow*
Application::GetRenderThreadWindow() const
{
	return renderThreadWindow;
}

} // namespace Framework
