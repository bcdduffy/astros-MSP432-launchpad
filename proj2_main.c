/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>

#define XGAMECENTER  4
#define YGAMECENTER  123
#define MAX_NUM_LIVES  3
#define GAME_X_START_POS 93
#define GAME_Y_START_POS 120
#define ADVANCE_TIMER_MS 25
#define ROOF_BOUND 5
#define PROG_BAR_LEFT_X 24
#define PROG_BAR_RIGHT_X 104
#define PROG_BAR_TOP_Y 100
#define PROG_BAR_BOTTOM_Y 110
#define PROG_BAR_FULL 104
#define PROG_BAR_THREEFOURTHS 84
#define PROG_BAR_HALF 64
#define PROG_BAR_QUARTER 44
#define SCORE_INCREMENT 500
//#define MAIN_BOX_Y_MIN  12
//#define MAIN_BOX_Y_MAX  96

/**
 * Starter code for Project 2. Good luck!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void PollNonBlockingLED()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

/**
 * Main function that runs, instantiating the HAL and application, then looping
 * through the application functions
 *
 * @param xPosIn:  X position of the new missle object.
 * @param yPosIn:  Y position of the new missle object.
 */
int main()
{
    WDT_A_holdTimer();


    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();


    WDT_A_hold(WDT_A_BASE);
    initADC();
    initJoyStick();
    startADC();
    //initLEDs();

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();
    while (1)
    {
        // Do not remove this line. This is your non-blocking check.
        PollNonBlockingLED();

        HAL_refresh(&hal);
        Application_loop(&app, &hal);
    }
}

/**
 * Constructs a new application struct setting scores to 0 and initializing the gamestate, while
 * constructing timers for the splash screen
 *
 * @param xPosIn:  X position of the new missle object.
 * @param yPosIn:  Y position of the new missle object.
 */
Application Application_construct()
{
    Application app;

    // Initialize local application state variables here!
    //app.baudChoice = BAUD_9600;
    app.gameState = SPLASH;
    app.firstScore = 0;
    app.secondScore = 0;
    app.thirdScore = 0;
    app.newGame = true;
    app.cursorIndex = CPLAYGAME;

    app.moveUp = false;
    app.moveDown = false;
    app.moveLeft = false;
    app.moveRight = false;


    app.threeSecondTimer = SWTimer_construct(3000);
    SWTimer_start(&app.threeSecondTimer);


    return app;
}

/**
 * Constructs a new meteor struct setting its position to 0,0 and inPlay to false
 *
 * @param xPosIn:  X position of the new missle object.
 * @param yPosIn:  Y position of the new missle object.
 */
Meteor Meteor_construct()
{
    Meteor constMeteor;

    // Initialize local application state variables here!
    //app.baudChoice = BAUD_9600;
    constMeteor.inPlay = false;

    constMeteor.xPos = 0;
    constMeteor.yPos = 0;

    constMeteor.collisionLeft = 0;
    constMeteor.collisionRight = 0;
    constMeteor.collisionBottom = 0;

    return constMeteor;
}


/**
 * Initializes a new meteor struct setting position of meteor to the passed x position
 * spawnign it at the top of the game map
 *
 * @param xPosIn:  X position of the new missle object.
 * @param yPosIn:  Y position of the new missle object.
 */
Meteor Meteor_init(int startPos)
{
    Meteor initMeteor;

    initMeteor.inPlay = true;

    initMeteor.xPos = startPos;
    initMeteor.yPos = 4;

    initMeteor.collisionLeft = startPos - 5;
    initMeteor.collisionRight = startPos +5;
    initMeteor.collisionBottom = 7;
    initMeteor.collisionTop = 0;

    return initMeteor;
}

/**
 * Initializes a new missile struct setting position of missile to the x and y position
 * passed which is the player position.
 *
 * @param xPosIn:  X position of the new missle object.
 * @param yPosIn:  Y position of the new missle object.
 */
Missile Missile_init(int xPosIn, int yPosIn)
{
    Missile initMissile;
    // Initialize local application state variables here!
    //app.baudChoice = BAUD_9600;

    initMissile.inPlay = true;

    initMissile.xPo = xPosIn;
    initMissile.yPo = yPosIn - ROOF_BOUND;

    return initMissile;
}

