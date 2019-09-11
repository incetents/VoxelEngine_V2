// Copyright(c) 2019 Emmanuel Lajeunesse

// [ Core Uniforms for Materials ]
uniform bool  VXL_useModel 			= false;
uniform mat4  VXL_model 			= mat4(1.0);
uniform mat3  VXL_model_rotation 	= mat3(1.0);
uniform bool  VXL_useInstancing 	= false;
uniform bool  VXL_useTexture 		= false;
uniform vec3  VXL_color 			= vec3(1.0);
uniform vec3  VXL_tint 				= vec3(1.0);
uniform vec4  VXL_viewport 			= vec4(0.0);
uniform float VXL_alpha 			= 1.0;
uniform vec4  VXL_output 			= vec4(1.0);