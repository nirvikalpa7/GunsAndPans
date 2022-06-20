
#include <iostream>
#include <fstream>
#include <filesystem>

#include <windows.h>

#include "Image.h"

//================================================================================================

namespace GunsAndPuns
{

    void TImage::genTexture()
    {
        if (data == nullptr || width == 0 || height == 0)
        {
            return;
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // scale linearly when image bigger than texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // scale linearly when image smalled than texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    }

    void __fastcall TImage::makeImage(const TGeneratedImg id)
    {
        if (id == TGeneratedImg::CHESS)
        {
            width = 128;
            height = 128;
            size_t SIZE = width * height;
            SIZE = (SIZE << 1) + SIZE;
            data = new (std::nothrow) GLubyte[SIZE];
            if (data == nullptr)
            {
                std::cerr << "Can not allocate memory in " << __FUNCTION__ << std::endl;
                return;
            }
            GLubyte c;
            size_t index, indexI;
            for (size_t i = 0; i < height; i++)
            {
                indexI = i * width;
                for (size_t j = 0; j < width; j++)
                {
                    c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
                    index = indexI + j;
                    index = (index << 1) + index;
                    data[index + 0] = c;
                    data[index + 1] = c;
                    data[index + 2] = c;
                }
            }
        }
    }

    bool __fastcall TImage::loadImage(const std::string& filename)
    {
        namespace fs = std::filesystem;

        if (filename.size() == 0 || !fs::exists(filename))
        {
            std::cerr << "Incorrect input params (filename) in " << __FUNCTION__ << std::endl;
            return false;
        }

        std::ifstream fin(filename.c_str(), std::ios_base::binary);
        if (!fin.is_open())
        {
            std::cerr << "Can not open image file: " << filename << " in " << __FUNCTION__ << std::endl;
            return false;
        }
        const size_t bmpHeaderSize = 54;
        char info[bmpHeaderSize];
        fin.read(info, bmpHeaderSize); // read the 54-byte header

        // extract image height and width from header
        const size_t offsetWidth = 18;
        const size_t offsetHeight = 22;
        const size_t offsetBitCount = 28;
        width = *(int*)&info[offsetWidth];
        height = *(int*)&info[offsetHeight];
        const short int bitCount = *(short int*)&info[offsetBitCount];
        if (bitCount != 24)
        {
            std::cerr << "Incorrect bitCount for an image: " << filename << std::endl;
            return false;
        }

        const int row_padded = (width * 3 + 3) & (~3);
        data = new (std::nothrow) GLubyte[width * height * 3];
        if (data == nullptr)
        {
            std::cerr << "Can not allocate memory in " << __FUNCTION__ << std::endl;
            fin.close();
            return false;
        }
        char* dataLine = new (std::nothrow) char[row_padded];
        if (dataLine == nullptr)
        {
            std::cerr << "Can not allocate memory in " << __FUNCTION__ << std::endl;
            delete[] data;
            fin.close();
            return false;
        }

        size_t j3, index, indexR, indexG;
        for (size_t i = 0U; i < height; i++)
        {
            fin.read(dataLine, row_padded);
            size_t offset = i * width;
            offset = (offset << 1) + offset;
            for (size_t j = 0U; j < width; j++)
            {
                // Convert (B, G, R) to (R, G, B)
                j3 = (j << 1) + j;
                index = offset + j3;
                indexR = j3 + 2;
                if (indexR < row_padded)
                {
                    data[index] = dataLine[indexR];
                }
                indexG = j3 + 1;
                if (indexG < row_padded)
                {
                    data[index + 1] = dataLine[indexG];
                }
                data[index + 2] = dataLine[j3];
            }
        }

        delete[] dataLine;
        fin.close();
        return true;
    }

}; // namespace GunsAndPuns