/**
 * Initializes a new game struct setting position of player to center of gameplay screen,
 * lives to 3, score to 0, and meteors and missiles to out of play
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
GameStruct GameStruct_init()
{
    GameStruct game;

    // Initialize local application state variables here!
    game.xPos = GAME_X_START_POS;
    game.yPos = GAME_Y_START_POS;

    game.totalScore = 0;
    game.numberLives = MAX_NUM_LIVES;

    game.meteor1.inPlay = false;
    game.meteor2.inPlay = false;
    game.meteor3.inPlay = false;

    game.missile1.inPlay = false;
    game.missile2.inPlay = false;

    // a 3-second timer (i.e. 3000 ms as specified in the SWTimer_contruct)
    //game.meteorTimer = SWTimer_construct(3000);
    //SWTimer_start(&game.meteorTimer);

    game.advanceTimer = SWTimer_construct(ADVANCE_TIMER_MS);
    SWTimer_start(&game.advanceTimer);

    return game;
}

/**
 * Main application loop with nonblocking code
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void Application_loop(Application *app_p, HAL *hal_p)
{
    // The obligatory non-blocking check. At any time in your code, pressing and
    // holding Launchpad S1 should always turn on Launchpad LED1.
    //
    if (Button_isPressed(&hal_p->launchpadS1))
        LED_turnOn(&hal_p->launchpadLED1);
    else
        LED_turnOff(&hal_p->launchpadLED1);
    applicationFSM(app_p, hal_p);

}

/**
 * Main application FSM both choosing which screen is displayed based on the gamestate
 * handling also transitions from one gamestate to another
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void applicationFSM(Application *app_p, HAL *hal_p){
    switch (app_p->gameState){
    case SPLASH:
        screenSplash(app_p, hal_p);
        if (SWTimer_expired(&app_p->threeSecondTimer)){
            app_p->gameState = MAIN;
            clearDisplay(app_p, hal_p);}
        break;
    case MAIN:
        screenMain(app_p, hal_p);
        if (app_p->cursorIndex == CPLAYGAME && Button_isTapped(&hal_p->boosterpackJS)){
            app_p->gameState = GAME;
            app_p->game = GameStruct_init();
            srand(app_p->xJoyPos);
            clearDisplay(app_p, hal_p);}
        else if (app_p->cursorIndex == CHOWTOPLAY && Button_isTapped(&hal_p->boosterpackJS)){
            app_p->gameState = HOWTOPLAY;
            clearDisplay(app_p, hal_p);
            screenHowToPlay(app_p, hal_p);}
        else if (app_p->cursorIndex == CHIGHSCORES && Button_isTapped(&hal_p->boosterpackJS)){
            app_p->gameState = HIGHSCORES;
            clearDisplay(app_p, hal_p);}
        break;
    case HOWTOPLAY:
        if (Button_isTapped(&hal_p->boosterpackJS)){
            app_p->gameState = MAIN;
            clearDisplay(app_p, hal_p);}
        break;
    case HIGHSCORES:
        screenHighScores(app_p, hal_p);
        if (Button_isTapped(&hal_p->boosterpackJS)){
            app_p->gameState = MAIN;
            clearDisplay(app_p, hal_p);}
        break;
    case GAME:
        screenGame(app_p, hal_p);
        if(app_p->game.numberLives == 0){
            app_p->gameState = GAMEOVER;
            clearDisplay(app_p, hal_p);}
        break;
    case GAMEOVER:
        screenGameOver(app_p, hal_p);
        if (Button_isTapped(&hal_p->boosterpackJS)){
            app_p->gameState = MAIN;
            app_p->newGame = true;
            modifyHighScores(app_p, hal_p);
            clearDisplay(app_p, hal_p);}
        break;
    default:
        break;
    }
}

/**
 * Handles what is displayed when the gamestate is in the initial splash screen
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void screenSplash(Application *app_p, HAL *hal_p)
{
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "ECE 2564 Project 2", -1, 10, 40, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Astro Defenders", -1, 20, 55, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Brenden Duffy", -1, 25, 80, true);


    Graphics_Rectangle progressBox;
    progressBox.xMin = PROG_BAR_LEFT_X;
    progressBox.xMax = PROG_BAR_RIGHT_X;
    progressBox.yMin = PROG_BAR_TOP_Y;
    progressBox.yMax = PROG_BAR_BOTTOM_Y;

    Graphics_drawRectangle(&hal_p->g_sContext, &progressBox);
    int progressPix = 0;
    double progress = SWTimer_percentElapsed(&app_p->threeSecondTimer);
    if (progress > 0.95)
    {
        progressPix = PROG_BAR_FULL;
    }
    else if (progress > 0.75){
        progressPix = PROG_BAR_THREEFOURTHS;
    }
    else if (progress > 0.50){
        progressPix = PROG_BAR_HALF;
        }
    else if (progress > 0.25){
        progressPix = PROG_BAR_QUARTER;
        }
    else if (progress > 0.0){
        progressPix = PROG_BAR_LEFT_X;
            }
    progressBox.xMin = PROG_BAR_LEFT_X;
    progressBox.xMax = progressPix;
    progressBox.yMin = PROG_BAR_TOP_Y;
    progressBox.yMax = PROG_BAR_BOTTOM_Y;
    Graphics_fillRectangle(&hal_p->g_sContext, &progressBox);
    displayAnimation(app_p, hal_p);
}

/**
 * Handles what is displaying the animation of the ship on the splash screen
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void displayAnimation(Application *app_p, HAL *hal_p){
    Graphics_drawLine(&hal_p->g_sContext, 65, 15, 67, 15);
    Graphics_drawLine(&hal_p->g_sContext, 64, 16, 67, 16);
    Graphics_drawLine(&hal_p->g_sContext, 62, 17, 65, 17);
    Graphics_drawLine(&hal_p->g_sContext, 62, 18, 65, 18);
    Graphics_drawLine(&hal_p->g_sContext, 61, 19, 67, 19);
    Graphics_drawLine(&hal_p->g_sContext, 60, 20, 67, 20);
    Graphics_drawLine(&hal_p->g_sContext, 61, 21, 67, 21);
    Graphics_drawLine(&hal_p->g_sContext, 62, 22, 65, 22);
    Graphics_drawLine(&hal_p->g_sContext, 62, 23, 65, 23);
    Graphics_drawLine(&hal_p->g_sContext, 64, 24, 67, 24);
    Graphics_drawLine(&hal_p->g_sContext, 65, 25, 67, 25);
    double progress = SWTimer_percentElapsed(&app_p->threeSecondTimer);
    if (progress > 0.95){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 65, 17, 69, 17);
        Graphics_drawLine(&hal_p->g_sContext, 65, 18, 69, 18);
        Graphics_drawLine(&hal_p->g_sContext, 65, 22, 69, 22);
        Graphics_drawLine(&hal_p->g_sContext, 65, 23, 69, 23);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);}
    else if (progress > 0.75){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 65, 17, 68, 17);
        Graphics_drawLine(&hal_p->g_sContext, 65, 18, 68, 18);
        Graphics_drawLine(&hal_p->g_sContext, 65, 22, 68, 22);
        Graphics_drawLine(&hal_p->g_sContext, 65, 23, 68, 23);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);}
    else if (progress > 0.50){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 65, 17, 67, 17);
        Graphics_drawLine(&hal_p->g_sContext, 65, 18, 67, 18);
        Graphics_drawLine(&hal_p->g_sContext, 65, 22, 67, 22);
        Graphics_drawLine(&hal_p->g_sContext, 65, 23, 67, 23);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);}
    else if (progress > 0.25){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 65, 17, 66, 17);
        Graphics_drawLine(&hal_p->g_sContext, 65, 18, 66, 18);
        Graphics_drawLine(&hal_p->g_sContext, 65, 22, 66, 22);
        Graphics_drawLine(&hal_p->g_sContext, 65, 23, 66, 23);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);}
    else if (progress > 0.0){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_YELLOW);
        Graphics_drawLine(&hal_p->g_sContext, 65, 17, 71, 17);
        Graphics_drawLine(&hal_p->g_sContext, 65, 18, 71, 18);
        Graphics_drawLine(&hal_p->g_sContext, 65, 22, 71, 22);
        Graphics_drawLine(&hal_p->g_sContext, 65, 23, 71, 23);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);}
}

/**
 * Handles what is displayed when the gamestate is in the main screen
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void screenMain(Application *app_p, HAL *hal_p)
{

    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "MAIN MENU", -1, 25, 30, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Play Game", -1, 30, 60, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "How to Play", -1, 30, 75, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "High Scores", -1, 30, 90, true);

    if(app_p->cursorIndex == CPLAYGAME){
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 60, true);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 75, true);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 90, true);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
    }
    else if (app_p->cursorIndex == CHOWTOPLAY){
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 75, true);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 60, true);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 90, true);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
    }
    else if (app_p->cursorIndex == CHIGHSCORES){
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 90, true);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 75, true);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 19, 60, true);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
    }

    moveState(app_p, hal_p);

}

/**
 * Handles what is displayed when the gamestate is in the highscores option
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void screenHowToPlay(Application *app_p, HAL *hal_p)
{
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "HOW TO PLAY", -1, 30, 2, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "---------------------", -1, 0, 10, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Shoot and destroy", -1, 0, 20, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "incoming debris to", -1, 0, 30, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "score points. Running", -1, 0, 40, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "into debris or", -1, 0, 50, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "allowing it to reach", -1, 0, 60, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "the planet costs a", -1, 0, 70, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "life. Try to get the", -1, 0, 80, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "high score! ", -1, 0, 90, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Use JOYSTICK to move", -1, 0, 100, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "To shoot press BB2", -1, 0, 110, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "(Press JSB to return)", -1, 0, 120, true);
}

/**
 * Handles what is displayed when the gamestate is in the highscores option
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void screenHighScores(Application *app_p, HAL *hal_p)
{

    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "HIGH SCORES", -1, 30, 2, true);

    char buffer[10];
    int myNum = app_p->firstScore; // number to print
    snprintf(buffer, 10, "%06d", myNum);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) buffer, -1, 45, 40, true);


    myNum = app_p->secondScore; // number to print
    snprintf(buffer, 10, "%06d", myNum);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) buffer, -1, 45, 60, true);

    myNum = app_p->thirdScore; // number to print
        snprintf(buffer, 10, "%06d", myNum);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) buffer, -1, 45, 80, true);

    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "(Press JSB to return)", -1, 0, 120, true);
}

/**
 * Handles what is displayed when the gamestate is in the main game
 * This also contains all of the functions relevant to the game including
 * player movement, meteors, and scoring
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void screenGame(Application *app_p, HAL *hal_p)
{
    char buffer[10];
    int myNum = app_p->game.totalScore; // number to print
    snprintf(buffer, 10, "%06d", myNum);

    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "ASTRO", -1, 15, 10, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "DEFENDERS", -1, 2, 20, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "SCORE", -1, 2, 50, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) buffer, -1, 15, 60, true);
    Graphics_drawLine(&hal_p->g_sContext, 60, 0, 60, 128);
    Graphics_drawLine(&hal_p->g_sContext, 127, 0, 127, 128);

    moveStateNoD(app_p, hal_p);
    gameMovementHandle(app_p, hal_p);
    displayLives(app_p, hal_p);
    handleMeteors(app_p, hal_p);
    displayMeteors(app_p, hal_p);
    collisionHandle(app_p, hal_p);
    displayMissiles(app_p, hal_p);

    if(app_p->game.totalScore >= 15000){
                    app_p->threeSecondTimer = SWTimer_construct(250);
                }
    else if(app_p->game.totalScore >= 10000){
                app_p->threeSecondTimer = SWTimer_construct(750);
            }
    else if(app_p->game.totalScore >= 6000){
            app_p->threeSecondTimer = SWTimer_construct(1500);
        }
    else if(app_p->game.totalScore >= 2000){
        app_p->threeSecondTimer = SWTimer_construct(2500);
    }

    if (Button_isTapped(&hal_p->boosterpackS2))
    {
        firingHandle(app_p, hal_p);
    }
}

/**
 * Handles what is displayed when the gamestate transitions to gameover
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void screenGameOver(Application *app_p, HAL *hal_p)
{
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "GAME OVER", -1, 40, 40, true);

    char buffer[10];
    int myNum = app_p->game.totalScore; // number to print
    snprintf(buffer, 10, "%06d", myNum);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) buffer, -1, 50, 60, true);
}

/**
 * Clears the entire screen
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void clearDisplay(Application *app_p, HAL *hal_p)
{
    Graphics_Rectangle mainBox;
    mainBox.xMin = 0;
    mainBox.xMax = 128;
    mainBox.yMin = 0;
    mainBox.yMax = 128;
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(&hal_p->g_sContext, &mainBox);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Handles movestate of the player in game to tell which direction the spaceship
 * should move based on joystick position
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void moveStateNoD(Application *app_p, HAL *hal_p)
{
    getSampleJoyStick (&app_p->xJoyPos, &app_p->yJoyPos);
    if(app_p->xJoyPos < 4000){
        app_p->moveFarLeft = true;
    }
    else if(app_p->xJoyPos < 7000){
            app_p->moveLeft = true;
        }
    else {
        app_p->moveLeft = false;
        app_p->moveFarLeft = false;
    }
    if(app_p->xJoyPos > 13000){
            app_p->moveFarRight = true;
        }
    else if(app_p->xJoyPos > 10000){
                app_p->moveRight = true;
            }
    else {
        app_p->moveRight = false;
        app_p->moveFarRight = false;
    }
    if(app_p->yJoyPos > 13000){
        app_p->moveFarUp = true;
        }
    else if(app_p->yJoyPos > 10000){
        app_p->moveUp = true;
        }
    else {
        app_p->moveUp = false;
        app_p->moveFarUp = false;
    }
    if(app_p->yJoyPos < 4000){
        app_p->moveFarDown = true;
       }
    else if(app_p->yJoyPos < 7000){
        app_p->moveDown = true;
       }
    else {
        app_p->moveDown = false;
        app_p->moveFarDown = false;
    }

}

/**
 * Handles movement of the cursor to ensure that the cursor is debounced in
 * the main menu screen
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void moveState(Application *app_p, HAL *hal_p)
{
    getSampleJoyStick (&app_p->xJoyPos, &app_p->yJoyPos);
    if(app_p->xJoyPos < 7000){
        app_p->moveLeft = true;
    }
    else {
        app_p->moveLeft = false;
    }
    if(app_p->xJoyPos > 10000){
            app_p->moveRight = true;
        }
    else {
        app_p->moveRight = false;
    }
    if(app_p->yJoyPos > 10000){
        app_p->moveUp = true;
        }
    else if(app_p->yJoyPos < 10000 && app_p->moveUp){
        cursorHandle(app_p, hal_p, true);
        app_p->moveUp = false;
    }
    else {
        app_p->moveUp = false;
    }
    if(app_p->yJoyPos < 7000){
                    app_p->moveDown = true;
                }
    else if(app_p->yJoyPos > 7000 && app_p->moveDown){
            cursorHandle(app_p, hal_p, false);
            app_p->moveDown = false;
        }
    else {
        app_p->moveDown = false;
    }
}

/**
 * Handles the changing of the cursor index in the main menu screen for
 * option selection and debouncing
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void cursorHandle(Application *app_p, HAL *hal_p, bool upMove)
{
    getSampleJoyStick (&app_p->xJoyPos, &app_p->yJoyPos);
    if(upMove && app_p->cursorIndex == CPLAYGAME){
        app_p->cursorIndex = CHIGHSCORES;
    }
    else if(upMove && app_p->cursorIndex == CHOWTOPLAY){
        app_p->cursorIndex = CPLAYGAME;
    }
    else if(upMove && app_p->cursorIndex == CHIGHSCORES){
            app_p->cursorIndex = CHOWTOPLAY;
        }
    if(!upMove && app_p->cursorIndex == CPLAYGAME){
        app_p->cursorIndex = CHOWTOPLAY;
    }
    else if(!upMove && app_p->cursorIndex == CHOWTOPLAY){
        app_p->cursorIndex = CHIGHSCORES;
    }
    else if(!upMove && app_p->cursorIndex == CHIGHSCORES){
            app_p->cursorIndex = CPLAYGAME;
        }
}

/**
 * Handles movement of the spaceship based on the joystick position, changing position
 * of the player incrementally
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void gameMovementHandle(Application *app_p, HAL *hal_p)
{

    Graphics_Rectangle spaceShip;
    spaceShip.xMin = app_p->game.xPos-2;
    spaceShip.xMax = app_p->game.xPos+2;
    spaceShip.yMin = app_p->game.yPos-2;
    spaceShip.yMax = app_p->game.yPos+2;
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(&hal_p->g_sContext, &spaceShip);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);

    getSampleJoyStick (&app_p->xJoyPos, &app_p->yJoyPos);
    if(app_p->moveDown && app_p->game.yPos < 124){
        app_p->game.yPos++;
    }
    if(app_p->moveUp && app_p->game.yPos > 3){
            app_p->game.yPos--;
        }
    if(app_p->moveLeft && app_p->game.xPos > 63){
            app_p->game.xPos--;
        }
    if(app_p->moveRight && app_p->game.xPos < 124){
            app_p->game.xPos++;
        }
    if(app_p->moveFarDown && app_p->game.yPos < 124){
            app_p->game.yPos+=2;
        }
        if(app_p->moveFarUp && app_p->game.yPos > 3){
                app_p->game.yPos-=2;
            }
        if(app_p->moveFarLeft && app_p->game.xPos > 63){
                app_p->game.xPos-=2;
            }
        if(app_p->moveFarRight && app_p->game.xPos < 124){
                app_p->game.xPos+=2;
            }

    drawSpaceShip(app_p, hal_p);

}

/**
 * Draws the spaceship based on joystick location, changing color of ship depending
 * on number of lives
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void drawSpaceShip(Application *app_p, HAL *hal_p)
{
    if(app_p->game.numberLives == 2){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_YELLOW);
    }
    else if (app_p->game.numberLives == 1){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_ORANGE);
    }
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.xPos-1, app_p->game.yPos-1, app_p->game.xPos+1, app_p->game.yPos-1);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.xPos-2, app_p->game.yPos+1, app_p->game.xPos+2, app_p->game.yPos+1);

    Graphics_drawLine(&hal_p->g_sContext, app_p->game.xPos-2, app_p->game.yPos+2, app_p->game.xPos-2, app_p->game.yPos+2);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.xPos+2, app_p->game.yPos+2, app_p->game.xPos+2, app_p->game.yPos+2);

    Graphics_drawLine(&hal_p->g_sContext, app_p->game.xPos, app_p->game.yPos-2, app_p->game.xPos, app_p->game.yPos+2);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.xPos-1, app_p->game.yPos, app_p->game.xPos+1, app_p->game.yPos);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Handles the display of the player lives on the left of the game screen
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void displayLives(Application *app_p, HAL *hal_p)
{

    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "LIVES", -1, 2, 85, true);

    Graphics_Rectangle life1;
    life1.xMin = 15;
    life1.xMax = 20;
    life1.yMin = 100;
    life1.yMax = 105;

    Graphics_Rectangle life2;
    life2.xMin = 25;
    life2.xMax = 30;
    life2.yMin = 100;
    life2.yMax = 105;

    Graphics_Rectangle life3;
    life3.xMin = 35;
    life3.xMax = 40;
    life3.yMin = 100;
    life3.yMax = 105;

    if (app_p->game.numberLives == 3)
    {
        Graphics_fillRectangle(&hal_p->g_sContext, &life1);
        Graphics_fillRectangle(&hal_p->g_sContext, &life2);
        Graphics_fillRectangle(&hal_p->g_sContext, &life3);

    }
    else if (app_p->game.numberLives == 2)
    {
        Graphics_fillRectangle(&hal_p->g_sContext, &life1);
        Graphics_fillRectangle(&hal_p->g_sContext, &life2);

    }
    else if (app_p->game.numberLives == 1)
    {
        Graphics_fillRectangle(&hal_p->g_sContext, &life1);

    }
}

/**
 * Handles the creating of meteors randomly and the advancing of meteors
 * and missile over time
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void handleMeteors(Application *app_p, HAL *hal_p)
{
    getSampleJoyStick (&app_p->xJoyPos, &app_p->yJoyPos);
    char buffer[10];
    int random = rand() % 100;
    random = random / 2.0;
    random = random + 65;
    snprintf(buffer, 10, "%06d", random);

    if (SWTimer_expired(&app_p->threeSecondTimer)&& random > 63)
    {
        SWTimer_start(&app_p->threeSecondTimer);
        if ((app_p->game.meteor1.inPlay)==false){
            app_p->game.meteor1 = Meteor_init(random);
        }
        else if ((app_p->game.meteor2.inPlay)==false){
            app_p->game.meteor2 = Meteor_init(random);
        }
        else if ((app_p->game.meteor3.inPlay)==false){
            app_p->game.meteor3 = Meteor_init(random);
        }
    }

    if (SWTimer_expired(&app_p->game.advanceTimer)){
        SWTimer_start(&app_p->game.advanceTimer);

        clearMeteor1(app_p, hal_p);
        clearMeteor2(app_p, hal_p);
        clearMeteor3(app_p, hal_p);

        if ((app_p->game.meteor1.inPlay)){
            app_p->game.meteor1.yPos++;
            app_p->game.meteor1.collisionBottom++;
            app_p->game.meteor1.collisionTop++;}
        if ((app_p->game.meteor2.inPlay)){
            app_p->game.meteor2.yPos++;
            app_p->game.meteor2.collisionBottom++;
            app_p->game.meteor2.collisionTop++;}
        if ((app_p->game.meteor3.inPlay)){
            app_p->game.meteor3.yPos++;
            app_p->game.meteor3.collisionBottom++;
            app_p->game.meteor3.collisionTop++;}
        if ((app_p->game.missile1.inPlay)){
            app_p->game.missile1.yPo -= 2;
                }
        if ((app_p->game.missile2.inPlay)){
             app_p->game.missile2.yPo -= 2 ;
                }
    }
}

/**
 * Handles the display for meteors on screen depending on whether or not they are on display
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void displayMeteors(Application *app_p, HAL *hal_p)
{
    if(app_p->game.meteor1.inPlay == true){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_Rectangle meteorDisp1;
        meteorDisp1.xMin = app_p->game.meteor1.xPos -3;
        meteorDisp1.xMax = app_p->game.meteor1.xPos + 3;
        meteorDisp1.yMin = (app_p->game.meteor1.yPos-1) - 3;
        meteorDisp1.yMax = (app_p->game.meteor1.yPos-1) + 3;
        Graphics_fillRectangle(&hal_p->g_sContext, &meteorDisp1);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
        meteorDisp1.xMin = app_p->game.meteor1.xPos -3;
        meteorDisp1.xMax = app_p->game.meteor1.xPos + 3;
        meteorDisp1.yMin = app_p->game.meteor1.yPos - 3;
        meteorDisp1.yMax = app_p->game.meteor1.yPos + 3;
        Graphics_fillRectangle(&hal_p->g_sContext, &meteorDisp1);
    }
    if(app_p->game.meteor2.inPlay){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_Rectangle meteorDisp2;
        meteorDisp2.xMin = app_p->game.meteor2.xPos -3;
        meteorDisp2.xMax = app_p->game.meteor2.xPos + 3;
        meteorDisp2.yMin = (app_p->game.meteor2.yPos-1) - 3;
        meteorDisp2.yMax = (app_p->game.meteor2.yPos-1) + 3;
        Graphics_fillRectangle(&hal_p->g_sContext, &meteorDisp2);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
        meteorDisp2.xMin = app_p->game.meteor2.xPos -3;
        meteorDisp2.xMax = app_p->game.meteor2.xPos + 3;
        meteorDisp2.yMin = app_p->game.meteor2.yPos - 3;
        meteorDisp2.yMax = app_p->game.meteor2.yPos + 3;
        Graphics_fillRectangle(&hal_p->g_sContext, &meteorDisp2);
        }
    if(app_p->game.meteor3.inPlay){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_Rectangle meteorDisp3;
        meteorDisp3.xMin = app_p->game.meteor3.xPos -3;
        meteorDisp3.xMax = app_p->game.meteor3.xPos + 3;
        meteorDisp3.yMin = (app_p->game.meteor3.yPos-1) - 3;
        meteorDisp3.yMax = (app_p->game.meteor3.yPos-1) + 3;
        Graphics_fillRectangle(&hal_p->g_sContext, &meteorDisp3);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
        meteorDisp3.xMin = app_p->game.meteor3.xPos -3;
        meteorDisp3.xMax = app_p->game.meteor3.xPos + 3;
        meteorDisp3.yMin = app_p->game.meteor3.yPos - 3;
        meteorDisp3.yMax = app_p->game.meteor3.yPos + 3;
        Graphics_fillRectangle(&hal_p->g_sContext, &meteorDisp3);
        }
}

/**
 * Handles the collisions functions checking for Meteors colliding with
 * the spaceships, ground or missiles
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void collisionHandle(Application *app_p, HAL *hal_p)
{

    shipCollision(app_p, hal_p);
    groundCollision(app_p, hal_p);
    missileCollision(app_p, hal_p);

}

/**
 * Clears the display of Meteor 1
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void clearMeteor1(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_Rectangle meteorClear1;
    meteorClear1.xMin = app_p->game.meteor1.xPos -3;
    meteorClear1.xMax = app_p->game.meteor1.xPos + 3;
    meteorClear1.yMin = (app_p->game.meteor1.yPos) - 3;
    meteorClear1.yMax = (app_p->game.meteor1.yPos) + 3;
    Graphics_fillRectangle(&hal_p->g_sContext, &meteorClear1);

    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Clears the display of Meteor 2
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void clearMeteor2(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_Rectangle meteorClear1;
    meteorClear1.xMin = app_p->game.meteor2.xPos -3;
    meteorClear1.xMax = app_p->game.meteor2.xPos + 3;
    meteorClear1.yMin = (app_p->game.meteor2.yPos) - 3;
    meteorClear1.yMax = (app_p->game.meteor2.yPos) + 3;
    Graphics_fillRectangle(&hal_p->g_sContext, &meteorClear1);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Clears the display of Meteor 3
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void clearMeteor3(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_Rectangle meteorClear1;
    meteorClear1.xMin = app_p->game.meteor3.xPos -3;
    meteorClear1.xMax = app_p->game.meteor3.xPos + 3;
    meteorClear1.yMin = (app_p->game.meteor3.yPos) - 3;
    meteorClear1.yMax = (app_p->game.meteor3.yPos) + 3;
    Graphics_fillRectangle(&hal_p->g_sContext, &meteorClear1);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Clears the display of Missile 1
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void clearMissile1(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_Rectangle missileClrDisp1;
    missileClrDisp1.xMin = app_p->game.missile1.xPo -2;
    missileClrDisp1.xMax = app_p->game.missile1.xPo + 2;
    missileClrDisp1.yMin = (app_p->game.missile1.yPo+2) - 2;
    missileClrDisp1.yMax = (app_p->game.missile1.yPo+2) + 2;
    Graphics_fillRectangle(&hal_p->g_sContext, &missileClrDisp1);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Clears the display of Missile 2
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void clearMissile2(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_Rectangle missileClrDisp2;
    missileClrDisp2.xMin = app_p->game.missile2.xPo - 2;
    missileClrDisp2.xMax = app_p->game.missile2.xPo + 2;
    missileClrDisp2.yMin = (app_p->game.missile2.yPo+2) - 2;
    missileClrDisp2.yMax = (app_p->game.missile2.yPo+2) + 2;

    Graphics_fillRectangle(&hal_p->g_sContext, &missileClrDisp2);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Animates meteor1 by X ing it out
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void xMeteor1(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.meteor1.xPos -3, (app_p->game.meteor1.yPos) - 3, app_p->game.meteor1.xPos +3, (app_p->game.meteor1.yPos) + 3);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.meteor1.xPos -3, (app_p->game.meteor1.yPos) + 3, app_p->game.meteor1.xPos +3, (app_p->game.meteor1.yPos) - 3);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Animates meteor2 by X ing it out
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void xMeteor2(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.meteor2.xPos -3, (app_p->game.meteor2.yPos) - 3, app_p->game.meteor2.xPos +3, (app_p->game.meteor2.yPos) + 3);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.meteor2.xPos -3, (app_p->game.meteor2.yPos) + 3, app_p->game.meteor2.xPos +3, (app_p->game.meteor2.yPos) - 3);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Animates meteor3 by X ing it out
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void xMeteor3(Application *app_p, HAL *hal_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.meteor3.xPos -3, (app_p->game.meteor3.yPos) - 3, app_p->game.meteor3.xPos +3, (app_p->game.meteor3.yPos) + 3);
    Graphics_drawLine(&hal_p->g_sContext, app_p->game.meteor3.xPos -3, (app_p->game.meteor3.yPos) + 3, app_p->game.meteor3.xPos +3, (app_p->game.meteor3.yPos) - 3);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
}

/**
 * Checks for ship collisions with meteors and clears meteor while subtracting life
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void shipCollision(Application *app_p, HAL *hal_p)
{
    if(app_p->game.yPos >= app_p->game.meteor1.collisionTop &&
            app_p->game.yPos <= app_p->game.meteor1.collisionBottom &&
            app_p->game.xPos >= app_p->game.meteor1.collisionLeft &&
            app_p->game.xPos <= app_p->game.meteor1.collisionRight &&
            app_p->game.meteor1.inPlay){

        app_p->game.meteor1.inPlay = false;
        clearMeteor1(app_p, hal_p);
        subtractLife(app_p, hal_p);
        xMeteor1(app_p, hal_p);
    }
    if(app_p->game.yPos >= app_p->game.meteor2.collisionTop &&
            app_p->game.yPos <= app_p->game.meteor2.collisionBottom &&
            app_p->game.xPos >= app_p->game.meteor2.collisionLeft &&
            app_p->game.xPos <= app_p->game.meteor2.collisionRight &&
            app_p->game.meteor2.inPlay){

        app_p->game.meteor2.inPlay = false;
        clearMeteor2(app_p, hal_p);
        subtractLife(app_p, hal_p);
        xMeteor2(app_p, hal_p);
    }
    if(app_p->game.yPos >= app_p->game.meteor3.collisionTop &&
            app_p->game.yPos <= app_p->game.meteor3.collisionBottom &&
            app_p->game.xPos >= app_p->game.meteor3.collisionLeft &&
            app_p->game.xPos <= app_p->game.meteor3.collisionRight &&
            app_p->game.meteor3.inPlay){

        app_p->game.meteor2.inPlay = false;
        clearMeteor3(app_p, hal_p);
        subtractLife(app_p, hal_p);
        xMeteor3(app_p, hal_p);
    }
}

/**
 * Handles meteor collisions with ground by putting it out of play and clearing its display
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void groundCollision(Application *app_p, HAL *hal_p)
{
    if(app_p->game.meteor1.inPlay && app_p->game.meteor1.collisionBottom == 128){

        app_p->game.meteor1.inPlay = false;
        clearMeteor1(app_p, hal_p);
        subtractLife(app_p, hal_p);
        xMeteor1(app_p, hal_p);
    }
    if(app_p->game.meteor2.inPlay && app_p->game.meteor2.collisionBottom == 128){

        app_p->game.meteor2.inPlay = false;
        clearMeteor2(app_p, hal_p);
        subtractLife(app_p, hal_p);
        xMeteor2(app_p, hal_p);
    }
    if(app_p->game.meteor3.inPlay && app_p->game.meteor3.collisionBottom == 128){

        app_p->game.meteor3.inPlay = false;
        clearMeteor3(app_p, hal_p);
        subtractLife(app_p, hal_p);
        xMeteor3(app_p, hal_p);
    }

}

/**
 * Handles missile collision with the top of screen by clearing it
 * and putting it out of play
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void missileCollision(Application *app_p, HAL *hal_p)
{
    if(app_p->game.missile1.inPlay && app_p->game.missile1.yPo <= 3){
        app_p->game.missile1.inPlay = false;
        clearMissile1(app_p, hal_p);
    }
    if(app_p->game.missile2.inPlay && app_p->game.missile2.yPo <= 3){
        app_p->game.missile2.inPlay = false;
        clearMissile2(app_p, hal_p);
    }
    missile1MeteorCollisions(app_p, hal_p);
}

/**
 * Checks if Missile 1 has collided with any oft he meteors and clears both
 * missile and meteor
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void missile1MeteorCollisions(Application *app_p, HAL *hal_p)
{

    if(app_p->game.missile1.yPo >= app_p->game.meteor1.collisionTop &&
            app_p->game.missile1.yPo <= app_p->game.meteor1.collisionBottom &&
            app_p->game.missile1.xPo >= app_p->game.meteor1.collisionLeft &&
            app_p->game.missile1.xPo <= app_p->game.meteor1.collisionRight &&
            app_p->game.meteor1.inPlay && app_p->game.missile1.inPlay){

        app_p->game.meteor1.inPlay = false;
        app_p->game.missile1.inPlay = false;
        clearMeteor1(app_p, hal_p);
        clearMissile1(app_p, hal_p);
        app_p->game.totalScore+=SCORE_INCREMENT;
    }
    if(app_p->game.missile1.yPo >= app_p->game.meteor2.collisionTop &&
            app_p->game.missile1.yPo <= app_p->game.meteor2.collisionBottom &&
            app_p->game.missile1.xPo >= app_p->game.meteor2.collisionLeft &&
            app_p->game.missile1.xPo <= app_p->game.meteor2.collisionRight &&
            app_p->game.meteor2.inPlay  && app_p->game.missile1.inPlay){

        app_p->game.meteor2.inPlay = false;
        app_p->game.missile1.inPlay = false;
        clearMeteor2(app_p, hal_p);
        clearMissile1(app_p, hal_p);
        app_p->game.totalScore+=500;
    }
    if(app_p->game.missile1.yPo >= app_p->game.meteor3.collisionTop &&
            app_p->game.missile1.yPo <= app_p->game.meteor3.collisionBottom &&
            app_p->game.missile1.xPo >= app_p->game.meteor3.collisionLeft &&
            app_p->game.missile1.xPo <= app_p->game.meteor3.collisionRight &&
            app_p->game.meteor3.inPlay  && app_p->game.missile1.inPlay){

        app_p->game.meteor3.inPlay = false;
        app_p->game.missile1.inPlay = false;
        clearMeteor3(app_p, hal_p);
        clearMissile1(app_p, hal_p);
        app_p->game.totalScore+=500;
    }
}

/**
 * Would handle collisions for missile 2 if it worked
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void missile2MeteorCollisions(Application *app_p, HAL *hal_p)
{

    if(app_p->game.missile2.yPo >= app_p->game.meteor1.collisionTop &&
            app_p->game.missile2.yPo <= app_p->game.meteor1.collisionBottom &&
            app_p->game.missile2.xPo >= app_p->game.meteor1.collisionLeft &&
            app_p->game.missile2.xPo <= app_p->game.meteor1.collisionRight &&
            app_p->game.meteor1.inPlay && app_p->game.missile2.inPlay){

        app_p->game.meteor1.inPlay = false;
        app_p->game.missile2.inPlay = false;
        clearMeteor1(app_p, hal_p);
        clearMissile2(app_p, hal_p);
        xMeteor1(app_p, hal_p);
    }
    if(app_p->game.missile2.yPo >= app_p->game.meteor2.collisionTop &&
            app_p->game.missile2.yPo <= app_p->game.meteor2.collisionBottom &&
            app_p->game.missile2.xPo >= app_p->game.meteor2.collisionLeft &&
            app_p->game.missile2.xPo <= app_p->game.meteor2.collisionRight &&
            app_p->game.meteor2.inPlay  && app_p->game.missile2.inPlay){

        app_p->game.meteor2.inPlay = false;
        app_p->game.missile2.inPlay = false;
        clearMeteor2(app_p, hal_p);
        clearMissile2(app_p, hal_p);
        xMeteor1(app_p, hal_p);
    }
    if(app_p->game.missile2.yPo >= app_p->game.meteor3.collisionTop &&
            app_p->game.missile2.yPo <= app_p->game.meteor3.collisionBottom &&
            app_p->game.missile2.xPo >= app_p->game.meteor3.collisionLeft &&
            app_p->game.missile2.xPo <= app_p->game.meteor3.collisionRight &&
            app_p->game.meteor3.inPlay  && app_p->game.missile2.inPlay){

        app_p->game.meteor3.inPlay = false;
        app_p->game.missile2.inPlay = false;
        clearMeteor3(app_p, hal_p);
        clearMissile2(app_p, hal_p);
    }
}

/**
 * Subtracts life from player and changes the number of lives displayed
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void subtractLife(Application *app_p, HAL *hal_p)
{

    app_p->game.numberLives--;

    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_Rectangle lifeBox;
    lifeBox.xMin = 15;
    lifeBox.xMax = 40;
    lifeBox.yMin = 100;
    lifeBox.yMax = 105;
    Graphics_fillRectangle(&hal_p->g_sContext, &lifeBox);
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);


}


/**
 * Handles the creation of missiles given a button press
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void firingHandle(Application *app_p, HAL *hal_p)
{

    if(!(app_p->game.missile1.inPlay)){
        app_p->game.missile1.inPlay = true;
        app_p->game.missile1.xPo = app_p->game.xPos;
        app_p->game.missile1.yPo = (app_p->game.yPos - 5);
    }
}

/**
 * Displays the missiles in play given if they are in play or not
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void displayMissiles(Application *app_p, HAL *hal_p)
{

    if(app_p->game.missile1.inPlay){
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_Rectangle missileDisp1;
        missileDisp1.xMin = app_p->game.missile1.xPo -2;
        missileDisp1.xMax = app_p->game.missile1.xPo + 2;
        missileDisp1.yMin = (app_p->game.missile1.yPo+2) - 2;
        missileDisp1.yMax = (app_p->game.missile1.yPo+2) + 2;
        Graphics_fillRectangle(&hal_p->g_sContext, &missileDisp1);
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_WHITE);
        Graphics_drawLine(&hal_p->g_sContext, app_p->game.missile1.xPo, app_p->game.missile1.yPo-2, app_p->game.missile1.xPo, app_p->game.missile1.yPo+2);
        Graphics_drawLine(&hal_p->g_sContext, app_p->game.missile1.xPo+1, app_p->game.missile1.yPo-1, app_p->game.missile1.xPo+1, app_p->game.missile1.yPo+2);
        Graphics_drawLine(&hal_p->g_sContext, app_p->game.missile1.xPo-1, app_p->game.missile1.yPo-1, app_p->game.missile1.xPo-1, app_p->game.missile1.yPo+2);
        Graphics_drawLine(&hal_p->g_sContext, app_p->game.missile1.xPo-2, app_p->game.missile1.yPo+2, app_p->game.missile1.xPo+2, app_p->game.missile1.yPo+2);

    }

}

/**
 * Rearranges the order of the scores in the high scores page
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void modifyHighScores(Application *app_p, HAL *hal_p)
{
    int scoreHold = 0;
    if(app_p->game.totalScore >= app_p->firstScore){
        scoreHold = app_p->firstScore;
        app_p->firstScore = app_p->game.totalScore;
        app_p->thirdScore = app_p->secondScore;
        app_p->secondScore = scoreHold;
    }
    else if (app_p->game.totalScore >= app_p->secondScore){
        scoreHold = app_p->secondScore;
        app_p->secondScore = app_p->game.totalScore;
        app_p->thirdScore = scoreHold;
    }
    else if (app_p->game.totalScore >= app_p->secondScore){
        app_p->thirdScore = app_p->game.totalScore;
    }
    app_p->threeSecondTimer = SWTimer_construct(3000);

}


