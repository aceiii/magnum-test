uniform vec3 color = vec3(1.0, 1.0, 1.0);
uniform sampler2D textureData;

in vec2 interpolatedTexturedCoordinates;
in vec3 interpolatedColor;

out vec4 fragmentColor;

void main() {
    fragmentColor.rgb = interpolatedColor * color * texture(textureData, interpolatedTexturedCoordinates).rgb;
    fragmentColor.a = 1.0;
}
