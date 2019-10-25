#ifndef ACTOR_H
#define ACTOR_H

class Actor 
{ 
    public :
        int x;
        int y;  // position on map
        int ch; // ascii code as an int. Lets us go over 256. 
        const char * name;


        TCODColor color;

        Actor(int x, int y, int ch, const char * name, const TCODColor &color);

        void update();
        bool moveOrAttack(int x, int y);
        void render() const;
};

#endif // ACTOR_H;