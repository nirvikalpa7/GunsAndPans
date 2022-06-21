
#include <cmath>
#include <numbers>

#include <windows.h>

#include "Objects.h"

namespace GunsAndPuns
{

    //================================================================================================
    // class TGround

    TGround::TGround()
        : y{ -3.0f }
        , x1{ 9.0f }, z1{ 4.0f }
        , x2{ 9.0f }, z2{ -9.0f }
        , x3{ -9.0f }, z3{ -9.0f }
        , x4{ -9.0f }, z4{ 4.0f }
    {
    }

    void TGround::draw() const
    {
        glBindTexture(GL_TEXTURE_2D, image.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x1, y, z1);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(x2, y, z2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(x3, y, z3);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(x4, y, z4);
        glEnd();
    }

    //================================================================================================
    // class TBullet

    TBullet::TBullet()
        : radius{ 0.1f }, speed{ 0.008f }, maxZ{ -10.0f }
        , cx{ 0.0f }, cy{ 0.0f }, cz{ 0.0f }
        , vx{ 0.0f }, vy{ 0.0f }, vz{ 0.0f }
        , active{ false }, ratio{ std::numbers::pi_v<double> / 180.0 }
    {
        obj = gluNewQuadric();
        resetCenter();
    }

    TBullet::~TBullet()
    {
        gluDeleteQuadric(obj);
    }

    void TBullet::resetCenter()
    {
        // start point
        cx = 0.0f;
        cy = -1.0f;
        cz = 3.0f;
    }

    void TBullet::calcVector(const GLfloat xzAngleDegree, const GLfloat yzAngleDegree)
    {
        vx = 0.0f;
        vy = 0.0f;
        vz = -1.0f;

        rotate(yzAngleDegree, vy, vz, vy, vz);
        rotate(xzAngleDegree, vx, vz, vx, vz);
    }

    void TBullet::draw() const
    {
        if (active)
        {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, image.texture);
            glTranslatef(cx, cy, cz);
            glRotatef(33.0f, 1.0f, 1.0f, 0.0f); // move texture
            gluQuadricTexture(obj, GL_TRUE);
            gluSphere(obj, radius, 20, 20);
            glPopMatrix();
        }
    }

    void __fastcall TBullet::move(const size_t dtMs)
    {
        if (active)
        {
            const GLfloat distance = dtMs * speed;
            cx += vx * distance;
            cy += vy * distance;
            cz += vz * distance;

            if (cz < maxZ)
            {
                active = false;
                resetCenter();
            }
        }
    }

    void TBullet::rotate(const GLfloat angleDegree,
        const GLfloat oldX, const GLfloat oldY,
        GLfloat& newX, GLfloat& newY)
    {
        const double angleRad = angleDegree * ratio;
        const double sinVal = sin(angleRad);
        const double cosVal = cos(angleRad);
        newX = static_cast<GLfloat>(oldX * cosVal - oldY * sinVal);
        newY = static_cast<GLfloat>(oldX * sinVal + oldY * cosVal);
    }

    //================================================================================================
    // class TScene

    TScene::TScene()
        : logRadius{ 0.25f }, width{ 10.0f }, height{ 7.0f }
        , z{ -5.0f }, topY{ 4.0f }, downY{ -2.0f }
    {
        objTop = gluNewQuadric();
        objDown = gluNewQuadric();
        objLeft = gluNewQuadric();
        objRight = gluNewQuadric();
    }

    TScene::~TScene()
    {
        gluDeleteQuadric(objTop);
        gluDeleteQuadric(objDown);
        gluDeleteQuadric(objLeft);
        gluDeleteQuadric(objRight);
    }

    void TScene::draw() const
    {
        const GLint details{ 20 };
        const GLfloat w2 = width * 0.5f;

        // down
        glPushMatrix();
        glTranslatef(-w2, downY, z);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, image.texture);
        gluQuadricTexture(objDown, GL_TRUE);
        gluCylinder(objDown, logRadius, logRadius, width, details, details);
        glPopMatrix();

