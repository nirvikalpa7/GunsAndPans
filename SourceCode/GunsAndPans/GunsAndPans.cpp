//================================================================================================
// Guns & Pans - 2022 / version 1.0 / Sidelnikov Dmitry (c)
// Used: C++20, OpenGL, GLU, GLUT, Visual Studio 2022
// For contact: +7 905 020 99 86 (WhatsApp), Telegram: @SpringDayInTheForest
//================================================================================================
// Я старался использовать самые лучшие практики по написанию кода тем неменее код далек от 
// идеального: в коде можно встретить магические числа, большое разнообразие типов данных
// и STL и портабельные целые типы, и старые сишные типы и типы OpenGl и Windows, также 
// встречаются сырые указатели и массивы. В игре не используется какое либо крутое освещение,
// тени или шейдеры. Это все можно добавить во вторую версию игры, сам OpenGL это поддерживает.
// Система проверок корректности параметров и состояний тоже наверное не идеальна. Но
// код коментирован, использован свой неймспейс в игре, код разбит на единицы трансяции. 
// Также я старался при разработке держать в уме принцип KISS. Математические операции также
// не были как то сильно оптимизированны в этой игре вопрос производительности остро не стоит.
//================================================================================================

#include <memory>

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Image.h"
#include "Objects.h"
#include "Game.h"

//================================================================================================

std::unique_ptr<GunsAndPuns::TGame> pGame;

const size_t mainWinWidth{ 800U };
const size_t mainWinHeight{ 600U };

const size_t targetFPS{ 40U };
const size_t timerSpeedMs{ 1000 / targetFPS };

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
        pGame->setXZAngle(horAngle);

        const GLfloat h = static_cast<GLfloat>(pGame->getWinHeight());
        const GLfloat verAngle = (h - y) / h;
        pGame->setYZAngle(verAngle);

        pGame->display();
    }
}

//================================================================================================

int main(int argc, char** argv)
{
    pGame = std::make_unique<GunsAndPuns::TGame>();
    pGame->checkResources(); // проверим необходимые ресурсы
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Guns & Pans - 2022 / version 1.0 / Sidelnikov Dmitry");

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