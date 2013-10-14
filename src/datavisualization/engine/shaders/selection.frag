uniform highp vec3 color_mdl;

void main() {
    gl_FragColor.rgb = color_mdl;
    gl_FragColor.a = 1.0;
}

