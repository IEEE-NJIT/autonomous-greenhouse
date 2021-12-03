#version 120

#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D texSampler;

varying vec2 textureCoord;
varying vec4 textureColor;

void main() {
    //gl_FragColor = vec4(1, 1, 1, texture2D(texSampler, textureCoord).r) * textureColor;
    gl_FragColor = vec4(textureColor.xyz, texture2D(texSampler, textureCoord).a);
}
