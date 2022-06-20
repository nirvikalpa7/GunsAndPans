#pragma once

#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Image.h"

namespace GunsAndPuns
{
    // статический объект в игре
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

    // Динамический объект в игре
    class TDynamicObject : public TObject {
    public:

        virtual void __fastcall move(const size_t dtMs) = 0;
    };

    //================================================================================================

    // Земля в игре
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

    // Снаряд в игре
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
        GLfloat cx, cy, cz; // координата центра
        GLfloat vx, vy, vz; // вектор движения
        GLUquadric* obj; // снаряд
        const double ratio;
    };

    //================================================================================================

    // Сцена с мишенями в игре
    class TScene : public TObject {
    public:

        TScene();
        ~TScene();
        void draw() const override;

        GLfloat getZ() const { return z; }

    private:

        GLfloat z, height, width, radius, topY, downY;

        GLUquadric* objTop;
        GLUquadric* objDown;
        GLUquadric* objLeft;
        GLUquadric* objRight;
    };

    //================================================================================================

    // Мишень
    class TTarget : public TDynamicObject
    {
    public:

        TTarget();

        void draw() const override;
        void __fastcall move(const size_t dtMs) override;

        bool __fastcall isInside(const GLfloat x, const GLfloat y) const;

        void __fastcall setPoints(size_t p) { points = p; }
        size_t getPoints() const { return points; }

        void __fastcall init(const GLfloat _cx, const GLfloat _cy, const GLfloat _vx, const GLfloat _size)
        {
            cx = _cx;
            cy = _cy;
            vx = _vx;
            size = _size;
            active = true;
        }

        bool active;

    private:

        const GLfloat speed{ 0.001f };
        const GLfloat sceneWidth{ 10.0f };
        GLfloat z, cx, cy, vx, size /* половина ширины */;
        size_t points;
    };

    //================================================================================================

    // Пушка в игре
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
        GLfloat gunAngleWidth; // угол поворота пушки в градусах
        GLUquadric* objBarrel;
        GLUquadric* objBase;
    };

    //================================================================================================

    // Экраны с текстурой для игры
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

}; // namespace GunsAndPuns