uniform highp mat4 MVP;

attribute highp vec3 vertexPosition_mdl;
attribute highp vec2 vertexUV;
attribute highp vec3 vertexNormal_mdl;

varying highp vec3 pos;

void main() {
    gl_Position = MVP * vec4(vertexPosition_mdl, 1.0);
    pos = vertexPosition_mdl;
}
