#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphics.h>

// -----

#undef main

int main (int argc, char *argv[])
{
  int gd = SDL;
  int gm = SDL_800x600;
  initgraph (&gd, &gm, "");
  
  setbkcolor (BLACK);
  cleardevice ();
  bool stop = false;
  unsigned int i = 0;
  do {
   setcolor(COLOR(255, 10, 10));
   setfillstyle(SOLID_FILL, COLOR(255, 10, 10));
   bar(100, 100, 130, 130);
   line(200, 200, 250, 250);
   putpixel(300, 300, COLOR(255,0 ,0));
   refresh();

    // added!
   while (event()) {
      int c = getevent();
      if (KEY_ESC == c || QUIT == c)
        stop = true;
   }

  } while (!stop);
  
  closegraph ();
  return 0;
  
} // main ()

// ----- end of file moveit.c

