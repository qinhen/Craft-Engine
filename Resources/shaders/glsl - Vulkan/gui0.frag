#version 450
layout (binding = 0) uniform sampler2D image;
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexcoord;
layout (location = 0) out vec4 outColor;

void main() 
{
	outColor = inColor * texture(image, inTexcoord);
}