#pragma zEnable 1
#pragma zWriteEnable 1
#pragma zFunc LessEqual

#pragma vertex
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform mat4 WorldViewProj;

varying vec3 n;
varying vec2 _uv;

void main()
{
	gl_Position = WorldViewProj * vec4(position, 1.0);
	n = normal;
	_uv = vec2(uv.x, -uv.y);
}

#pragma fragment
#pragma include "test_inc.shader"

varying vec3 n;
varying vec2 _uv;

uniform sampler2D Diffuse;

void main()
{
	gl_FragColor = test_func(n) * texture2D(Diffuse, _uv);
}
