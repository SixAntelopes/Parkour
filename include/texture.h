#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>

class Texture2D {
public:
    unsigned int ID;
    unsigned int Width, Height; // 加载图片大小
    unsigned int Internal_Format;   // 纹理格式
    unsigned int Image_Format;  // 加载的图片格式
    unsigned int Wrap_S;    // S轴的环绕方式
    unsigned int Wrap_T;    // T轴的环绕方式
    unsigned int Filter_Min;    // 缩小时的过滤方式
    unsigned int Filter_Max;    // 放大时的过滤方式
    unsigned char* gif_data = nullptr;
    int frames = 0, frame = 0, delays = 0, delay = 0, stride_bytes = 0;

    Texture2D();    // 构造（设置默认纹理模式）
    // 从图片生成纹理
    void Generate(unsigned int width, unsigned int height, unsigned char* data, int delays = 0, int frames = 0, int stride_bytes = 0);
    void Bind() const;  // 捆绑纹理到目前活跃的 GL_TEXTURE_2D 纹理对象
};

#endif