/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_


#include <HAL/HAL.h>


typedef enum {CPLAYGAME, CHOWTOPLAY, CHIGHSCORES} Cursor_Index;

typedef enum {SPLASH, MAIN, GAME, HOWTOPLAY, HIGHSCORES, GAMEOVER} Game_State;

//typedef enum {QUARTER, HALF, THREEQUARTER, FULL} Loadbar_State;


struct _Missile
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    bool inPlay;

    int xPo;
    int yPo;
};
typedef struct _Missile Missile;

struct _Meteor
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    bool inPlay;

    int collisionLeft;
    int collisionRight;
    int collisionBottom;
    int collisionTop;

    int xPos;
    int yPos;
};
typedef struct _Meteor Meteor;

struct _GameStruct
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    int xPos;
    int yPos;

    Meteor meteor1;
    Meteor meteor2;
    Meteor meteor3;

    Missile missile1;
    Missile missile2;

    int totalScore;

    int numberLives;


    SWTimer meteorTimer;
    SWTimer advanceTimer;
};
typedef struct _GameStruct GameStruct;

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    //UART_Baudrate baudChoice;

    Cursor_Index cursorIndex;
    bool newGame;
    int firstScore;
    int secondScore;
    int thirdScore;

    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;

    bool moveFarUp;
    bool moveFarDown;
    bool moveFarLeft;
    bool moveFarRight;

    unsigned xJoyPos;

    unsigned yJoyPos;

    SWTimer threeSecondTimer;

    Game_State gameState;

    GameStruct game;

};
typedef struct _Application Application;






// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

Meteor Meteor_construct();
Meteor Meteor_init(int startPos);

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);

// Called whenever the UART module needs to be updated
//void Application_updateCommunications(Application* app, HAL* hal);

// Splash screen display function
void screenSplash(Application *app_p, HAL *hal_p);
// Main screen display function
void screenMain(Application *app_p, HAL *hal_p);
// HowToPlay screen display function
void screenHowToPlay(Application *app_p, HAL *hal_p);
// Highscores screen display function
void screenHighScores(Application *app_p, HAL *hal_p);
// Game screen display function, and game functionality handling
void screenGame(Application *app_p, HAL *hal_p);
// GameOver screen display function
void screenGameOver(Application *app_p, HAL *hal_p);
// Clears Entire Display
void clearDisplay(Application *app_p, HAL *hal_p);

// Main FSM function to handle game screens
void applicationFSM(Application *app_p, HAL *hal_p);

// Handles movestate for cursors in the main screen
void moveState(Application *app_p, HAL *hal_p);

// Handles movement of the cursor on the main menu screen
void cursorHandle(Application *app_p, HAL *hal_p, bool upMove);

// Handles moving the player position based on the movestate
void gameMovementHandle(Application *app_p, HAL *hal_p);
// Handles movestate for cursors during the running game
void moveStateNoD(Application *app_p, HAL *hal_p);

// Displays number of lives on screen
void displayLives(Application *app_p, HAL *hal_p);

// Handles creation of meteors and their advancing on the screen
void handleMeteors(Application *app_p, HAL *hal_p);
// Displays meteors in play
void displayMeteors(Application *app_p, HAL *hal_p);

// Handles all the collision handling methods
void collisionHandle(Application *app_p, HAL *hal_p);

//Clears meteor 1 display
void clearMeteor1(Application *app_p, HAL *hal_p);
//Clears meteor 2 display
void clearMeteor2(Application *app_p, HAL *hal_p);
//Clears meteor 3 display
void clearMeteor3(Application *app_p, HAL *hal_p);

// Handles ship colliding with asteroids
void shipCollision(Application *app_p, HAL *hal_p);

// Handles meteor colliding with ground
void groundCollision(Application *app_p, HAL *hal_p);
// Subtracts life from player and updates display
void subtractLife(Application *app_p, HAL *hal_p);

// Displays missiles in play
void displayMissiles(Application *app_p, HAL *hal_p);
// Handles the creation of new missile on button press
void firingHandle(Application *app_p, HAL *hal_p);
// Initializes new missile absed on position the button in pressed
Missile Missile_init(int xPosIn, int yPosIn);

// Handles missile colliding with roof
void missileCollision(Application *app_p, HAL *hal_p);

// Clears missile1 in the display
void clearMissile1(Application *app_p, HAL *hal_p);

// Handles missile1 colliding with meteors
void missile1MeteorCollisions(Application *app_p, HAL *hal_p);
// Handles missile2 colliding with meteors
void missile2MeteorCollisions(Application *app_p, HAL *hal_p);

// Rearranges the order of the scores based on the new total score of ended game
void modifyHighScores(Application *app_p, HAL *hal_p);
// Draws the spaceship in the display based on joystick position
void drawSpaceShip(Application *app_p, HAL *hal_p);

// Draws the spaceship in the display based on joystick position
void displayAnimation(Application *app_p, HAL *hal_p);

// Draws a red X over meteor1 position
void xMeteor1(Application *app_p, HAL *hal_p);
// Draws a red X over meteor2 position
void xMeteor2(Application *app_p, HAL *hal_p);
// Draws a red X over meteor3 position
void xMeteor3(Application *app_p, HAL *hal_p);
#endif /* APPLICATION_H_ */
