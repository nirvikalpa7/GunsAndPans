
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
        , cx{ 0.0f }, cy{ 0.0f }, vx{ 0.0f }, vy{ 0.0f }, size{ 0.0f }
        , points{ 0U }, horizontalTarget{ false }
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
            const GLfloat sceneLogRadius{ 0.25f };
            if (horizontalTarget)
            {
                cx = cx + vx * dtMs * speed;
                const GLfloat border = sceneWidth * 0.5f - size - sceneLogRadius;
                if (cx > 0.0f && cx > border)
                {
                    vx *= -1.0f;
                    cx = border;
                }
                else if (cx < 0.0f && cx < -border)
                {
                    vx *= -1.0f;
                    cx = -border;
                }
            }
            else
            {
                cy = cy + vy * dtMs * speed;
                const GLfloat borderTop = yTop - size - sceneLogRadius;
                const GLfloat borderDown = yDown + size + sceneLogRadius;
                if (cy > 0.0f && cy > borderTop)
                {
                    vy *= -1.0f;
                    cy = borderTop;
                }
                else if (cy < 0.0f && cy < borderDown)
                {
                    vy *= -1.0f;
                    cy = borderDown;
                }
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

    //================================================================================================
    // class TLevelTargets
        
    void TLevelTargets::init(const std::vector<std::string>& texturesNames)
    {
        appleTarget.loadTexture(texturesNames[6]);
        smallTarget.loadTexture(texturesNames[0]);
        bigTarget.loadTexture(texturesNames[1]);

        appleTarget.setPoints(30);
        smallTarget.setPoints(25);
        bigTarget.setPoints(20);

        leftTarget.loadTexture(texturesNames[8]);
        rightTarget.loadTexture(texturesNames[9]);
        fastTarget.loadTexture(texturesNames[10]);

        fastTarget.setPoints(30);
        leftTarget.setPoints(25);
        rightTarget.setPoints(25);
    }

    void TLevelTargets::reInit()
    {
        appleTarget.init(0.0f, 4.9f, 1.5f, 0.0f, 0.5f, true);
        smallTarget.init(0.0f, 2.3f, -1.0f, 0.0f, 0.75f, true);
        bigTarget.init(0.0f, -0.2f, 1.0f, 0.0f, 1.0f, true);

        fastTarget.init(0.0f, 4.9f, -1.6f, 0.0f, 0.5f, true);
        leftTarget.init(-3.0f, 1.0f, 0.0f, 1.0f, 0.75f, false);
        rightTarget.init(3.0f, 1.0f, 0.0f, -1.0f, 0.75f, false);

        level = TLevelNum::FIRST;
    }

    bool TLevelTargets::isTargetsActive() const
    {
        if (level == TLevelNum::FIRST)
        {
            return appleTarget.active || smallTarget.active || bigTarget.active;
        }
        else if (level == TLevelNum::SECOND)
        {
            return leftTarget.active || rightTarget.active || fastTarget.active;
        }

        return false;
    }

    size_t TLevelTargets::getAllTargetsPoints() const
    {
        return appleTarget.getPoints() + smallTarget.getPoints() + bigTarget.getPoints() +
            leftTarget.getPoints() + rightTarget.getPoints() + fastTarget.getPoints();
    }

    bool TLevelTargets::collisionCheck(const GLfloat x, const GLfloat y, size_t& scores, TBullet& bullet)
    {
        if (level == TLevelNum::FIRST)
        {
            if (smallTarget.active && smallTarget.isInside(x, y))
            {
                smallTarget.active = false;
                bullet.active = false;
                bullet.resetCenter();
                scores += smallTarget.getPoints();
                return true;
            }
            else if (bigTarget.active && bigTarget.isInside(x, y))
            {
                bigTarget.active = false;
                bullet.active = false;
                bullet.resetCenter();
                scores += bigTarget.getPoints();
                return true;
            }
            else if (appleTarget.active && appleTarget.isInside(x, y))
            {
                appleTarget.active = false;
                bullet.active = false;
                bullet.resetCenter();
                scores += appleTarget.getPoints();
                return true;
            }
        }
        else if (level == TLevelNum::SECOND)
        {
            if (leftTarget.active && leftTarget.isInside(x, y))
            {
                leftTarget.active = false;
                bullet.active = false;
                bullet.resetCenter();
                scores += leftTarget.getPoints();
                return true;
            }
            else if (rightTarget.active && rightTarget.isInside(x, y))
            {
                rightTarget.active = false;
                bullet.active = false;
                bullet.resetCenter();
                scores += rightTarget.getPoints();
                return true;
            }
            else if (fastTarget.active && fastTarget.isInside(x, y))
            {
                fastTarget.active = false;
                bullet.active = false;
                bullet.resetCenter();
                scores += fastTarget.getPoints();
                return true;
            }
        }

        return false;
    }

    void TLevelTargets::draw() const
    {
        if (level == TLevelNum::FIRST)
        {
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
        else if (level == TLevelNum::SECOND)
        {
            if (fastTarget.active)
            {
                fastTarget.draw();
            }
            if (leftTarget.active)
            {
                leftTarget.draw();
            }
            if (rightTarget.active)
            {
                rightTarget.draw();
            }
        }
    }

    void __fastcall TLevelTargets::move(const size_t dt)
    {
        if (level == TLevelNum::FIRST)
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
        }
        else if (level == TLevelNum::SECOND)
        {
            if (fastTarget.active)
            {
                fastTarget.move(dt);
            }
            if (leftTarget.active)
            {
                leftTarget.move(dt);
            }
            if (rightTarget.active)
            {
                rightTarget.move(dt);
            }
        }
    }

    void TLevelTargets::nextLevel()
    {
        if (level == TLevelNum::FIRST)
        {
            level = TLevelNum::SECOND;
        }
        else if (level == TLevelNum::SECOND)
        {
            level = TLevelNum::FINISH;
        }
    }

}; // namespace GunsAndPuns