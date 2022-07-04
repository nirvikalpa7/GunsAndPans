#pragma once

#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Image.h"

namespace GunsAndPuns
{
    // Static object in the game
    class TObject {
    public:

        virtual void draw() const = 0;

        void __fastcall loadTexture(const std::string& filename)
        {
            if (!image.loadImage(filename)) {
                exit(1);
            }
            image.genTexture();
        }

        void __fastcall genTexture(const TImage::TGeneratedImg id)
        {
            image.makeImage(id);
            image.genTexture();
        }

    protected:

        TImage image;
    };

    // Dynamic object in the game
    class TDynamicObject : public TObject {
    public:

        virtual void __fastcall move(const size_t dtMs) = 0;
    };

    //================================================================================================

    // Ground in the game
    class TGround : public TObject {
    public:

        TGround();
        void draw() const override;

        GLfloat getY() const { return y; }
        GLfloat getFarZ() const { return z2; }

    private:

        GLfloat y;
        GLfloat x1, z1;
        GLfloat x2, z2;
        GLfloat x3, z3;
        GLfloat x4, z4;
    };

    //================================================================================================

    // Gun bullet in the game
    class TBullet : public TDynamicObject {
    public:

        TBullet();
        ~TBullet();

        void draw() const override;
        void __fastcall move(const size_t dtMs) override;

        GLfloat getCX() const { return cx; }
        GLfloat getCY() const { return cy; }
        GLfloat getCZ() const { return cz; }
        GLfloat getRadius() const { return radius; }

        void resetCenter();
        void calcVector(const GLfloat xzAngleDegree, const GLfloat yzAngleDegree);

        bool active;

    private:

        void rotate(const GLfloat angle, const GLfloat oldX, const GLfloat oldY, GLfloat& newX, GLfloat& newY);

        const GLfloat radius, speed, maxZ;
        GLfloat cx, cy, cz; // center
        GLfloat vx, vy, vz; // movement vector 
        GLUquadric* obj;    // bullet
        const double ratio;
    };

    //================================================================================================

    // A scene with targets in the game
    class TScene : public TObject {
    public:

        TScene();
        ~TScene();
        void draw() const override;

        GLfloat getZ() const { return z; }

    private:

        GLfloat z, height, width, logRadius, topY, downY;

        GLUquadric* objTop;
        GLUquadric* objDown;
        GLUquadric* objLeft;
        GLUquadric* objRight;
    };

    //================================================================================================

    // A target in the game
    class TTarget : public TDynamicObject
    {
    public:

        TTarget();

        void draw() const override;
        void __fastcall move(const size_t dtMs) override;

        bool __fastcall isInside(const GLfloat x, const GLfloat y) const;

        void __fastcall setPoints(size_t p) { points = p; }
        size_t getPoints() const { return points; }

        void __fastcall init(const GLfloat _cx, const GLfloat _cy, 
            const GLfloat _vx, const GLfloat _vy, const GLfloat _size, const bool _horizontalTarget)
        {
            cx = _cx;
            cy = _cy;
            vx = _vx;
            vy = _vy;
            size = _size;
            active = true;
            horizontalTarget = _horizontalTarget;
        }

        bool active;

    private:

        const GLfloat speed{ 0.001f };
        const GLfloat sceneWidth{ 10.0f };
        const GLfloat yTop{ 4.0f };
        const GLfloat yDown{ -2.0f };
        GLfloat z, cx, cy, vx, vy, size /* half width */;
        size_t points;
        bool horizontalTarget;
    };

    //================================================================================================

    // A gun in the game
    class TGun : public TObject
    {
    public:

        void draw() const override;

        void __fastcall setXZAngle(GLfloat angle) { xzAngle = angle; };
        void __fastcall setYZAngle(GLfloat angle) { yzAngle = angle; };

        GLfloat getXZAngleDegree() const { return xzAngle * gunAngleWidth; }
        GLfloat getYZAngleDegree() const { return yzAngle * gunAngleWidth; }

        TGun();
        ~TGun();

    private:

        GLfloat length;
        GLfloat xzAngle; // -1..1
        GLfloat yzAngle; // 0..1
        GLfloat gunAngleWidth; // rotation view angle in degrees
        GLUquadric* objBarrel;
        GLUquadric* objBase;
    };

    //================================================================================================

    // Texture screen in the game
    class TScreen : public TObject
    {
    public:

        void draw() const override;

        void __fastcall setParams(const GLfloat _z, const GLfloat _width, const GLfloat _height)
        {
            z = _z;
            width = _width;
            height = _height;
        }

    private:

        GLfloat z{ 0.0f };
        GLfloat width{ 6.0f };
        GLfloat height{ 5.0f };
    };

    //================================================================================================

    // Gun amunitions screen in the game
    class TGunAmunitions
    {
    public:

        void __fastcall addFileAsTexture(const std::string& fname);
        void __fastcall setAmunNumber(const size_t num) { currentAmunitionsNumber = num; }
        
        size_t getAmunNumber() const { return currentAmunitionsNumber; }

        void initCurTexture()
        {
            curImage.freeMem();
            curImage.loadImage(fileNames[currentAmunitionsNumber]);
            curImage.genTexture();
        }
        
        void decrementNumber() 
        { 
            currentAmunitionsNumber--;
            initCurTexture();
        }
        
        void setParams(const GLfloat _width, const GLfloat _height, 
            const GLfloat _cx, const GLfloat _cy, const GLfloat _cz)
        {
            height = _height;
            width = _width;
            cx = _cx;
            cy = _cy;
            cz = _cz;
        }
   
        void draw() const;

    private:

        std::vector<std::string> fileNames;

        TImage curImage;
        size_t currentAmunitionsNumber{ 10U };

        GLfloat width, height, cx, cy, cz;
    };

    //================================================================================================

    // Set of targets for several levels
    class TLevels : public TDynamicObject
    {
    public:

        enum class TLevelNum 
        {
            FIRST,
            SECOND,
            FINISH
        };

        void init(const std::vector<std::string> & textures);
        void reInit();
        bool collisionCheck(const GLfloat x, const GLfloat y, size_t& scores, TBullet& bullet);
        
        void draw() const override;
        void __fastcall move(const size_t dtMs) override;

        TLevelNum getLevel() const { return level; }
        GLfloat getSceneZ() const { return scene.getZ(); }
        size_t getAllTargetsPoints() const;

        bool isTargetsActive() const;
        void nextLevel();

    private:

        TGround ground;
        TScene scene;

        // Level 1
        TTarget appleTarget;
        TTarget smallTarget;
        TTarget bigTarget;

        // Level 2
        TTarget leftTarget;
        TTarget rightTarget;
        TTarget lemonTarget;

        TLevelNum level{ TLevelNum::FIRST };
    };


}; // namespace GunsAndPuns