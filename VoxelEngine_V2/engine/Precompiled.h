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

#include "math/Color.h"
#include "math/Lerp.h"
#include "math/MathCore.h"
#include "math/Matrix2x2.h"
#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"
#include "math/MatrixStack.h"
#include "math/Model.h"
#include "math/Quaternion.h"
#include "math/Random.h"
#include "math/Rotation.h"
#include "math/Transform.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "modules/Component.h"
#include "modules/Entity.h"
#include "modules/Layer.h"
#include "modules/Material.h"
#include "modules/RenderManager.h"
#include "modules/Scene.h"

#include "rendering/Debug.h"
#include "rendering/FramebufferObject.h"
#include "rendering/Geometry.h"
#include "rendering/Graphics.h"
#include "rendering/Mesh.h"
#include "rendering/Shader.h"
#include "rendering/UBO.h"
#include "rendering/Uniform.h"
#include "rendering/VBO.h"

#include "textures/Texture.h"
#include "textures/Cubemap.h"
#include "textures/RenderTexture.h"

#include "utilities/Asset.h"
#include "utilities/FileIO.h"
#include "utilities/Macros.h"
#include "utilities/Loader.h"
#include "utilities/logger.h"
#include "utilities/singleton.h"
#include "utilities/stringUtil.h"
#include "utilities/Time.h"
#include "utilities/Util.h"
