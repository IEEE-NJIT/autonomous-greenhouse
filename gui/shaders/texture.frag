#version 120

#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D texSampler;

varying vec2 textureCoord;
varying vec4 textureColor;

void main() {
    //gl_FragColor = texture2D(texSampler, textureCoord) * textureColor;
    gl_FragColor = texture2D(texSampler, textureCoord);
}