// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;
layout (location = 8) in mat4 instanceMatrix;

// Output
out fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;

} f_data;

// Uniforms
uniform mat4 VXL_model			= mat4(1.0);
uniform mat4 VXL_viewProjection = mat4(1.0);
uniform bool VXL_useInstancing = false;
uniform bool VXL_useModel = true;

// Main
void main()
{
	// Model
	if(VXL_useModel)
	{
		mat4 model = VXL_model;
		
		if(VXL_useInstancing)
			model = instanceMatrix * model;

		vec4 object_position = model * vec4(m_position, 1.0);

		// Position
		f_data.pos = vec3(object_position);
		// Normal
		f_data.normal = vec3(model * vec4(m_normal, 0));
	}
	else
	{
		// Position
		f_data.pos = m_position;
		// Normal
		f_data.normal = m_normal;
	}
	gl_Position = VXL_viewProjection * vec4(f_data.pos, 1); 
	
	// UV
	f_data.uv = m_uv;
	
}