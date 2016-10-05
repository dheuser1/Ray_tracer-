#version 400 core

layout(location=0) in vec4 vPosition;
uniform vec3 vColor;
uniform mat4 projection;
uniform mat4 modelview;
out vec4 outColor;

void main()
{
    gl_Position = projection * modelview * vec4(vPosition.xyzw);
    outColor = vec4(vColor,1.0);
}
