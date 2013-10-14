#version 120

uniform highp vec3 lightPosition_wrld;
uniform highp vec3 color_mdl;
uniform highp float lightStrength;
uniform highp float ambientStrength;

varying highp vec3 position_wrld;
varying highp vec3 normal_cmr;
varying highp vec3 eyeDirection_cmr;
varying highp vec3 lightDirection_cmr;
varying highp vec2 coords_mdl;

void main() {
    highp float heightMod = (coords_mdl.y * 0.3) + 0.7; // Add 30% black to the bottom
    highp vec3 materialDiffuseColor = heightMod * color_mdl;
    highp vec3 materialAmbientColor = vec3(ambientStrength, ambientStrength, ambientStrength) * materialDiffuseColor;
    highp vec3 materialSpecularColor = vec3(1.0, 1.0, 1.0);

    highp float distance = length(lightPosition_wrld - position_wrld);
    highp vec3 n = normalize(normal_cmr);
    highp vec3 l = normalize(lightDirection_cmr);
    highp float cosTheta = clamp(dot(n, l), 0.0, 1.0);

    highp vec3 E = normalize(eyeDirection_cmr);
    highp vec3 R = reflect(-l, n);
    highp float cosAlpha = clamp(dot(E, R), 0.0, 1.0);

    gl_FragColor.rgb =
        materialAmbientColor +
        materialDiffuseColor * lightStrength * (cosTheta * cosTheta) / distance +
        materialSpecularColor * lightStrength * pow(cosAlpha, 5) / distance;
    gl_FragColor.rgb = clamp(gl_FragColor.rgb, 0.0, 1.0);
    gl_FragColor.a = 1.0;
}

