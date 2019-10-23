#include "libtcod.hpp"
#include "Actor.hpp"
#include "Map.hpp"
#include "Engine.hpp"

// Get things going
Engine::Engine()
{
    // Make the window. It has size 80 x 50 squares. It's name is "libtcod..."
    TCODConsole::initRoot(80, 50, "libtcod C++ tutorial", false);
    player = new Actor( 40, 25, '@', TCODColor::white);
    actors.push(player);
    //actors.push(new Actor( 60, 13, '@', TCODColor::yellow));
    map = new Map(80, 45);
}

// Clear all the actors and the map itself
Engine::~Engine()
{
    actors.clearAndDelete();
    delete map;
}

// Checks for a key press and then does a thing based on that key press
void Engine::update()
{
    TCOD_key_t key;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
    switch (key.vk)
    {
    case TCODK_UP:
        // if the space up isn't a wall then move up
        if( ! map->isWall(player->x, player->y-1))
        {
            player -> y--;
        }
        break;
    case TCODK_DOWN:
        // if the space down isn't a wall then move up
        if( ! map->isWall(player->x, player ->y+1))
        {
            player -> y++;
        }
        break;
    case TCODK_LEFT:
        // if the space left isn't a wall then move up
        if( ! map->isWall(player->x-1, player ->y))
        {
            player -> x--;
        }
        break;
    case TCODK_RIGHT:
        // if the space left isn't a wall then move up
        if( ! map->isWall(player->x+1, player ->y))
        {
            player -> x++;
        }
        break;
    default:
        break;
    }
}

void Engine::render()
{
    TCODConsole::root->clear();
    // Redraw the map
    map->render();

    //draw all the actors
    for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++)
    {
        (*iterator)->render();
    }
}