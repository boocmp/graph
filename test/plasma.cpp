#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphics.h>

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
      // TODO(): Add some logic
      direction = dir;
    }

    void Update(float dt) override {
      switch (direction) {
        case NONE: return;
        case UP: {
          vec2 head = chain[0];
          head.y += 1;
          chain.push_front(head);
          chain.pop_back();
        }break;
      }
    }

  private:
   std::vector<vec2> chain;
   Direction direction;
};

class Game {
  public:
    Game() {
    }

    ~Game() = default;

    void Update(float dt) {
      const float quant = 0.01f;
      for (float t = 0; t < dt; t += quant) {
        for (Updatable* u: updatable) {
          u->Update(quant);
        }
      }
      // new state:
      // Check Consistency

    }

    void Render() {
      for (Renderable* r: renderable) {
        r->Render();
      }
    }

    void HandleInput();

    bool IsOver() const {
      return is_over;
    }

  private:
   Snake snake;
   GameField game_field;

   std::vector<Updatable*> updatable;
   std::vector<Renderable*> renderable;
};

int GetTicks();

int main (int argc, char *argv[])
{
    Game game;
    int ticks = GetTicks();
    while (!game.IsOver()) {
      game.Render();
      game.HandleInput();
      game.Update(GetTicks() - ticks);
      ticks = GetTicks();
    }

    return 0;
} // main ()

// ----- end of file moveit.c

