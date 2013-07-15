#version 150

varying highp vec2 UV;
varying highp vec2 coords_mdl;
varying highp vec3 position_wrld;
varying highp vec3 normal_cmr;
varying highp vec3 eyeDirection_cmr;
varying highp vec3 lightDirection_cmr;

uniform highp vec3 lightPosition_wrld;
uniform highp vec3 color_mdl;
uniform highp float lightStrength;
uniform highp float ambientStrength;

void main() {
    gl_FragColor.rgb = color_mdl;
}