        // up
        glPushMatrix();
        glTranslatef(-w2, topY, z);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, image.texture);
        gluQuadricTexture(objTop, GL_TRUE);
        gluCylinder(objTop, logRadius, logRadius, width, details, details);
        glPopMatrix();

        // left
        glPushMatrix();
        glTranslatef(-w2, topY, z);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, image.texture);
        gluQuadricTexture(objLeft, GL_TRUE);
        gluCylinder(objLeft, logRadius, logRadius, height, details, details);
        glPopMatrix();

        // right
        glPushMatrix();
        glTranslatef(w2, topY, z);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, image.texture);
        gluQuadricTexture(objRight, GL_TRUE);
        gluCylinder(objRight, logRadius, logRadius, height, details, details);
        glPopMatrix();
    }

    //================================================================================================
    // class TTarget

    TTarget::TTarget()
        : active{ true }, z{ -5.0f }
        , cx{ 0.0f }, cy{ 0.0f }, vx{ 0.0f }, size{ 0.0f }
        , points{ 0U }
    {

    }

    void TTarget::draw() const
    {
        if (active)
        {
            glBindTexture(GL_TEXTURE_2D, image.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(cx - size, cy - size, z);
            glTexCoord2f(1.0, 1.0); 
            glVertex3f(cx - size, cy + size, z);
            glTexCoord2f(0.0, 1.0); 
            glVertex3f(cx + size, cy + size, z);
            glTexCoord2f(0.0, 0.0); 
            glVertex3f(cx + size, cy - size, z);
            glEnd();
        }
    }

    void __fastcall TTarget::move(const size_t dtMs)
    {
        if (active)
        {
            cx = cx + vx * dtMs * speed;
            const GLfloat sceneLogRadius{ 0.25f };
            const GLfloat border = sceneWidth * 0.5f - size - sceneLogRadius;
            if (cx > 0.0f && cx > border)
            {
                vx *= -1.0f;
                cx = border;
            }
            if (cx < 0.0f && cx < -border)
            {
                vx *= -1.0f;
                cx = -border;
            }
        }
    }

    bool __fastcall TTarget::isInside(const GLfloat x, const GLfloat y) const
    {
        return ((x > cx - size) && (x < cx + size) && (y > cy - size) && (y < cy + size));
    }

    //================================================================================================
    // class TGun

    TGun::TGun()
        : length{ 1.0f }
        , xzAngle{ 0.0f }, yzAngle{ 0.0f }
        , gunAngleWidth{ 45.0f }
    {
        objBarrel = gluNewQuadric();
        objBase = gluNewQuadric();
    }

    TGun::~TGun()
    {
        gluDeleteQuadric(objBarrel);
        gluDeleteQuadric(objBase);
    }

    void TGun::draw() const
    {
        glPushMatrix();
        glTranslatef(0, -1, 3);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

        const GLfloat xzAngleDegree{ xzAngle * gunAngleWidth };
        const GLfloat yzAngleDegree{ yzAngle * gunAngleWidth };

        glRotatef(xzAngleDegree, 0.0f, 1.0f, 0.0f);
        glRotatef(yzAngleDegree, 1.0f, 0.0f, 0.0f);

        const GLfloat barrelBigRadius{ 0.2f };
        const GLfloat barrelSmallRadius{ 0.1f };
        const GLint details{ 20 };

        glBindTexture(GL_TEXTURE_2D, image.texture);
        gluQuadricTexture(objBarrel, GL_TRUE);
        gluCylinder(objBarrel, barrelBigRadius, barrelSmallRadius, length, details, details);

        const GLfloat gunBaseRadius{ 0.3f };

        glBindTexture(GL_TEXTURE_2D, image.texture);
        gluQuadricTexture(objBase, GL_TRUE);
        gluSphere(objBase, gunBaseRadius, details, details);

        glPopMatrix();
    }

    //================================================================================================
    // class TScreen

    void TScreen::draw() const
    {
        const GLfloat h2 = height * 0.5f;
        const GLfloat w2 = width * 0.5f;
        glBindTexture(GL_TEXTURE_2D, image.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-w2, -h2, z);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(w2, -h2, z);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(w2, h2, z);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-w2, h2, z);
        glEnd();
    }

    //================================================================================================
    // class TGunAmunitions

    void __fastcall TGunAmunitions::addFileAsTexture(const std::string& fname)
    {
        fileNames.push_back(fname);
    }

    void TGunAmunitions::draw() const
    {
        if (currentAmunitionsNumber < fileNames.size())
        {
            const GLfloat h2 = height * 0.5f;
            const GLfloat w2 = width * 0.5f;
            glBindTexture(GL_TEXTURE_2D, curImage.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(cx - w2, cy - h2, cz);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(cx + w2, cy - h2, cz);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(cx + w2, cy + h2, cz);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(cx - w2, cy + h2, cz);
            glEnd();
        }
    }

}; // namespace GunsAndPuns