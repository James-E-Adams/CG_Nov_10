// Extended by James Adams and Shiran Gazit.

#version 330

layout(location = 0) in vec4 position;
uniform vec2 window;
uniform mat4 translation;
uniform mat4 scale;
out vec2 window_size;

void main()
{
    vec4 tempPos = position;
    mat4 tempTrans = translation;
    float ratio = window.x / window.y;

    if(ratio > 1)
    {
        tempPos.x = tempPos.x / ratio;
    }
    else if(ratio < 1)
    {
        tempPos.y = tempPos.y * ratio;
    }

    gl_Position = tempTrans * tempPos;
    window_size = window;
}
