layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 color;

out vec2 interpolatedTexturedCoordinates;
out vec3 interpolatedColor;

void main() {
    interpolatedTexturedCoordinates = textureCoordinates;
    interpolatedColor = color;

    gl_Position = position;
}
