#pragma once

#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//================================================================================================

namespace GunsAndPuns
{
    // Game OpenGL texture
    struct TImage
    {
        TImage() : width{ 0 }, height{ 0 }, texture{ 0U }, data{ nullptr }
        {
        }

        GLsizei width;
        GLsizei height;
        GLuint texture;
        GLubyte* data;
                
        enum class TGeneratedImg
        {
            CHESS
        };

        void __fastcall makeImage(const TGeneratedImg id);
        bool __fastcall loadImage(const std::string& filename);
        void genTexture();

        void freeMem()
        {
            if (data != nullptr)
            {
                delete[] data;
                data = nullptr;
            }
            width = 0;
            height = 0;
            texture = 0U;
        }

        ~TImage()
        {
            freeMem();
        }
    };

}; // namespace GunsAndPuns

//================================================================================================
