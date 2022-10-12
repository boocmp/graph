#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphics.h>
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

// 1. GameField
   // -- cells : usual, tunnel, wall
// 2. Snake
// 3. Food
   // -- apple, poison, etc
// 4. Stats

class Renderable {
  public:
    virtual ~Renderable() = default;
    virtual void Render() = 0;
};

class Updatable {
  public:
  virtual ~Updatable() = default;
  virtual void Update(float dt) = 0;
};

class Cell : public Renderable {
  public:
   static constexpr const int kCellWidth = 24;
   static constexpr const int kCellHeight = 24;
   virtual ~Cell() = default;
};

class EmptyCell : public Cell {
  public:
   EmptyCell(int _x, int _y): x(_x), y(_y) {}

   void Render() override {
    setcolor(WHITE);
    rectangle(x * kCellWidth, y * kCellHeight,
             (x+1)*kCellWidth, (y+1)*kCellHeight);
   }

  private:
   int x = 0;
   int y = 0;
};

class GameField : public Renderable {
  public:
    GameField(int w, int h) : width(w), height(h) {
      cells.resize(width);
      for (auto& column: cells) {
        column.resize(height);
      }
      for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
          cells[x][y] = std::make_unique<EmptyCell>(x, y);
        }
      }
    }
    virtual ~GameField() = default;

    int GetWidth() const {
      return width;
    }
    int GetHeight() const {
      return height;
    }

    Cell* GetCell(int x, int y) const {
      return cells[x][y].get();
    }

    void Render() override {
      const int kCellWidth = 16;
      const int kCellHeight = 16;

      for (int x = 0; x < GetWidth(); ++x) {
        for (int y = 0; y < GetHeight(); ++y) {
          Cell* cell = GetCell(x, y);
          cell->Render();
        }
      }
    }

  private:
    int width = 0;
    int height = 0;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;
};

class Food {
  public:
   virtual ~Food() = default;
};

struct vec2 {
  int x, y;

  bool operator ==(const vec2& v) const {
    return x == v.x && y == v.y;
  }
};

enum Direction {
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

class Snake : public Updatable, public Renderable {
  public:
    explicit Snake(const vec2& start_position) {
      chain.push_back(start_position);
      direction = NONE;
    }

    size_t GetLength() const {
      return chain.size();
    }

    const std::vector<vec2>& GetChain() const {
      return chain;
    }

    const vec2& GetHead() const {
      return GetChain()[0];
    }

    Direction GetDirection() const {
      return direction;
    }

    void SetDirection(Direction dir) {
      if (dir == NONE)
        return;
      switch (dir) {
        case NONE:
        return;
        case LEFT: if (direction == RIGHT) return;
        case RIGHT: if (direction == LEFT) return;
        case UP: if (direction == DOWN) return;
        case DOWN: if (direction == UP) return;
      }
      direction = dir;
    }

    void Update(float dt) override {
      vec2 head = chain[0];
      switch (direction) {
        case NONE: return;
        case UP: head.y--; break;
        case DOWN: head.y++; break;
        case RIGHT: head.x++; break;
        case LEFT: head.x--; break;
      }
      chain.insert(chain.begin(), head);
      chain.pop_back();
    }

    void Render() override {
      setcolor(YELLOW);
      for (const vec2& c: chain) {
        circle(c.x * Cell::kCellWidth + Cell::kCellWidth/2,
               c.y * Cell::kCellHeight + Cell::kCellHeight/2, 7);
      }
    }

  private:
   std::vector<vec2> chain;
   Direction direction;
};

constexpr const auto kQuant = 1000ms;

class Game {
  public:
    Game() :
     game_field(24, 24),
     snake({game_field.GetWidth()/2, game_field.GetHeight() / 2}) {
      renderable.push_back(&game_field);
      renderable.push_back(&snake);

      updatable.push_back(&snake);
    }

    ~Game() = default;

    void Update(float quant) {
      std::cout << quant << std::endl;
      for (Updatable* u: updatable) {
          u->Update(quant);
      }
      // Check rule:
      const vec2& head = snake.GetHead();
      if (head.x < 0 || head.x >= game_field.GetWidth()) {
        return;
      }
      if (head.y < 0 || head.y >= game_field.GetHeight()) {
        is_over = true;
        return;
      }
      for (size_t i = 1; i < snake.GetChain().size(); ++i) {
        if (snake.GetChain()[i] == head) {
          is_over = true;
          return;
        }
      }
    }

    void Render() {
      for (Renderable* r: renderable) {
        r->Render();
      }
      refresh();
    }

    void HandleInput() {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: is_over = true; break;
        case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case KEY_UP: snake.SetDirection(UP); break;
          case KEY_DOWN: snake.SetDirection(DOWN); break;
          case KEY_LEFT: snake.SetDirection(LEFT); break;
          case KEY_RIGHT: snake.SetDirection(RIGHT); break;
        }
      }
      }
    }

    bool IsOver() const {
      return is_over;
    }

  private:
   GameField game_field;
   Snake snake;

   std::vector<Updatable*> updatable;
   std::vector<Renderable*> renderable;
   bool is_over = false;
};

int main (int argc, char *argv[])
{
    int gd = SDL;
    int gm = SDL_800x600;
    char s[] = "";
    initgraph (&gd, &gm, s);
    sdlbgifast();
    setbkcolor (BLACK);
    cleardevice ();

    using clock = std::chrono::high_resolution_clock;

    Game game;
    auto ticks = clock::now();
    while (!game.IsOver()) {
      cleardevice();
      /*auto delta = clock::now() - ticks;
      ticks = clock::now();
      if (delta < kQuant) {
        std::this_thread::sleep_for(kQuant / 2);
      }*/
      game.Render();
      game.HandleInput();
      game.Update(16);
      SDL_Delay(120);



      /*while (delta >= kQuant) {
        game.Update(std::chrono::duration_cast<std::chrono::milliseconds>(kQuant).count());
        delta -= kQuant;
      }*/
    }

    closegraph ();
    return 0;
} // main ()

// ----- end of file moveit.c

