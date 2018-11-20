#version 420

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;

} f_data;

//Values sent out (to final array)
layout (location = 0) out vec4 final_color;

layout (binding = 0) uniform sampler2D texture1;
//layout (binding = 1) uniform sampler2D texture2;

//Main
void main()
{
	vec4 tex1 = texture(texture1, f_data.uv);
	//vec4 tex2 = texture(texture2, f_data.uv);

	final_color = tex1;
	//final_color = mix(tex1, tex2, 0.5);
	//final_color.rgb = vec3(1,1,1) - final_color.rgb
	//final_color.r = 0;
}