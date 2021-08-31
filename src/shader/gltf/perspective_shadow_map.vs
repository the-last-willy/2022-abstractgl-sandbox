#version 460 core

uniform mat4 mvp;

// uniform float near;
// uniform float far;

// LOCATION BADNESS, TIED TO G_BUFFER SHADER.
layout(location = 2) in vec3 POSITION;

void main() {
    gl_Position = mvp * vec4(POSITION, 1.);

    // vec4 position = mvp * vec4(POSITION, 1.);

    // gl_Position.xyw = position.xyw;

    // float a = - 2 / (near - far);
	// float b = (near + far) / (near - far);

    // gl_Position.z = position.w * (a * position.z + b);
}
