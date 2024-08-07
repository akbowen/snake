#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <stdbool.h>
#include <termios.h>

// declare functions
void run();
void printMap();
void initMap();
void move(int dx, int dy);
void update();
void changeDirection(char key);
void clearScreen();
void generateFood();

char getMapValue(int value);

// map dimensions
const int mapwidth = 20;
const int mapheight = 20;

const int size = mapwidth * mapheight;

// tile values for map
int map[size];

// snake head details
int headxpos;
int headypos;
int direction;

// amount of food snake starts with (how long snake is)
int food = 3;

// determine if running
bool running;

int main()
{
    run();
    return 0;
}

// Function to set terminal to non-canonical mode
void setNonCanonicalMode(struct termios *original)
{
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, original);
    new_settings = *original;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

// Function to restore terminal settings
void restoreTerminalMode(struct termios *original)
{
    tcsetattr(STDIN_FILENO, TCSANOW, original);
}

// Main game logic
void run()
{
    struct termios original_settings;
    setNonCanonicalMode(&original_settings);

    // initialize map
    initMap();
    running = true;

    while (running)
    {
        // Set up the file descriptor set
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        // Set up the timeout
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        // Check if there is input available
        int result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
        {
            // Input is available, read it
            int key = getchar();
            changeDirection(key);
        }

        // update map
        update();

        // clear screen
        clearScreen();

        // print map
        printMap();

        // wait .5s
        usleep(500000);
    }

    restoreTerminalMode(&original_settings);
}

// change snake's direction
void changeDirection(char key)
{
  /*
    W=1
  A=4 D=2
    S=3
  */   
  switch(key)
  {
    case 'w':
         if(direction != 2) direction = 0;
         break;
    case 'd':
         if(direction != 3) direction = 1;
         break;
    case 's':
         if(direction != 4) direction = 2;
         break;
    case 'a':
         if(direction != 5) direction = 3;
         break;
  }
}

// move the snake's head
void move(int dx, int dy)
{
  // determine new location
  int newx = headxpos + dx;
  int newy = headypos + dy;
  
  // Is there food there?
  if(map[newx + newy * mapwidth] == -2)
  {
    // make the body bigger
    food++;
    
    // put another food on the map
    generateFood();          
  }
  // Is the space free?
  else if(map[newx + newy * mapwidth] != 0)
  {
    running = false;   
  }
  // move the head to new location
  headxpos = newx;
  headypos = newy;
  map[headxpos + headypos * mapwidth] = food + 1;
}

// clear the screen
void clearScreen()
{
  system("clear");     
}

// generate food
void generateFood()
{
  int x = 0;
  int y = 0;
  do
  {  
    x = rand() % (mapwidth - 2) + 1;
    y = rand() % (mapheight - 2) + 1;
  } while(map[x + y * mapwidth] != 0);   
  
  // place the food
  map[x + y * mapwidth] = -2;
}

// update map
void update()
{
  switch(direction)
  {
    case 0:
         move(-1, 0);
         break;
    case 1:
         move(0, 1);
         break;
    case 2:
         move(1, 0);
         break;
    case 3:
         move(0, -1);
         break;               
  }
  for(int i = 0; i < size; i++)
  {
     if(map[i] > 0) map[i]--;       
  }
}
// initialize map
void initMap()
{
  // place the head in the middle
  headxpos = mapwidth / 2;
  headypos = mapheight / 2;
  map[headxpos + headypos * mapwidth] = 1;
  
  // place top and bottom walls
  for(int x = 0; x < mapwidth; x++)
  {
    map[x] = -1;
    map[x + (mapheight - 1) * mapwidth] = -1;        
  }
  // place side walls
  for(int y = 0; y < mapheight; y++)
  {
    map[0 + y * mapwidth] = -1;
    map[(mapwidth - 1) + y * mapwidth] = -1;        
  }
  // generate food
  generateFood();   
}

// print map to console
void printMap()
{
  for(int x = 0; x < mapwidth; x++)
  {
    for(int y = 0; y < mapheight; y++)
    {
      printf("%c", getMapValue(map[x + y * mapwidth]));        
    }
    printf("\n");  
  }        
}

// return the character to display
char getMapValue(int value)
{
  // body
  if(value > 0) return 'o';
  
  switch(value)
  {
    // wall
    case -1:
         return 'X';
         break;
    // food
    case -2:
         return 'F';
         break;
                
  }     

  return ' ';
}
