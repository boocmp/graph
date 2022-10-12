#include <vector>
#include <memory>
#include <graphics.h>

#undef main

class Cell {
  public:
   virtual ~Cell() = default;
   virtual void Render(int x, int y) = 0;
};

class EmptyCell : public Cell {
  public:
  void Render(int x, int y) override {

  }
};

class GameField {
  public:
   GameField(int w = 24, int h = 24) {
     fields.resize(w);
     for (auto& column: fields) {
       column.resize(h);
       for (auto& cell : column) {
         cell = std::make_unique<EmptyCell>();
       }
     }
   }

   int GetWidth() const { return fields.size(); }
   int GetHeight() const { return fields[0].size(); }

   void Render() {
     // Render background
     for (int x = 0; x < GetWidth(); ++x) {
       for (int y = 0; y < GetHeight(); ++y) {
          fields[x][y]->Render(x , y);
       }
     }
   }

  private:
   std::vector<std::vector<std::unique_ptr<Cell>>> fields;
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
       case NONE: return;
       case UP: new_head.y--; break;
       case DOWN: new_head.y++; break;
       case RIGHT: new_head.x++; break;
       case LEFT: new_head.x--; break;
     }
     units.insert(units.begin(), new_head);
     units.pop_back();
   }

   void Render() {

   }

  private:
   Direction direction;
   std::vector<Coords> units;
};

class Game {
  public:
   Game() : snake({game_field.GetWidth()/2, game_field.GetHeight()/2}) {}

   bool IsOver() const {
     return is_over;
   }

   void Render() {
     game_field.Render();
     snake.Render();
   }

   void ProcessInput() {
   }

   void UpdateState(int dt) {
     snake.UpdateState();
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
  while (!game.IsOver()) {
    game.ProcessInput();
    game.Render();
    game.UpdateState(16);
  }
  return 0;
}
