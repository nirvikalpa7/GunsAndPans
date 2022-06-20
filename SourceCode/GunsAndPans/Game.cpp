
#include <iostream>
#include <fstream>
#include <filesystem>
#include <codecvt>

#include <windows.h>

#include "Game.h"

namespace GunsAndPuns
{

    //================================================================================================

    TGame::TGame()
        : winWidth{ 0U }, winHeight{ 0U }, lastTime{ 0U }, isDrawing{ false }
    {
        scores = startScores;

        const std::string texturesDir = "Textures\\";
        texturesNames.push_back(texturesDir + "ImageTarget1.bmp");
        texturesNames.push_back(texturesDir + "ImageTarget2.bmp");
        texturesNames.push_back(texturesDir + "ImageGround1.bmp");
        texturesNames.push_back(texturesDir + "ImageStart1.bmp");
        texturesNames.push_back(texturesDir + "ImageFinish1.bmp");
        texturesNames.push_back(texturesDir + "ImageGun1.bmp");
        texturesNames.push_back(texturesDir + "ImageTarget3.bmp");
        texturesNames.push_back(texturesDir + "ImageBullet1.bmp");

        const std::wstring soundsDir = L"Sounds\\";
        soundsNames.push_back(soundsDir + L"SoundStart.wav");
        soundsNames.push_back(soundsDir + L"SoundPlay.wav");
        soundsNames.push_back(soundsDir + L"SoundShoot.wav");
        soundsNames.push_back(soundsDir + L"SoundTarget.wav");
        soundsNames.push_back(soundsDir + L"SoundFinish.wav");
    }

    TGame::~TGame()
    {
    }

