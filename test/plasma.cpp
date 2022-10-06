#include <vector>
#include <graphics.h>

#undef main

class Cell {
  public:
   virtual ~Cell() = default;
   virtual void Render() = 0;
};

class GameField {
  public:
   GameFields(int w = 12, int h = 24);

   int GetWidth() const { return fields.size(); }
   int GetHeight() const { return fields[0].size(); }

   void Render() {
     // Render background
     for (int x = 0; x < GetWidth(); ++x)
       for (int y = 0; y < GetHeight(); ++y) {
          Coords cell_coors = {x, y};
          fields[x][y]->Render();
       }
     }
   }

  private:
   std::vector<std::vector<Cell*>> fields;
};

struct Coords {
  int x, y;
};

enum Direction {
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

class Snake {
  public:
    explicit Snake(const Coords& head) {
     direction = NONE;
     units.push_back(head);
   }

   const std::vector<Coords>& GetUnits() const {
     return units;
   }

   void SetDirection(Direction dir) {
     if (direction != NONE && dir == NONE)
       return;
     if (direction == UP && dir == DOWN)
       return;
     direction = dir;
   }

   void UpdateState() {
     if (direction == NONE)
       return;

     Coords new_head = units[0];
     switch (direction) {
       case UP: {
         new_head.y--;
       }break;
       case RIGHT: {
         new_head.x += 1;
       }break;
     }
     units.push_front(new_head);
     units.pop_pack();
   }

  private:
   Direction direction;
   std::vector<Coords> units;
};

class Game {
  public:
   Game() {}

   bool IsOver() const {
     return is_over;
   }

   void Render() {
     game_field.Render();
     snake.Render();
   }

   void ProcessInput() {
      int key = GetPressedKey();
      if (!key)
        return;
      switch (key) {
        case 'w':
          snake.SetDirection(UP);
        break;
      }
   }

   void UpdateState(int dt) {
     snake.UpdateState();
     game_field.UpdateState();
     CheckRules();
   }

  private:
   void CheckRules() {
     // Check snake & set is_over to true if it dies.
   }

   GameField game_field;
   Snake snake;

   bool is_over = false;
};

int GetTicks();

int main (int argc, char *argv[]) {
  Game game;
  int time = GetTicks();
  while (!game.IsOver()) {
    game.ProcessInput();
    game.Render();
    game.UpdateState(GetTicks() - time);
    time = GetTicks();
  }
  return 0;
}
