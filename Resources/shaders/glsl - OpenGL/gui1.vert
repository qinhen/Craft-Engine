#version 450
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;
layout (location = 2) in uint inCol;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outTexcoord;
layout (location = 0) uniform mat4 transformMatrix;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	outColor = unpackUnorm4x8(inCol);
	outTexcoord = inTex;
	vec2 pos = (transformMatrix * vec4(inPos, 1.0, 1.0)).xy;
	gl_Position = vec4(pos, 1.0, 1.0);
}
