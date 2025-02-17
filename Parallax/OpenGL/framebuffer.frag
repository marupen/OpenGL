#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
 
uniform sampler2D screenTexture;
 
const vec2 offset = vec2(1.0 / 400.0, 1.0 / 300.0);  
 
void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset.x,  offset.y), // �������-�����
        vec2( 0.0f,    offset.y), // �������-�����������
        vec2( offset.x,  offset.y), // �������-������
        vec2(-offset.x,  0.0f),   // �����������-�����
        vec2( 0.0f,    0.0f),   // �����������-�����������
        vec2( offset.x,  0.0f),   // �����������-������
        vec2(-offset.x, -offset.y), // ������-�����
        vec2( 0.0f,   -offset.y), // ������-�����������
        vec2( offset.x, -offset.y)  // ������-������    
    );
 
    float kernel[9] = float[](
    0, 0, 0,
    0, 1, 0,
    0, 0, 0  
);
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);
} 