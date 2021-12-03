#version 120

attribute vec3 position;
attribute vec4 color;
attribute vec2 texture;

varying vec2 textureCoord;
varying vec4 textureColor;

uniform mat4 mvpMatrix;

void main() {
    textureCoord = texture;
    textureColor = color;
    gl_Position = mvpMatrix * vec4(position, 1.0);
}