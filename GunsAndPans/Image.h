#pragma once

#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//================================================================================================

namespace GunsAndPuns
{
    // Класс для хранения текстур
    struct TImage
    {
        GLsizei width{ 0 };
        GLsizei height{ 0 };
        GLuint texture{ 0U };
        GLubyte* data{ nullptr };

        enum class TGeneratedImg
        {
            CHESS
        };

        void __fastcall makeImage(const TGeneratedImg id);
        bool __fastcall loadImage(const std::string& filename);
        void genTexture();

        ~TImage()
        {
            if (data != nullptr)
            {
                delete[] data;
            }
        }
    };

}; // namespace GunsAndPuns

//================================================================================================