    std::string __fastcall TGame::wstringToString(const std::wstring& wstr) const
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;
        return converterX.to_bytes(wstr);
    }

    void TGame::checkResources() const
    {
        namespace fs = std::filesystem;

        // првоерка необходимых ресурсов перед запуском игры
        for (const auto& filename : texturesNames)
        {
            if (!fs::exists(filename))
            {
                std::cerr << "Can not find resouce: " << filename << " in " << __FUNCTION__ << std::endl;
                exit(1);
            }
        }

        for (const auto& filename : soundsNames)
        {
            if (!fs::exists(filename))
            {
                std::cerr << "Can not find resouce: " << wstringToString(filename) << " in " << __FUNCTION__ << std::endl;
                exit(1);
            }
        }
    }

    void __fastcall TGame::kbHit(const uint8_t key)
    {
        static const uint8_t YES_KEY = 'y';
        static const uint8_t YES_CAPS_KEY = 'Y';

        switch (key)
        {
        case ESC_KEY:
            if (state == TGameState::PLAY)
            {
                state = TGameState::FINISH;
                playSound(TSoundId::FINISH);
                saveScore();
                display();
            }
            else if (state == TGameState::FINISH)
            {
                exit(0);
            }
            break;

        case ENTER_KEY:
            if (state == TGameState::START)
            {
                state = TGameState::PLAY;
                playSound(TSoundId::PLAY);
                display();
            }
            else if (state == TGameState::PLAY)
            {
                shoot();
            }
            break;

        case YES_KEY:
        case YES_CAPS_KEY:
            if (state == TGameState::FINISH)
            {
                state = TGameState::PLAY;
                playSound(TSoundId::PLAY);
                reInit();
                display();
            }
            break;

        default:
            break;
        }
    }

    void __fastcall TGame::playSound(const TSoundId id)
    {
        if (soundsNames.size() != soundsNumber)
        {
            std::cerr << "Expected " << soundsNumber << " sounds in the array. In " << __FUNCTION__ << std::endl;
            return;
        }

        const DWORD fdwSound = SND_FILENAME | SND_ASYNC;

        switch (id)
        {
        case TSoundId::START:
            PlaySound(soundsNames[0].c_str(), NULL, fdwSound);
            break;

        case TSoundId::PLAY:
            PlaySound(soundsNames[1].c_str(), NULL, fdwSound);
            break;

        case TSoundId::SHOOT:
            PlaySound(soundsNames[2].c_str(), NULL, fdwSound);
            break;

        case TSoundId::TARGET:
            PlaySound(soundsNames[3].c_str(), NULL, fdwSound);
            break;

        case TSoundId::FINISH:
            PlaySound(soundsNames[4].c_str(), NULL, fdwSound);
            break;

        default:
            std::cerr << "Incorrect sound ID in " << __FUNCTION__ << std::endl;
        }
    }

    void TGame::reInit()
    {
        appleTarget.setPoints(30);
        appleTarget.init(0.0f, 4.9f, 1.5f, 0.5f);

        smallTarget.setPoints(25);
        smallTarget.init(0.0f, 2.3f, -1.0f, 0.75f);

        bigTarget.setPoints(20);
        bigTarget.init(0.0f, -0.2f, 1.0f, 1.0f);

        bullet.resetCenter();

        scores = startScores;
    }

    void TGame::init()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);

        // Let's init internal objects
        reInit();

        scene.genTexture(TImage::TGeneratedImg::CHESS);

        if (texturesNames.size() != textureNumber)
        {
            std::cerr << "Expected " << textureNumber << " textures in the array. In " << __FUNCTION__ << std::endl;
            return;
        }
        bullet.loadTexture(texturesNames[7]);
        appleTarget.loadTexture(texturesNames[6]);
        smallTarget.loadTexture(texturesNames[0]);
        bigTarget.loadTexture(texturesNames[1]);
        ground.loadTexture(texturesNames[2]);
        startScr.loadTexture(texturesNames[3]);
        finishScr.loadTexture(texturesNames[4]);
        gun.loadTexture(texturesNames[5]);

        lastTime = GetTickCount64();
    }

    void TGame::drawPlayScreen() const
    {
        ground.draw();
        scene.draw();
        gun.draw();
        bullet.draw();
        if (appleTarget.active)
        {
            appleTarget.draw();
        }
        if (smallTarget.active)
        {
            smallTarget.draw();
        }
        if (bigTarget.active)
        {
            bigTarget.draw();
        }
    }

    void TGame::display()
    {
        isDrawing = true;
        dt = GetTickCount64() - lastTime;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (state)
        {
        case TGame::TGameState::START:
            startScr.draw();
            break;

        case TGame::TGameState::PLAY:
            move();
            drawPlayScreen();
            break;

        case TGame::TGameState::FINISH:
            finishScr.draw();
            break;

        default:
            std::cerr << "Incorrect game state! In " << __FUNCTION__ << std::endl;
            break;
        }

        glutSwapBuffers();
        lastTime = GetTickCount64();
        isDrawing = false;
    }

    void __fastcall TGame::resize(const GLsizei w, const GLsizei h)
    {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, 1.0 * (GLfloat)w / (GLfloat)h, 1.0, 30.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0, 0.0, -5);

        winWidth = w;
        winHeight = h;
    }

    void TGame::onTimer()
    {
        if (!isDrawing)
        {
            display();
        }
    }

    void TGame::move()
    {
        if (state == TGame::TGameState::PLAY)
        {
            if (bigTarget.active)
            {
                bigTarget.move(dt);
            }
            if (smallTarget.active)
            {
                smallTarget.move(dt);
            }
            if (appleTarget.active)
            {
                appleTarget.move(dt);
            }
            if (bullet.active)
            {
                bullet.move(dt);
                collisionCheck();
            }
        }
    }

    void TGame::shoot()
    {
        if (state == TGame::TGameState::PLAY)
        {
            if (!bullet.active)
            {
                if (scores <= 0U)
                {
                    state = TGame::TGameState::FINISH;
                    playSound(TSoundId::FINISH);
                    saveScore();
                    display();
                    return;
                }
                scores -= bulletCost;
                playSound(TSoundId::SHOOT);
                bullet.resetCenter();
                const auto XZAngle = gun.getXZAngleDegree();
                const auto YZAngle = gun.getYZAngleDegree();
                bullet.calcVector(XZAngle, YZAngle);
                bullet.active = true;
            }
        }
    }

    void TGame::collisionCheck()
    {
        if (state == TGame::TGameState::PLAY)
        {
            if (bullet.active)
            {
                // Crossed scene with targets?
                if (fabs(scene.getZ() - bullet.getCZ()) <= bullet.getRadius() * 1.5f)
                {
                    const GLfloat x = bullet.getCX();
                    const GLfloat y = bullet.getCY();
                    if (smallTarget.active && smallTarget.isInside(x, y))
                    {
                        playSound(TSoundId::TARGET);
                        smallTarget.active = false;
                        bullet.active = false;
                        bullet.resetCenter();
                        scores += smallTarget.getPoints();
                    }
                    else if (bigTarget.active && bigTarget.isInside(x, y))
                    {
                        playSound(TSoundId::TARGET);
                        bigTarget.active = false;
                        bullet.active = false;
                        bullet.resetCenter();
                        scores += bigTarget.getPoints();
                    }
                    else if (appleTarget.active && appleTarget.isInside(x, y))
                    {
                        playSound(TSoundId::TARGET);
                        appleTarget.active = false;
                        bullet.active = false;
                        bullet.resetCenter();
                        scores += appleTarget.getPoints();
                    }

                    if (!bigTarget.active && !smallTarget.active && !appleTarget.active) // Finish game?
                    {
                        kbHit(GunsAndPuns::TGame::ESC_KEY);
                    }
                }
            }
        }
    }

    void TGame::saveScore()
    {
        std::ofstream fout;
        fout.open(scoreFileName);
        if (fout.is_open())
        {
            const size_t maxScores = startScores - (bulletCost << 1)
                + bigTarget.getPoints() + smallTarget.getPoints() + appleTarget.getPoints();

            fout << "Guns & Pans Game Results" << std::endl;
            fout << "Max possible scores: " << maxScores << std::endl;
            fout << "Your current scores: " << scores << std::endl;
            fout << "Bullet cost: " << bulletCost << std::endl;
        }
    }

}; // namespace GunsAndPuns