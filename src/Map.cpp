#include "libtcod.hpp"
#include "Map.hpp"
#include "Actor.hpp"
#include "Engine.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

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
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MIN_SIZE, 1.5f, 1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder( &listener, NULL);
}

// Destructor releases everything that was allocated in the constructor

Map::~Map()
{
    delete [] tiles;
}

bool Map::isWall(int x, int y) const
{
    return !tiles[x+y*width].canWalk;
}

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
            tiles[tile_x+tile_y*width].canWalk = true;
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
        // Randomly add a character to some of the rooms. 
        TCODRandom *rng = TCODRandom::getInstance();
        if ( rng -> getInt(0, 3) == 0)
        {
            engine.actors.push(new Actor ((x1 + x2) /2, (y1 + y2)/2, '@', TCODColor::yellow));
        }
    }
    
}

void Map::render() const
{
    // Static makes it so that we only create these variables on the first call and not over and over again. 
    // const helps with optimization, tells compiler that we wont be changing these things. 
    static const TCODColor darkWall(0, 0, 100);
    static const TCODColor darkGround(50,50,150);

    // Now we need to scan the map and make all the tiles what they should be. 
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            TCODConsole::root -> setCharBackground(x, y, isWall(x, y) ? darkWall : darkGround);
        }
    }
}