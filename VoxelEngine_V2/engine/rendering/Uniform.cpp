// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Uniform.h"

#include <GL/gl3w.h>

#include "Shader.h"

#include "../math/Vector.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

namespace Vxl
{
	//	glSubroutine::glSubroutine(Shader* shader, uint32_t subroutine_count)
	//	{
	//		m_type = shader->GetType();
	//		m_locationCount = subroutine_count;
	//		m_locations = new uint32_t[subroutine_count];
	//		memset(m_locations, 0, subroutine_count * sizeof(uint32_t));
	//	}
	//	
	//	void glSubroutine::Set(const std::string& UniformName, const std::string& FunctionName)
	//	{
	//		if (m_uniforms.find(UniformName) != m_uniforms.end() && m_functions.find(FunctionName) != m_functions.end())
	//		{
	//			uint32_t index = m_uniforms[UniformName];
	//			uint32_t functionIndex = m_functions[FunctionName];
	//			m_locations[index] = functionIndex;
	//			return;
	//		}
	//		VXL_ASSERT(FALSE, "Subroutine Name or Function not found");
	//	}
	//	
	//	void glSubroutine::Bind()
	//	{
	//		glUniformSubroutinesuiv((GLenum)m_type, m_locationCount, m_locations);
	//	}

}