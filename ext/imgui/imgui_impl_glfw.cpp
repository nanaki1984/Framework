// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <imgui.h>
#include "imgui_impl_glfw.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include "Core/Memory/MallocAllocator.h"
#include "Render/Resources/Mesh.h"
#include "Render/Resources/Material.h"
#include "Render/Resources/Shader.h"
#include "Render/Resources/ResourceServer.h"
#include "Render/Key.h"
#include "Render/RenderQueue.h"

using namespace Framework;

// Data
static GLFWwindow*  g_Window = NULL;
static double       g_Time = 0.0f;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;

struct UIMesh
{
private:
    int                      index;
	RHI::VertexBufferDesc    vbDesc;
	RHI::IndexBufferDesc     ibDesc;
	WeakPtr<Mesh>            mesh;
    Array<WeakPtr<Material>> materials;
public:
    UIMesh()
    : materials(Memory::GetAllocator<MallocAllocator>())
	{ }

    void Init(int _index, int nVertices, int nIndices, int nDrawCalls)
    {
        index = _index;

        String str;

        vbDesc.flags     = RHI::HardwareBuffer::Dynamic;
		vbDesc.nVertices = nVertices;

		vbDesc.vertexDecl.AddVertexElement(RHI::VertexDecl::XYZ,      RHI::VertexDecl::Float2);
		vbDesc.vertexDecl.AddVertexElement(RHI::VertexDecl::TexCoord, RHI::VertexDecl::Float2);
		vbDesc.vertexDecl.AddVertexElement(RHI::VertexDecl::Color0,   RHI::VertexDecl::Float1);

		ibDesc.flags     = RHI::HardwareBuffer::Dynamic;
		ibDesc.nIndices  = nIndices;
		ibDesc.indexSize = RHI::IndexWord;

        str.Set("uiMesh");
        str.Append('0' + index);

        mesh = ResourceServer::Instance()->NewResource<Mesh>(str, Resource::ReadOnly);
        mesh->Load(std::move(vbDesc), ibDesc, nDrawCalls);

        for (int i = 0; i < nDrawCalls; ++i)
        {
            str.Set("uiMaterial");
            str.Append('0' + index);
            str.Append('_');
            str.Append('0' + i);

            WeakPtr<Material> mat = ResourceServer::Instance()->NewResource<Material>(str, Resource::ReadOnly);
            mat->SetShader("home:shaders/ui.shader");
            materials.PushBack(mat);
        }
    }

    void Reserve(int nVertices, int nIndices, int nDrawCalls)
    {
        if (nVertices > vbDesc.nVertices || nIndices > ibDesc.nIndices)
        {
            vbDesc.nVertices = nVertices;
            ibDesc.nIndices  = nIndices;

            mesh->Load(std::move(vbDesc), ibDesc, nDrawCalls);
        }
        else if (nDrawCalls > mesh->GetSubMeshCount())
            mesh->SetSubMeshCount(nDrawCalls);

        String str;
        for (int i = materials.Count(); i < nDrawCalls; ++i)
        {
            str.Set("uiMaterial");
            str.Append('0' + index);
            str.Append('_');
            str.Append('0' + i);

            WeakPtr<Material> mat = ResourceServer::Instance()->NewResource<Material>(str, Resource::ReadOnly);
            mat->SetShader("shaders:UI.shader");
            materials.PushBack(mat);
        }
    }

    void FillAndRender(
        int fb_height, const float *projMtx, RHI::Key &key,
        const ImVector<ImDrawVert> &vertices,
        const ImVector<ImDrawIdx>  &indices,
        const ImVector<ImDrawCmd>  &drawCalls)
    {
        mesh->GetVertexBufferData().Reset();
        mesh->GetVertexBufferData().WriteBytes(vertices.Data, sizeof(ImDrawVert) * vertices.Size);

        mesh->GetIndexBufferData().Reset();
        mesh->GetIndexBufferData().WriteBytes(indices.Data, sizeof(ImDrawIdx) * indices.Size);

        mesh->Apply();

        uint32_t startIndexOffset = 0;
        for (int i = 0, c = drawCalls.Size; i < c; ++i)
        {
            DrawPrimitives &dc = mesh->GetSubMeshPrimitives(i);

            dc.primType    = DrawPrimitives::TriangleList;
            dc.startIndex  = startIndexOffset;
            dc.nPrimitives = drawCalls[i].ElemCount / 3;

            startIndexOffset += drawCalls[i].ElemCount;

            const ImVec4 &scissorRect = drawCalls[i].ClipRect;

            RenderModeState &renderMode = materials[i]->GetRenderMode();
            renderMode.scissorTestEnabled = true;
            renderMode.scissorTestRect[0] = scissorRect.x;
            renderMode.scissorTestRect[1] = (fb_height - scissorRect.w);
            renderMode.scissorTestRect[2] = (scissorRect.z - scissorRect.x);
            renderMode.scissorTestRect[3] = (scissorRect.w - scissorRect.y);

            materials[i]->SetTexture("Texture", ResourceServer::Instance()->GetResourceById((ResourceId)drawCalls[i].TextureId).Cast<Texture>());

            MaterialParamsBlock *params;
            uint16_t paramsBlockId = RenderQueue::Instance()->GetMaterialParamsBlock(&params);
            params->AddMatrix("ProjMtx", Math::Matrix(projMtx));

            key.DrawCall(0, materials[i], 0, mesh, (uint8_t)i, paramsBlockId, .0f);
            RenderQueue::Instance()->SendCommand(key);
        }
    }
};

