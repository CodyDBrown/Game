#include "libtcod.hpp"
#include "Actor.hpp"
#include "Map.hpp"
#include "Engine.hpp"

Engine::Engine() : gameStatus(STARTUP), fovRadius(10) {
    TCODConsole::initRoot(80,50,"libtcod C++ tutorial",false);
    player = new Actor(40,25,'@',"player",TCODColor::white);
    actors.push_back(player);
    map = new Map(80,45);
}

Engine::~Engine() {
    actors.clear();
    delete map;
}

void Engine::update() {
    TCOD_key_t key;
    if ( gameStatus == STARTUP ) map->computeFOV();
    gameStatus=IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL);
    int dx=0,dy=0;
    switch(key.vk) {
		case TCODK_UP:
            dy=-1; break;
		case TCODK_DOWN :
            dy=1; break;
		case TCODK_LEFT :
            dx=-1; break;
		case TCODK_RIGHT : dx=1; break;
        default:break;
    }
    if ( dx != 0 || dy != 0 ) {
    	gameStatus=NEW_TURN;
    	if ( player->moveOrAttack(player->x+dx,player->y+dy) ) {
	    	map->computeFOV();
	    }
	}
	if ( gameStatus == NEW_TURN ) {
	    for (std::vector<Actor*>::iterator it = actors.begin();
         it != actors.end();
	        it++) {
	        Actor *actor=*it;
	        if ( actor != player ) {
	            actor->update();
	        }
	    }
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	// draw the map
	map->render();
	// draw the actors
	for (std::vector<Actor*>::iterator it =actors.begin();
	    it != actors.end(); it++) 
    {
		Actor *actor=*it;
		if ( map->isInFOV(actor->x,actor->y) ) 
        {
	        actor->render();
	    }
	}
}