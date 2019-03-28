// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

// STL
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Engine
#include "input/Input.h"
#include "input/XGamePad.h"

#include "opengl/glUtil.h"
#include "opengl/FramebufferObject.h"
#include "opengl/Geometry.h"
#include "opengl/Mesh.h"
#include "opengl/Shader.h"
#include "opengl/Texture.h"

#include "math/Color.h"
#include "math/Lerp.h"
#include "math/MathCore.h"
#include "math/Matrix2x2.h"
#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/MatrixStack.h"
#include "math/Quaternion.h"
#include "math/Random.h"
#include "math/Rotation.h"
#include "math/Transform.h"