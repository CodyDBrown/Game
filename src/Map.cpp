#include "libtcod.hpp"
#include "Map.hpp"
#include "Actor.hpp"
#include "Engine.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

static const int MAX_ROOM_MONSTERS = 3;

class BspListener: public ITCODBspCallback
{
    public: 
        Map &map_;
        int roomNum_;
        int lastx_;
        int lasty_;

    public:
        BspListener(Map &map) : map_(map), roomNum_(0) {}

        bool visitNode(TCODBsp *node, void *userData)
        {
            if (node -> isLeaf() )
            {
                int x, y, w, h;
                TCODRandom *rng = TCODRandom::getInstance();
                w = rng -> getInt(ROOM_MIN_SIZE, node -> w-2);
                h=rng->getInt(ROOM_MIN_SIZE, node->h-2);
                x=rng->getInt(node->x+1, node->x+node->w-w-1);
                y=rng->getInt(node->y+1, node->y+node->h-h-1);
                map_.createRoom(roomNum_ == 0, x, y, x+w-1, y+h-1);

                if ( roomNum_ != 0)
                {
                    map_.dig(lastx_, lasty_, x + w/2, lasty_);
                    map_.dig(x+w/2, lasty_, x+w/2, y+h/2);
                }
                lastx_ = x + w/2;
                lasty_ = y + h/2;
                roomNum_++;
            }
            return true;
        }
};

// The constructor allocates the Tile array and creates two pillars
Map::Map(int width, int height) : width(width), height(height)
{
    tiles = new Tile[width*height];
    map = new TCODMap(width, height);
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MIN_SIZE, 1.5f, 1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder( &listener, NULL);
}

// Destructor releases everything that was allocated in the constructor

Map::~Map()
{
    delete [] tiles;
    delete map;
}

bool Map::isWall(int x, int y) const
{
    return !map -> isWalkable(x,y);
}

bool Map::canWalk(int x, int y) const
{
    if (isWall(x,y))
    {
        // Running into a wall.
        return false;
    }
    for (std::vector<Actor*>::iterator it = engine.actors.begin();
         it != engine.actors.end();
         it++ )
    {
        Actor *actor = *it;
        if (actor -> x == x && actor -> y == y)
        {
            // Running into an NPC
            return false;
        }
    }
    return true;

}

bool Map::isExplored(int x, int y) const
{
    return tiles[x+y*width].explored;
}

bool Map::isInFOV(int x, int y) const
{
    if ( map -> isInFov(x, y) )
    {
        tiles[x+y*width].explored = true;
        return true;
    }
    return false;
}

void Map::computeFOV()
{
    map -> computeFov(  engine.player -> x,
                        engine.player -> y,
                        engine.fovRadius);
}


// Functions that build the map. These are called once on start up 
// and then never again. 
void Map::dig( int x1, int y1, int x2, int y2)
{
    // Switch values if out of order
    if (x2 < x1) 
    {
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }

    if (y2 < y1)
    {
        int tmp = y2;
        y2 = y1;
        y1 = tmp;
    }
    for (int tile_x = x1; tile_x <= x2; tile_x++)
    {
        for (int tile_y = y1; tile_y <= y2; tile_y++)
        {
            map -> setProperties(tile_x, tile_y, true, true);
        }
    }
}

void Map::createRoom( bool first, int x1, int y1, int x2, int y2)
{
    dig (x1, y1, x2, y2);
    if( first )
    {
        // Put the player in the first room
        engine.player -> x = (x1 + x2)/2;
        engine.player -> y = (y1 + y2) / 2;
    }
    else
    {
        TCODRandom *rng = TCODRandom::getInstance();
        int nMonsters = rng -> getInt(0, MAX_ROOM_MONSTERS);
        while (nMonsters > 0)
        {
            int x = rng -> getInt(x1, x2);
            int y = rng -> getInt(y1, y2);
            if (canWalk(x, y))
            {
                addMonster(x, y);
            }
            nMonsters--;
            
        }
    }
    
}

void Map::addMonster(int x, int y)
{
    TCODRandom *rng = TCODRandom::getInstance();
    if ( rng -> getInt(0, 100) < 80 )
    {
        // Create an orc
        engine.actors.push_back(new Actor (x, y,
                                      'o', "orc",
                                      TCODColor::desaturatedGreen));
    }
    else
    {
        // Create a troll
        engine.actors.push_back(new Actor (x, y, 'T', "troll", TCODColor::darkerGreen));
    }
    
}

void Map::render() const
{
    // Static makes it so that we only create these variables on the first call and not over and over again. 
    // const helps with optimization, tells compiler that we wont be changing these things. 
    static const TCODColor darkWall(0, 0, 100);
    static const TCODColor darkGround(50,50,150);

    static const TCODColor lightWall(130, 110, 50);
    static const TCODColor lightGround(200, 180, 50);

    static const TCODColor bloodyFloor = TCODColor::darkerCrimson;


    // Now we need to scan the map and make all the tiles what they should be. 
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (isInFOV(x, y))
            {
                TCODConsole::root -> setCharBackground(x, y, isWall(x,y) ? lightWall : lightGround);
            }
            else if ( isExplored(x, y) )
            {
                TCODConsole::root -> setCharBackground(x, y, isWall(x,y) ? darkWall : darkGround);
            }
        }
    }
}