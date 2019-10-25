#ifndef MAP_H
#define MAP_H

struct Tile
// struct is basically the same except that the field defaults to public,
// where class defaults to private. 
{
    public: 
        bool explored; // Had the player explored these tiles or not?
        // Constructor
        Tile() : explored(false) {} 

        enum Type
        {
            WALL,
            FLOOR,
            LAVA,
            BLOODY
        };
};

class Map
{
    public:
        int width, height;

        Map(int width, int height);
        ~Map();

        bool isWall(int x, int y) const;
        bool canWalk(int x, int y) const;
        bool isInFOV(int x, int y) const;
        bool isExplored(int x, int y) const;

        void computeFOV();
        void render() const;
    
    protected:
        Tile *tiles;
        TCODMap *map;
        friend class BspListener;
        
        void dig(int x1, int y1, int x2, int y2);
        void createRoom (bool first, int x1, int y1, int x2, int y2);
        void addMonster(int x, int y);
};

#endif // MAP_H