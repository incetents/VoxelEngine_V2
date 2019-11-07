
#Attributes
{
	vec3 m_position 	: 0
	vec2 m_uv 			: 1
	vec3 m_normal 		: 2
	vec3 m_tangent 		: 6
	mat4 instanceMatrix : 8
}

#Include
{
	_UBO.glsl
}

#Properties
{
	// Uniforms
	uniform bool VXL_useModel 		 = true;
	uniform mat4 VXL_model			 = mat4(1.0);
	uniform mat3 VXL_normalMatrix	 = mat3(1.0);
	uniform bool VXL_useInstancing 	 = false;
	
	// Uniform Textures
	layout (binding = 0) uniform sampler2D albedo_handler;
	// Uniforms
	uniform bool VXL_useTexture = false;
	uniform vec3 VXL_color 		= vec3(1,1,1);
	uniform vec3 VXL_tint 		= vec3(1,1,1);
	uniform vec4 VXL_output 	= vec4(0,0,0,0); // used for color ID
	uniform float VXL_alpha 	= 1.0;
}

#Vertex
{
	// Output
	out vertex_data
	{
		vec3 pos;
		vec2 uv;
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;

	} v_data;

	// Main
	void main()
	{
		// Model
		if(VXL_useModel)
		{
			mat4 model = VXL_model;
			
			// Instance matrix occurs before model matrix (if applicable)
			if(VXL_useInstancing)
				model *= instanceMatrix;

			// Position
			v_data.pos = vec3(model * vec4(m_position, 1.0));
			// Normals
			v_data.normal = VXL_normalMatrix * m_normal;
			v_data.tangent = VXL_normalMatrix * m_tangent;
		}
		// Passthrough
		else
		{
			// Position
			v_data.pos = m_position;
			// Normal
			v_data.normal = m_normal;
			v_data.tangent = m_tangent;
		}
		
		gl_Position = UBO_viewProjection * vec4(v_data.pos, 1); 
		
		// Constants
		v_data.uv = m_uv;
		v_data.bitangent = cross(v_data.normal, v_data.tangent);
	}
}

#Fragment
{
	// Input
	in vertex_data
	{
		vec3 pos;
		vec2 uv;
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;

	} v_data;

	// Output
	layout (location = 0) out vec4 output_albedo;
	layout (location = 1) out vec4 output_normal;
	layout (location = 2) out vec4 output_reflection;
	layout (location = 3) out vec4 output_colorID;

	//Main
	void main()
	{
		if(VXL_useTexture)
		{
			vec4 _tex = texture(albedo_handler, v_data.uv);
			output_albedo.rgb = _tex.rgb * VXL_tint;
		}
		else
		{
			output_albedo.rgb = VXL_color * VXL_tint;
		}
			
		// output UV for testing reasons
		//output_albedo = vec4(v_data.uv, 0, 1);
		
		output_normal = vec4(normalize(v_data.normal), 1); // worldspace Normals
		
		//	if(TESTMODE == 1)
		//		output_albedo = vec4(normalize(v_data.normal), 1.0);
		//	if(TESTMODE == 2)
		//		output_albedo = vec4(normalize(v_data.tangent), 1.0);
		//	if(TESTMODE == 3)
		//		output_albedo = vec4(normalize(v_data.bitangent), 1.0);
		
		output_albedo.a = VXL_alpha;

		//float d = dot(-getCameraForwad(), normalize(v_data.normal));	
		//output_test = vec4(d, 0, 0.2, 1);

		output_reflection = vec4(0,0,0,1);
		output_colorID = VXL_output;
	}
}