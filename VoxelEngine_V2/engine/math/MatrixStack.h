// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../Utilities/Singleton.h"
#include "../Math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"

#include <Windows.h>
#include <stack>
#include <string>
#include <assert.h>

namespace Vxl
{
	//	static class _MatrixStack : Singleton<class _MatrixStack>
	//	{
	//	private:
	//		std::stack<Matrix4x4> m_stack;
	//		BYTE m_size = 0;
	//	public:
	//		_MatrixStack&	pushMatrix(const Matrix4x4& matrix);
	//		_MatrixStack&	popMatrix();
	//		Matrix4x4		getState();
	//	
	//	
	//	} SingletonInstance(_MatrixStack);
}