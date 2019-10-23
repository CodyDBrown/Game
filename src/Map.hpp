#ifndef MAP_H
#define MAP_H

struct Tile
// struct is basically the same except that the field defaults to public,
// where class defaults to private. 
{
    public: 
        bool canWalk; // Can we walk through this tile or not?
        
        // constructor
        Tile() : canWalk(false) {}
};

class Map
{
    public:
        int width, height;

        Map(int width, int height);
        ~Map();
        bool isWall(int x, int y) const;
        void render() const;
    
    protected:
        Tile *tiles;
        friend class BspListener;
        
        void dig(int x1, int y1, int x2, int y2);
        void createRoom (bool first, int x1, int y1, int x2, int y2);
};

#endif // MAP_H