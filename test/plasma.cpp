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

#undef main

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

class Cell {
  public:
   virtual ~Cell() = default;
};

class GameField {
  public:
    virtual ~GameField() = default;

    int GetWidth() const;
    int GetHeight() const;

    Cell* GetCell(int x, int y) const;
};

class Food {
  public:
   virtual ~Food() = default;
};

struct vec2 {
  int x, y;
};

enum Direction {
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

class Snake : public Updatable {
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

  private:
   std::vector<vec2> chain;
   Direction direction;
};

constexpr const auto kQuant = 1000ms;

class Game {
  public:
    Game() : snake({0, 0}) {
    }

    ~Game() = default;

    void Update(float quant) {
      std::cout << quant << std::endl;
      for (Updatable* u: updatable) {
          u->Update(quant);
      }
    }

    void Render() {
      for (Renderable* r: renderable) {
        r->Render();
      }
    }

    void HandleInput() {

    }

    bool IsOver() const {
      return is_over;
    }

  private:
   Snake snake;
   GameField game_field;

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
    setbkcolor (BLACK);
    cleardevice ();

    using clock = std::chrono::high_resolution_clock;

    Game game;
    auto ticks = clock::now();
    while (!game.IsOver()) {
      auto delta = clock::now() - ticks;
      ticks = clock::now();
      if (delta < kQuant) {
        std::this_thread::sleep_for(kQuant / 2);
      }
      game.Render();
      refresh();
      game.HandleInput();

      while (delta >= kQuant) {
        game.Update(std::chrono::duration_cast<std::chrono::milliseconds>(kQuant).count());
        delta -= kQuant;
      }
    }

    closegraph ();
    return 0;
} // main ()

// ----- end of file moveit.c

