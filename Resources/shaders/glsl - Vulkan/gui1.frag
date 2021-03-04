#version 450
layout (binding = 0) uniform sampler2D image;
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexcoord;
layout (location = 0) out vec4 outColor;

float SingleSapmle(vec2 texcoord)
{
	float sdf = texture(image, texcoord).r;
	float afwidth = length(vec2(dFdx(sdf), dFdy(sdf))) * 0.70710678118;
	return smoothstep(0.5 - afwidth, 0.5 + afwidth, sdf);
}

void main() 
{	
	float power = SingleSapmle(inTexcoord);
	outColor = vec4(inColor.rgb, inColor.a * power);	
}