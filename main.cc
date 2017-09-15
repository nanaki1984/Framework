#include "Core/Memory/Memory.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/LinearAllocator.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Core/Memory/ScratchAllocator.h"
#include "Core/Application.h"

#include "Managers/GetManager.h"
#include "Managers/EntitiesManager.h"
#include "Managers/TransformsManager.h"
#include "Game/Entity.h"
#include "Game/MsgDispatcher.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "TestRotate.h"
#include "CamInput.h"

using namespace Framework;

int main(int argc, char **argv) {
	Memory::InitializeMemory();

	Memory::InitAllocator<MallocAllocator>();
	Memory::InitAllocator<LinearAllocator>(&Memory::GetAllocator<MallocAllocator>(), 1 * 1024 * 1024, 16);
	Memory::InitAllocator<BlocksAllocator>(&Memory::GetAllocator<MallocAllocator>(), 8192);
    Memory::InitAllocator<ScratchAllocator>(&Memory::GetAllocator<MallocAllocator>(), 512 * 1024);

	{
		Application app("Test");
        if (app.Initialize(640, 480))
        {
#if 1
            auto entMng = GetManager<EntitiesManager>();

            auto cam = entMng->NewEntity("Main Camera");
            cam->GetTransform()->SetWorldPosition(Math::Vector3(0.0f, 2.0f, 10.0f));
            cam->AddComponent<Camera>();
            cam->AddComponent<CamInput>();

            auto car = entMng->NewEntity("Car");
            car->GetTransform()->SetLocalPosition(Math::Vector3(0.0f, 0.0f, 2.0f));
            auto meshRndr = car->AddComponent<MeshRenderer>();
            meshRndr->SetMesh("home:test.3d");
            //car->AddComponent<TestRotate>();
            car->AddComponent("TestRotate");

            auto mat = ResourceServer::Instance()->NewResource<Material>("test_mat", Resource::Writable);
            mat->SetShader("home:test.shader");
            mat->SetTexture("Diffuse", "home:test.dds", Resource::ReadOnly);

            meshRndr->SetMaterial(mat);

            auto car2 = entMng->CloneEntity(car, "Car2", car);
            car2->GetTransform()->SetWorldPosition(Math::Vector3(4.0f, 0.0f, 0.0f));
            auto car3 = entMng->CloneEntity(car, "Car3", car);
            car3->GetTransform()->SetWorldPosition(Math::Vector3(-4.0f, 4.0f, 0.0f));
            //auto otherCarTr = car3->GetTransform()->GetChildren().Find("Car2");
            //Math::Vector3 wp = otherCarTr->GetWorldPosition();
            //Core::Log::Instance()->Write(Core::Log::Info, "%f, %f, %f", wp.x, wp.y, wp.z);
            //otherCarTr->SetWorldPosition(Math::Vector3(-8.0f, 4.0f, 0.0f));
            auto car4 = entMng->CloneEntity(car, "Car4", car, false);
            car4->GetTransform()->SetWorldPosition(Math::Vector3(0.0f, 0.0f, 8.0f));

            for (int i = 0; i < 400; ++i)
            {
                auto c = entMng->CloneEntity(car);

                int x = (i % 20), z = i / 20;

                c->GetTransform()->SetWorldPosition(Math::Vector3((x - 10.0f) * 3.0f, (rand() / (float)RAND_MAX) * 5.0f - 2.5f, (z - 10.0f) * 3.0f));
            }

            auto shd = ResourceServer::Instance()->NewResourceFromFile<ComputeShader>("home:test_compute.cshader", Resource::ReadOnly);
#else
            Camera::EnsureLinking();

            app.DeserializeEntities("home:test.scene");

            auto mat = ResourceServer::Instance()->NewResource<Material>("test_mat", Resource::Writable);
            mat->SetShader("home:test.shader");
            mat->SetTexture("Diffuse", "home:test.dds", Resource::ReadOnly);

            auto trMng = GetManager<TransformsManager>();
            for (auto it = trMng->Begin(), end = trMng->End(); it != end; ++it) {
                auto entity = (*it)->GetEntity();
                auto meshRndr = entity->GetComponent<MeshRenderer>().Get();
                if (meshRndr != nullptr)
                    meshRndr->SetMaterial(mat);
            }
#endif
            app.Run();
        }
	}

	Memory::ShutdownMemory();
}
