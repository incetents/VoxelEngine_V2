// Copyright(c) 2019 Emmanuel Lajeunesse

// Math.glsl
#define PI (3.14159)
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
// H [0,360], S = [0=white, 1=fullcolor], V = [0=dark, 1=fullcolor]
vec3 HSV_TO_RGB(float h, float s, float v)
{
	float     hh, p, q, t, ff;
    int        i;

    hh = mod(h, 360.0);
    hh /= 60.0;
    i = int(hh);
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
		return vec3(v,t,p);
    case 1:
		return vec3(q,v,p);
    case 2:
		return vec3(p,v,t);
    case 3:
		return vec3(p,q,v);
    case 4:
		return vec3(t,p,v);
    case 5:
    default:
		return vec3(v,p,q);
    }
}