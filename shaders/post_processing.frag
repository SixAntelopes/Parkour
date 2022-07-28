#version 330 core
in  vec2  TexCoords;
out vec4  color;

uniform sampler2D scene;
uniform vec2      offsets[9];
uniform int       edge_kernel[9];
uniform float     blur_kernel[9];

uniform bool shake;

void main() {
    color = vec4(0.0f);
    vec3 sample[9];
    // 若使用卷积矩阵，对纹理的偏移像素进行采样 // 对纹理座标，使用STPQ
    if(shake)
    // 处理特效
    if (shake) {
        for(int i = 0; i < 9; i++) {
            sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
            color += vec4(sample[i] * blur_kernel[i], 0.0f);
        }
        color.a = 1.0f;
    } else {
        color =  texture(scene, TexCoords);
    }
}