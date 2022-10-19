#include <vector>
#include <memory>
#include <functional>
#include <graphics.h>

#undef main

class Cell {
  public:
   virtual ~Cell() = default;
   virtual void Render(int x, int y) = 0;

   void OnVisit(std::function<void()> on_visit) {
     this->on_visit = on_visit;
   }

   void Visit() {
     if (on_visit) {
       on_visit();
     }
   }

  private:
   std::function<void()> on_visit;
};

class EmptyCell : public Cell {
  public:
  void Render(int x, int y) override {
    setcolor(WHITE);
    rectangle(x * 24, y * 24, (x+1) * 24, (y+1) * 24);
  }
};

class AppleCell : public Cell {
  public:
   void Render(int x, int y) {
    setcolor(WHITE);
    rectangle(x * 24, y * 24, (x+1) * 24, (y+1) * 24);
    setfillstyle(SOLID_FILL, RED);
    bar(x * 24 + 3, y*24 + 3, (x+1)*24 -3, (y+1) * 24 - 3);
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

   Cell* GetCell(int x, int y) const {
     if (x < 0 || x >= fields.size()) return nullptr;
     if (y < 0 || y >= fields[x].size()) return nullptr;
     return fields[x][y].get();
   }

   Cell* SetCell(int x, int y, Cell* cell) {
     fields[x][y].reset(cell);
     return fields[x][y].get();
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

   const Coords& GetHead() const {
     return units[0];
   }

   void SetDirection(Direction dir) {
     if (direction != NONE && dir == NONE)
       return;
     if (direction == UP && dir == DOWN)
       return;
     if (direction == DOWN && dir == UP)
       return;
     if (direction == LEFT && dir == RIGHT)
       return;
     if (direction == RIGHT && dir == LEFT)
       return;
     direction = dir;
   }

   void Grow(int count) {
     grow += count;
   }

   void UpdateState() {
     Coords new_head = units[0];
     switch (direction) {
       case NONE: return;
       case UP: new_head.y--; break;
       case DOWN: new_head.y++; break;
       case RIGHT: new_head.x++; break;
       case LEFT: new_head.x--; break;
     }
     units.insert(units.begin(), new_head);
     if (grow > 0) {
       --grow;
       return;
     }

     units.pop_back();
   }

   void Render() {
      for (const Coords& u: units) {
        setcolor(YELLOW);
        circle(u.x * 24 + 12, u.y *24 + 12, 7);
      }
   }

  private:
   Direction direction;
   std::vector<Coords> units;
   int grow = 0;
};

class Game {
  public:

   void AddApple() {
     int x = rand()%game_field.GetWidth();
     int y = rand()%game_field.GetHeight();
     while (x == snake.GetHead().x && y == snake.GetHead().y) {
       x = rand()%game_field.GetWidth();
       y = rand()%game_field.GetHeight();
     }
     auto* apple = new AppleCell;
     apple->OnVisit([this,x, y]() {
       game_field.SetCell(x, y, new EmptyCell);
         snake.Grow(5);
        AddApple();
     } );
     game_field.SetCell(x, y, apple);
    }

   Game() : snake({game_field.GetWidth() >> 1, game_field.GetHeight() >> 1}) {
      srand(time(nullptr));
      AddApple();
      AddApple();
   }

   bool IsOver() const {
     return is_over;
   }

   void Render() {
     setbkcolor(BLACK);
     cleardevice();
     game_field.Render();
     snake.Render();
     refresh();
   }

   void ProcessInput() {
     SDL_Event event;
     while (SDL_PollEvent(&event)) {
       if (event.type == SDL_QUIT) {
         is_over = true;
         return;
       }
     }
     const Uint8 *state = SDL_GetKeyboardState(nullptr);
     if (state[SDL_SCANCODE_RIGHT]) {
       snake.SetDirection(RIGHT);
     } else if (state[SDL_SCANCODE_LEFT]) {
       snake.SetDirection(LEFT);
     } else if (state[SDL_SCANCODE_UP]) {
       snake.SetDirection(UP);
     } else if (state[SDL_SCANCODE_DOWN]) {
       snake.SetDirection(DOWN);
     }
   }

   void UpdateState(int dt) {
     snake.UpdateState();
     CheckRules();
     if (Cell* cell = game_field.GetCell(snake.GetHead().x, snake.GetHead().y)) {
       cell->Visit();
     }
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

  int gd = SDL;
  int gm = SDL_800x600;
  initgraph (&gd, &gm, (char*)"Snake");
  sdlbgifast();

  Game game;
  while (!game.IsOver()) {
    game.ProcessInput();
    game.Render();
    game.UpdateState(16);
    SDL_Delay(80);
  }

  closegraph();
  return 0;
}
