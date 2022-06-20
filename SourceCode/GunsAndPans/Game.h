#pragma once

#include <vector>

#include "Objects.h"

namespace GunsAndPuns
{

    //================================================================================================

    class TGame
    {
    public:

        TGame();
        ~TGame();

        enum class TSoundId
        {
            START,
            PLAY,
            SHOOT,
            TARGET,
            FINISH
        };

        enum class TGameState
        {
            START,
            PLAY,
            FINISH
        };

        void checkResources() const;
        void init();
        void display();
        void __fastcall resize(const GLsizei w, const GLsizei h);
        void __fastcall playSound(const TSoundId id);
        void __fastcall kbHit(const uint8_t key);

        void __fastcall setXZAngle(GLfloat angle) { gun.setXZAngle(angle); };
        void __fastcall setYZAngle(GLfloat angle) { gun.setYZAngle(angle); };

        size_t getWinWidth() const { return winWidth; }
        size_t getWinHeight() const { return winHeight; }

        bool isPlaying() const { return (state == TGameState::PLAY); }

    private:

        void drawPlayScreen() const;
        void reInit();

        // string type converter
        std::string __fastcall wstringToString(const std::wstring& wstr) const;

        TGameState state{ TGameState::START };

        std::vector<std::string> texturesNames;
        const size_t textureNumber{ 6U };

        std::vector<std::wstring> soundsNames;
        const size_t soundsNumber{ 5U };

        TGun gun;
        TScene scene;
        TGround ground;
        TBullet bullet;
        TTarget smallTarget;
        TTarget bigTarget;

        TScreen startScr;
        TScreen finishScr;

        size_t winWidth, winHeight, score;
        const size_t bulletCost{ 5U };
    };

}; // namespace GunsAndPuns

//================================================================================================
