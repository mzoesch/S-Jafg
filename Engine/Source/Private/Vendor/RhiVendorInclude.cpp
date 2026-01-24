// Copyright mzoesch. All rights reserved.

#include "Rhi/RhiVendorInclude.h"

#define STB_IMAGE_IMPLEMENTATION
// #define STBI_NO_JPEG
// #define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM   (.ppm and .pgm)
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
