#ifndef ACTOR_H
#define ACTOR_H

class Actor 
{ 
    public :
        int x;
        int y;  // position on map
        int ch; // ascii code as an int. Lets us go over 256. 
        
        TCODColor color; // color

        Actor(int x, int y, int ch, const TCODColor &color);
    
        void render() const;
};

#endif // ACTOR_H;