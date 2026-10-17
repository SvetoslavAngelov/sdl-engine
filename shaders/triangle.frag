#version 450
layout(location = 0) in vec4 vertex_colour;
layout(location = 0) out vec4 fragment_colour;

void main() {
    fragment_colour = vertex_colour;
}
