#include "Project.h"
#include <SDL.h>
#include "BadSandwich.h"
#include "GoodSandwich.h"
#include "NormalSandwich.h"
#include "../../MainMenu.h"
#include "../../../OpenGL/Environment.hpp"
#include "../../../Engine/Window.hpp"
#include "../../../Engine/Input.hpp"

#include "../../../CS200/RenderingAPI.hpp"
#include "../../../CS200/NDC.hpp"
#include "../../../CS200/IRenderer2D.hpp"


Project::Project() : 
	player_ptr(nullptr)
	//camera(nullptr)
{}

void Project::Load() 
{
	
	//srand(static_cast<unsigned int>(time(NULL)));
	//Engine::GetWindow().SetSize({ default_width, default_height });
	/*camera = new CS230::Camera({ Math::vec2{ 0,0 }, static_cast<Math::vec2>(Engine::GetWindow().GetSize()) });
	camera->SetLimit({ {0,0},Engine::GetWindow().GetSize() });
	AddGSComponent(camera);*/
	//GetGSComponent<CS230::Camera>()->SetPosition({ 0.0, 0.0 });
	if(!OpenGL::IsWebGL){
		Engine::GetWindow().ForceResize(600, 800);
		Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
	}
    
	
#ifdef _DEBUG
	AddGSComponent(new CS230::ShowCollision());
	AddGSComponent(new Grid());
#endif
	AddGSComponent(new Random());
	AddGSComponent(new CS230::GameObjectManager);
	AddGSComponent(new CS230::ParticleManager<Particles::Tears>());
	AddGSComponent(new CS230::ParticleManager<Particles::Shining>());
	
	player_ptr = new Player();
	GetGSComponent<CS230::GameObjectManager>()->Add(player_ptr);
    bus_ptr = new Bus(player_ptr);
    GetGSComponent<CS230::GameObjectManager>()->Add(bus_ptr);
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line1,player_ptr,0));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line2,player_ptr,0));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line3,player_ptr,0));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line4,player_ptr,0));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line5,player_ptr,0));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line1, player_ptr, 1));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line2, player_ptr, 1));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line3, player_ptr, 1));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line4, player_ptr, 1));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Passenger(BusLine::line5, player_ptr, 1));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Obstacle(player_ptr, { (BusStartPosition + SeatWidthHeight + PlayerWidthHeight * (static_cast<double>(rand() )/ RAND_MAX) * 2) , SeatWidthHeight }));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Obstacle(player_ptr, { (BusStartPosition + SeatWidthHeight + PlayerWidthHeight * (static_cast<double>(rand()) / RAND_MAX) * 2) , SeatWidthHeight * 3 }));
	GetGSComponent<CS230::GameObjectManager>()->Add(new Obstacle(player_ptr, { (BusStartPosition + SeatWidthHeight + PlayerWidthHeight * (static_cast<double>(rand()) / RAND_MAX) * 2) , SeatWidthHeight * 5 }));

	CS200::RenderingAPI::SetClearColor(0x000000FF);
}

void Project::Update([[maybe_unused]] double dt) 
{
	Engine::GetLogger().LogEvent("Final Update");
	UpdateGSComponents(dt);
	/*camera->SetLimit({ {0,0},Engine::GetWindow().GetSize() });
	camera->Update(static_cast<Math::vec2>(Engine::GetWindow().GetSize()));*/
	GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);

	Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->SortForDraw();

	if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
	}
    if (bus_ptr->IsEnd())
    {
        Bus::SandwichLevel level = bus_ptr->sandwich_level;
        switch (level)
        {
            case Bus::SandwichLevel::GoodSandwich:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<GoodSandwich>();
                break;
            case Bus::SandwichLevel::NormalSandwich:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<NormalSandwich>();
                break;
            case Bus::SandwichLevel::BadSandwich:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<BadSandwich>();
                break;
            default: break;
        }
	}
}

void Project::Unload() 
{
	GetGSComponent<CS230::GameObjectManager>()->Unload();
	ClearGSComponents();
	//player_ptr = nullptr;
	//camera = nullptr;
}

void Project::Draw() 
{
    CS200::RenderingAPI::Clear();
    auto& renderer_2d = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
	//Math::TransformationMatrix camera_matrix = camera->GetMatrix();
	GetGSComponent<CS230::GameObjectManager>()->DrawAll(Math::TransformationMatrix());
	#ifdef _DEBUG
	GetGSComponent<Grid>()->Draw(*(GetGSComponent<CS230::Camera>()));
	#endif
	
    renderer_2d.EndScene();

}

void Project::DrawImGui()
{

}

gsl::czstring Project::GetName() const
{
    return "Final Project";
}