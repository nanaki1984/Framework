#pragma translucency AlphaBlend

#pragma zEnable 1
#pragma zWriteEnable 0
#pragma zFunc LessEqual

#pragma blendEnable 1
#pragma blendOp Add
#pragma blendSrcFactor SrcAlpha
#pragma blendDstFactor InvSrcAlpha

#pragma vertex
attribute vec2 position;
attribute vec2 uv;
attribute vec4 color;

uniform mat4 ProjMtx;

varying vec2 Frag_UV;
varying vec4 Frag_Color;

void main()
{
	Frag_UV = uv;
	Frag_Color = color;
	gl_Position = ProjMtx * vec4(position.xy, 0, 1);
}

#pragma fragment
#version 130
uniform sampler2D Texture;

in vec2 Frag_UV;
in vec4 Frag_Color;

out vec4 Out_Color;

void main()
{
	Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
}
