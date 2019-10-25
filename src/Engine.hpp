#ifndef ENGINE_H
#define ENGINE_H

class Engine 
{
   public :
      std::vector<Actor *> actors;
      Actor *player;
      Map *map;

      Engine();
      ~Engine();
      void update();
      void render();

      int fovRadius;

      enum GameStatus
      {
         STARTUP, 
         IDLE,
         NEW_TURN,
         VICTORY,
         DEFEAT
      } gameStatus; 

   private:
      bool computeFov_;
};

extern Engine engine;

#endif // ENGINE_H