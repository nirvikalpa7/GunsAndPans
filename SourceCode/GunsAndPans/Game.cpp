
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
        texturesNames.push_back(texturesDir + "ImageTarget4.bmp");
        texturesNames.push_back(texturesDir + "ImageTarget5.bmp");
        texturesNames.push_back(texturesDir + "ImageTarget6.bmp");

        std::string fn;
        const size_t amunTexturesNumber{ 10U };
        for (size_t i = 0; i <= amunTexturesNumber; i++)
        {
            fn = texturesDir + "ImageAmmunitions" + ((i == amunTexturesNumber) ? "" : "0") 
                + std::to_string(i) + ".bmp";
            texturesNames.push_back(fn);
        }

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

        // checking resourses - textures and sound files
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

    void __fastcall TGame::kbHit(const TGame::TKeyCode key)
    {
        switch (key)
        {
        case TKeyCode::ESC_KEY:
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

        case TKeyCode::ENTER_KEY:
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

        case TKeyCode::YES_KEY:
        case TKeyCode::YES_CAPS_KEY:
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
        level.reInit();

        bullet.resetCenter();

        scores = startScores;

        amun.setAmunNumber(startScores / bulletCost);
        amun.initCurTexture();
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
        scene.genTexture(TImage::TGeneratedImg::CHESS);

        if (texturesNames.size() != textureNumber)
        {
            std::cerr << "Expected " << textureNumber << " textures in the array. In " << __FUNCTION__ << std::endl;
            return;
        }
        bullet.loadTexture(texturesNames[7]);
        ground.loadTexture(texturesNames[2]);
        startScr.loadTexture(texturesNames[3]);
        finishScr.loadTexture(texturesNames[4]);
        gun.loadTexture(texturesNames[5]);

        level.init(texturesNames);

        const size_t startAmunIndex{ 11U };
        const size_t finishAmunIndex{ 21U };
        for (size_t i = startAmunIndex; i <= finishAmunIndex; i++)
        {
            amun.addFileAsTexture(texturesNames[i]);
        }
        amun.setParams(0.6f, 0.3f, -1.1f, -0.9f, 3.0f);
        
        reInit();

        lastTime = GetTickCount64();
    }

    void TGame::drawPlayScreen() const
    {
        ground.draw();
        scene.draw();
        gun.draw();
        amun.draw();
        if (bullet.active)
        {
            bullet.draw();
        }
        
        level.draw();
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
        GLfloat startZCoord{ - 5.0f };

        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, 1.0 * (GLfloat)w / (GLfloat)h, 1.0, 30.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, startZCoord);

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
            level.move(dt);
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
                if (amun.getAmunNumber() == 0U)
                {
                    state = TGame::TGameState::FINISH;
                    playSound(TSoundId::FINISH);
                    saveScore();
                    display();
                    return;
                }
                scores -= bulletCost;
                amun.decrementNumber();
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
                // Have bullet crossed scene with targets?
                if (fabs(scene.getZ() - bullet.getCZ()) <= bullet.getRadius() * 1.5f)
                {
                    const GLfloat x = bullet.getCX();
                    const GLfloat y = bullet.getCY();

                    if (level.collisionCheck(x, y, scores, bullet)) // Target get into target?
                    {
                        playSound(TSoundId::TARGET);
                    }

                    if (!level.isTargetsActive()) // All targets are destroyed?
                    {
                        if (level.getLevel() == TLevelTargets::TLevelNum::FIRST) // Next level?
                        {
                            level.nextLevel();
                        }
                        else if (level.getLevel() == TLevelTargets::TLevelNum::SECOND) // Finish game?
                        {
                            kbHit(TKeyCode::ESC_KEY);
                        }
                    }
                }
            }
        }
    }

    void TGame::saveScore() const
    {
        std::ofstream fout;
        fout.open(scoreFileName);
        if (fout.is_open())
        {
            const size_t maxScores = startScores + level.getAllTargetsPoints() - (bulletCost * 6);

            fout << "Guns & Pans Game Results" << std::endl;
            fout << "Max possible scores: " << maxScores << std::endl;
            fout << "Your current scores: " << scores << std::endl;
            fout << "Bullet cost: " << bulletCost << std::endl;
        }
    }

}; // namespace GunsAndPuns