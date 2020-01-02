// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "MatrixStack.h"

namespace Vxl
{
	//	_MatrixStack& _MatrixStack::pushMatrix(const Matrix4x4& matrix)
	//	{
	//		VXL_ASSERT(m_size < 255);
	//	
	//		if (m_size == 0)
	//			m_stack.push(matrix);
	//		else
	//			m_stack.push(m_stack.top() * matrix);
	//	
	//		m_size++;
	//		return *this;
	//	}
	//	_MatrixStack& _MatrixStack::popMatrix()
	//	{
	//		if (m_size == 0)
	//			return *this;
	//	
	//		m_stack.pop();
	//	
	//		m_size--;
	//		return *this;
	//	}
	//	Matrix4x4 _MatrixStack::getState()
	//	{
	//		if (m_size == 0)
	//			return Matrix4x4();
	//	
	//		return m_stack.top();
	//	}
}