#pragma once
#include <fstream>
#include <iterator>
#include <SFML/Graphics.hpp>
#include "../Graphics_Engine/Space/QuadTree.h"
#include "../Graphics_Engine/Graphics/Graphics.h"
#include "../Graphics_Engine/Factory.h"
#include "../Graphics_Engine/Cooldown.h"
#include "../Graphics_Engine/Graphics/Textures.h"
#include "../Graphics_Engine/Sound/Sound.h"
#include "../Graphics_Engine/Physics/Physics.h"

#include <boost/lexical_cast.hpp>
#include "Queue_Manager.h"
#include "Interface_Runnable.h"
#include "Component.h"
#include "Agent.h"
#include "Interface_Network_Observer.h"

class Graphics_Engine : public Interface_Runnable , public Component<message::Parsed_Message> , public Interface_Network_Observer
{
private:
	std::shared_ptr<sf::RenderWindow> window_;
	sf::Event event_;
	std::unique_ptr<QuadTree> Quadtree_;
	std::unique_ptr<std::vector<std::shared_ptr<Object>>> object_vector_;
	Queue_Manager<message::Parsed_Message> incoming_messages_;

	void event_loop();
	void handle_messages();
	void loading_screen();
	void activate_objects();
	void objects_to_quadtree();
	void draw_objects();
	void clean_dead_objects();
	void game_loop();

public:

	Graphics_Engine();
	~Graphics_Engine();
	
	void operator=(const Graphics_Engine&& gm) noexcept;

	void provide_message(message::Parsed_Message &pmsg);
	std::string service_name();

	void agent_added(std::shared_ptr<Agent> added_agent);
	void agent_removed(std::shared_ptr<Agent> added_agent);


	void run();
	void pause();
	void restart();
	void close();

};