static Array<UIMesh> *uiMeshes;
static uint8_t __uiMeshesBuffer[sizeof(Array<UIMesh>)];

void ImGui_ImplGlfwGL3_RenderDrawLists(ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    const float projMtx[16] =
    {
         2.0f / io.DisplaySize.x, 0.0f,                      0.0f, 0.0f,
         0.0f,                    2.0f / -io.DisplaySize.y,  0.0f, 0.0f,
         0.0f,                    0.0f,                     -1.0f, 0.0f,
        -1.0f,                    1.0f,                      0.0f, 1.0f,
    };

    RHI::Key key;
    key.cameraDepth = 0xff;
    key.cameraLayer = 0xf;

    key.Sequence(0).ResetRenderTarget();
    RenderQueue::Instance()->SendCommand(key);

    RHI::Viewport viewport;
    viewport.x      = 0;
    viewport.y      = 0;
    viewport.width  = fb_width;
    viewport.height = fb_height;
    viewport.minZ   = -1.0f;
    viewport.maxZ   =  1.0f;

    key.Sequence(0).SetViewport(viewport);
    RenderQueue::Instance()->SendCommand(key);

    key.Sequence(0).Clear(RHI::BaseRenderer::ClearDepth, 0, 1.0f, 0);
    RenderQueue::Instance()->SendCommand(key);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        if (n >= uiMeshes->Count())
        {
            uiMeshes->PushBack(UIMesh());
            (*uiMeshes)[n].Init(n, cmd_list->VtxBuffer.Size, cmd_list->IdxBuffer.Size, cmd_list->CmdBuffer.Size);
        }
        else
            (*uiMeshes)[n].Reserve(cmd_list->VtxBuffer.Size, cmd_list->IdxBuffer.Size, cmd_list->CmdBuffer.Size);

        (*uiMeshes)[n].FillAndRender(fb_height, projMtx, key, cmd_list->VtxBuffer, cmd_list->IdxBuffer, cmd_list->CmdBuffer);
    }
}

static const char* ImGui_ImplGlfwGL3_GetClipboardText(void* user_data)
{
	return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_ImplGlfwGL3_SetClipboardText(void* user_data, const char* text)
{
	glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void ImGui_ImplGlfwGL3_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
{
	if (action == GLFW_PRESS && button >= 0 && button < 3)
		g_MousePressed[button] = true;
}

void ImGui_ImplGlfwGL3_ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset)
{
	g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
}

void ImGui_ImplGlfwGL3_KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ImGui_ImplGlfwGL3_CharCallback(GLFWwindow*, unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter((unsigned short)c);
}

bool ImGui_ImplGlfwGL3_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    WeakPtr<Texture> fontTex = ResourceServer::Instance()->NewResource<Texture>("uiFont", Resource::ReadOnly);
    RHI::TextureBufferDesc texDesc;
    Memory::Zero(&texDesc);
    texDesc.width  = width;
    texDesc.height = height;
    texDesc.format = ImageFormat::A8R8G8B8;
    texDesc.type   = RHI::BaseTextureBuffer::Texture2D;
    fontTex->Load(texDesc);
    Image &img = fontTex->GetImage(0);
    Memory::Copy(static_cast<unsigned char*>(img.GetPixels()), pixels, img.GetSize());
    fontTex->Apply(0);

    io.Fonts->TexID = (void*)fontTex->GetId();

	return true;
}

bool ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks)
{
	g_Window = window;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.RenderDrawListsFn = ImGui_ImplGlfwGL3_RenderDrawLists;       // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
	io.ClipboardUserData = g_Window;
#ifdef _WIN32
	io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

	if (install_callbacks)
	{
		glfwSetMouseButtonCallback(window, ImGui_ImplGlfwGL3_MouseButtonCallback);
		glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);
		glfwSetKeyCallback(window, ImGui_ImplGlfwGL3_KeyCallback);
		glfwSetCharCallback(window, ImGui_ImplGlfwGL3_CharCallback);
	}

    uiMeshes = new(__uiMeshesBuffer) Array<UIMesh>(Memory::GetAllocator<MallocAllocator>());

    ImGui_ImplGlfwGL3_CreateFontsTexture();

	return true;
}

void ImGui_ImplGlfwGL3_Shutdown()
{
	ImGui::Shutdown();
}

void ImGui_ImplGlfwGL3_NewFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(g_Window, &w, &h);
	glfwGetFramebufferSize(g_Window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	double current_time = glfwGetTime();
	io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
	g_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED))
	{
		double mouse_x, mouse_y;
		glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
		io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
	}
	else
	{
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}

	for (int i = 0; i < 3; i++)
	{
		io.MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(g_Window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		g_MousePressed[i] = false;
	}

	io.MouseWheel = g_MouseWheel;
	g_MouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	// Start the frame
	ImGui::NewFrame();
}
