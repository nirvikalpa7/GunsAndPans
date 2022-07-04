//================================================================================================
// Guns & Pans - 2022 / version 1.6.0 / Sidelnikov Dmitry (c)
// Used: C++20, OpenGL, GLU, GLUT, Visual Studio 2022
// Contacts: +79050209986 (WhatsApp), Telegram: @SpringDayInTheForest, E-Mail: nirvikalpa@yandex.ru
//================================================================================================
// Binary files can be found in Release section on Git Hub
//================================================================================================
// I tried to use the best practices for writing code, however, the code is far from perfect: 
// in the code you can find magic numbers, a wide variety of data types and STL and portable 
// integer types, and old C types and OpenGL and Windows types, raw pointers and arrays are 
// also found. The game does not use any cool lighting, shadows or shaders. All this can be 
// added to the second version of the game, OpenGL itself supports it. The system of checking 
// the correctness of parameters and states is also probably not ideal. But the code is commented, 
// its namespace is used in the game, the code is divided into translation units, OOP and 
// polymorphism are used. I also tried to keep the KISS principle in mind when developing. 
// Mathematical operations were also not heavily optimized in this game, since the performance 
// problem is not acute.
//================================================================================================

#include <memory>

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "TImage.h"
#include "TObjects.h"
#include "TGame.h"

//================================================================================================

std::unique_ptr<GunsAndPuns::TGame> pGame;

const size_t mainWinWidth{ 800U };
const size_t mainWinHeight{ 600U };

const size_t targetFPS{ 40U };
const size_t timerSpeedMs{ 1000U / targetFPS };

const std::string gameVersion{ "1.6.0" };
const std::string authors{ "Dmitry Sidelnikov" };
const std::string winTitle{ "Guns & Pans - 2022 / version " + gameVersion + " / " + authors };

//================================================================================================
// Callback functions for GLUT library

void display()
{
    if (pGame)
    {
        pGame->display();
    }
}

void myReshape(const int w, const int h)
{
    if (pGame)
    {
        pGame->resize(w, h);
    }
}

void keyboard(const uint8_t key, const int x, const int y)
{
    if (pGame)
    {
        pGame->kbHit(static_cast<GunsAndPuns::TGame::TKeyCode>(key));
    }
}

void timer(const int value)
{
    if (pGame)
    {
        glutTimerFunc(timerSpeedMs, timer, 0);
        pGame->onTimer();
    }
};

void mouseClick(const int button, const int state, const int x, const int y)
{
    if (!pGame)
    {
        return;
    }

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
            if (pGame->isPlaying())
            {
                pGame->shoot();
            }
            else if (pGame->isStarting())
            {
                pGame->kbHit(GunsAndPuns::TGame::TKeyCode::ENTER_KEY);
            }
        }
        break;

    default:
        break;
    };
}

void mouseMove(const int x, const int y)
{
    if (pGame && pGame->isPlaying())
    {
        const GLfloat wHalf = static_cast<GLfloat>(pGame->getWinWidth() >> 1);
        const GLfloat horAngle = (x - wHalf) / wHalf;
        pGame->setXZAngle(horAngle); // -1 .. 1 percentage

        const GLfloat h = static_cast<GLfloat>(pGame->getWinHeight());
        const GLfloat verAngle = (h - y) / h; // 0 .. 1 percentage
        pGame->setYZAngle(verAngle);

        pGame->display();
    }
}

//================================================================================================

int main(int argc, char** argv)
{
    pGame = std::make_unique<GunsAndPuns::TGame>();
    pGame->checkResources(); // check textures and sounds we need in the game
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow(winTitle.c_str());

    pGame->init();
  
    glutReshapeWindow(mainWinWidth, mainWinHeight);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMove);
    glutTimerFunc(timerSpeedMs, timer, 0);

    pGame->playSound(GunsAndPuns::TGame::TSoundId::START);

    glutMainLoop();
    return 0;
}