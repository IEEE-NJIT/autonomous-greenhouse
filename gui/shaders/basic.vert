#version 120

attribute vec3 position;
attribute vec4 color;
attribute vec2 texture;

varying vec4 outColor;
varying vec2 textureCoord;

uniform mat4 mvpMatrix;

void main() {
    outColor = color;
    textureCoord = texture;
    gl_Position = mvpMatrix * vec4(position, 1.0);
    //gl_Position = vec4(position, 1.0);
}