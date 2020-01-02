// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_Inspector.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_colors.h"

#include "../editor/Editor.h"

#include "../input/Input.h"

#include "../math/Vector.h"

#include "../modules/Entity.h"

#include "../textures/Texture2D.h"

#include "../utilities/Asset.h"

namespace Vxl
{
	// Imgui Locking Shortcut
	bool Locked = false;
	bool ioPrev_MouseDoubleClicked = false;
	bool ioPrev_KeyCtrl = false;
	void DisableDragInputStart()
	{
		auto& io = ImGui::GetIO();

		ioPrev_MouseDoubleClicked = io.MouseDoubleClicked[0];
		ioPrev_KeyCtrl = io.KeyCtrl;

		io.MouseDoubleClicked[0] = false;
		io.KeyCtrl = false;

		ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
	}
	void DisableDragInputEnd()
	{
		auto& io = ImGui::GetIO();

		io.MouseDoubleClicked[0] = ioPrev_MouseDoubleClicked;
		io.KeyCtrl = ioPrev_KeyCtrl;

		ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, false);
	}

	void DisableStart()
	{
		if (Locked == true)
			return;

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

		Locked = true;
	}
	void DisableEnd()
	{
		if (Locked == false)
			return;

		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		Locked = false;
	}

	bool CheckBoxTristate(const char* label, int* v_tristate)
	{
		bool ret;
		if (*v_tristate == -1)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
			bool b = false;
			ret = ImGui::Checkbox(label, &b);
			if (ret)
				*v_tristate = 1;
			ImGui::PopItemFlag();
		}
		else
		{
			bool b = (*v_tristate != 0);
			ret = ImGui::Checkbox(label, &b);
			if (ret)
				*v_tristate = (int)b;
		}
		return ret;
	}
	//

	void Inspector::Draw()
	{
		if (Editor.hasSelection())
		{
			bool multiSelect = Editor.m_selectedNodes.size() > 1;

			// All Nodes
			std::vector<SceneNode*> nodes;
			uint32_t nodeCount = (uint32_t)Editor.m_selectedNodes.size();
			nodes.reserve(nodeCount);
			for (uint32_t i = 0; i < nodeCount; i++)
				nodes.push_back(Assets.getSceneNode(Editor.m_selectedNodes[i]));

			// First Node
			SceneNode* node = Assets.getSceneNode(Editor.m_selectedNodes[0]);

			// Name
			static char Name[MAX_ENTITY_NAME_LENGTH];
			strcpy_s(Name, node->m_name.c_str());

			ImGui::Text("Name: "); ImGui::SameLine();

			// ~ Section
			ImGui::PushItemWidth(-1);

			float EntityColor[3] = { node->m_labelColor.r, node->m_labelColor.g, node->m_labelColor.b };

			if (multiSelect)
			{
				// No name
				ImGui::Text(" --- ");
				// No ID
				ImGui::TextColored(ImGuiColor::Orange, "ID: ---");

				// Active
				int triState = -1;
				bool AllFalse = true;
				bool AllTrue = true;
				for (const auto& node : nodes)
				{
					if (node->m_isActive)
						AllFalse = false;
					else
						AllTrue = false;
				}
				if (AllTrue)
					triState = 1;
				else if (AllFalse)
					triState = 0;

				if (CheckBoxTristate("Active", &triState))
				{
					for (const auto& node : nodes)
						node->m_isActive = (bool)triState;
				}

				// Color
				ImGui::TextColored(ImGuiColor::Orange, "Label Color:");
				ImGui::SameLine();

				if (ImGui::ColorEdit3("Label Color", EntityColor, ImGuiColorEditFlags_NoInputs))
				{
					for(auto& node : nodes)
						node->m_labelColor = Color3F(EntityColor[0], EntityColor[1], EntityColor[2]);
				}
			}
			else
			{
				// Naming Scheme
				if (ImGui::InputText("input text", Name, IM_ARRAYSIZE(Name), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					node->m_name = std::string(Name);
				}
				// ID
				ImGui::TextColored(ImGuiColor::Orange, "ID: %d", node->m_uniqueID);

				// Active
				ImGui::Checkbox("Active", &node->m_isActive);

				// Color
				ImGui::TextColored(ImGuiColor::Orange, "Label Color:");
				ImGui::SameLine();

				if (ImGui::ColorEdit3("Label Color", EntityColor, ImGuiColorEditFlags_NoInputs))
				{
					node->m_labelColor = Color3F(EntityColor[0], EntityColor[1], EntityColor[2]);
				}
			}

			// ~ Section
			ImGui::PopItemWidth();

			if (ImGui::Button("Copy Color"))
			{
				LabelColorClipboard.r = EntityColor[0];
				LabelColorClipboard.g = EntityColor[1];
				LabelColorClipboard.b = EntityColor[2];
			}
			ImGui::SameLine();
			if (ImGui::Button("Paste Color"))
			{
				node->m_labelColor = LabelColorClipboard;
			}
			ImGui::PushItemWidth(-1);

			// Drag Speed
			ImGui::Text("DragSpeed: "); ImGui::SameLine(); ImGui::DragFloat("DragSpeed", &DragSpeed, 0.05f, 0.1f, 10.0f);
			ImGui::PopItemWidth();

			ImGui::Separator();

			// Transform
			DrawTransform();
			// Material
			DrawMaterial();
		}
	}

	void Inspector::DrawTransform()
	{
		bool multiSelect = Editor.m_selectedNodes.size() > 1;

		// All Nodes
		std::vector<SceneNode*> nodes;
		uint32_t nodeCount = (uint32_t)Editor.m_selectedNodes.size();
		nodes.reserve(nodeCount);
		for (uint32_t i = 0; i < nodeCount; i++)
			nodes.push_back(Assets.getSceneNode(Editor.m_selectedNodes[i]));

		// First Node
		SceneNode* node = Assets.getSceneNode(Editor.m_selectedNodes[0]);

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushItemWidth(-1);

			if (multiSelect)
			{
				float multi_p[3] = { 0, 0, 0 };

				DisableDragInputStart();

				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Yellow);
				//
				ImGui::TextColored(ImGuiColor::Orange, node->m_useTransform ? "Local:" : "Local: [LOCKED]");
				ImGui::Text("Position:"); ImGui::SameLine();
				if (ImGui::DragFloat3("Position", multi_p, DragSpeed, 0, 0, "---"))
				{
					for (uint32_t i = 0; i < Editor.m_selectedNodes.size(); i++)
					{
						SceneNode* node = Assets.getSceneNode(Editor.m_selectedNodes[i]);
						node->m_transform.increasePosition(multi_p[0], multi_p[1], multi_p[2]);
					}
				}
				// Nothing
				DisableStart();
				ImGui::Text("Rotation:"); ImGui::SameLine();
				ImGui::DragFloat3("Rotation", multi_p, DragSpeed, 0, 0, "---");
				ImGui::Text("Scale:   "); ImGui::SameLine();
				ImGui::DragFloat3("Scale", multi_p, DragSpeed, 0, 0, "---");
				DisableEnd();
				//
				ImGui::Separator();
				ImGui::TextColored(ImGuiColor::Orange, "World:");
				//
				ImGui::Text("Position:");
				ImGui::SameLine();
				if (ImGui::DragFloat3("PositionW", multi_p, DragSpeed, 0, 0, "---"))
				{
					for (uint32_t i = 0; i < Editor.m_selectedNodes.size(); i++)
					{
						SceneNode* node = Assets.getSceneNode(Editor.m_selectedNodes[i]);
						node->m_transform.increaseWorldPosition(multi_p[0], multi_p[1], multi_p[2]);
					}
				}
				//
				DisableStart();
				ImGui::Text("Forward: "); ImGui::SameLine();
				ImGui::DragFloat3("Forward", multi_p, DragSpeed, 0, 0, "---");
				ImGui::Text("Scale:   "); ImGui::SameLine();
				ImGui::DragFloat3("Scale", multi_p, DragSpeed, 0, 0, "---");
				DisableEnd();
				//
				ImGui::PopStyleColor();

				DisableDragInputEnd();
			}
			else
			{
				float p[3] = { node->m_transform.getPosition().x, node->m_transform.getPosition().y, node->m_transform.getPosition().z };
				float r[3] = { node->m_transform.getRotationEuler().x, node->m_transform.getRotationEuler().y, node->m_transform.getRotationEuler().z };
				float s[3] = { node->m_transform.getScale().x, node->m_transform.getScale().y, node->m_transform.getScale().z };

				float pw[3] = { node->m_transform.getWorldPosition().x, node->m_transform.getWorldPosition().y, node->m_transform.getWorldPosition().z };
				float rw[3] = { node->m_transform.getForward().x, node->m_transform.getForward().y, node->m_transform.getForward().z };
				float sw[3] = { node->m_transform.getWorldScale().x, node->m_transform.getWorldScale().y, node->m_transform.getWorldScale().z };

				// Camera has reversed forward
				if (node->getType() == SceneNodeType::CAMERA)
				{
					rw[0] = -rw[0];
					rw[1] = -rw[1];
					rw[2] = -rw[2];
				}

				ImGui::TextColored(ImGuiColor::Orange, node->m_useTransform ? "Local:" : "Local: [LOCKED]");

				// Lock if transform is not used
				if (!node->m_useTransform)
					DisableStart();

				ImGui::Text("Position:"); ImGui::SameLine();
				if (ImGui::DragFloat3("Position", p, DragSpeed))
					node->m_transform.setPosition(p[0], p[1], p[2]);

				// NO LOCAL EFFECT
				//if (entity->GetType() == EntityType::LIGHT)
				//	ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

				ImGui::Text("Rotation:"); ImGui::SameLine();
				if (ImGui::DragFloat3("Rotation", r, DragSpeed))
					node->m_transform.setRotation(r[0], r[1], r[2]);

				// ~~~
				//if (entity->GetType() == EntityType::LIGHT)
				//	ImGui::PopStyleColor();


				// NO LOCAL EFFECT
				//if (entity->GetType() == EntityType::LIGHT || Entity->GetType() == EntityType::CAMERA)
				//	ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

				ImGui::Text("Scale:   "); ImGui::SameLine();
				if (ImGui::DragFloat3("Scale", s, DragSpeed))
					node->m_transform.setScale(s[0], s[1], s[2]);

				// ~~~
				//if (entity->GetType() == EntityType::LIGHT || Entity->GetType() == EntityType::CAMERA)
				//	ImGui::PopStyleColor();

				ImGui::Separator();

				// World Position //
				ImGui::TextColored(ImGuiColor::Orange, "World:");

				ImGui::Text("Position:");
				ImGui::SameLine();
				if (ImGui::DragFloat3("PositionW", pw, DragSpeed))
					node->m_transform.setWorldPosition(Vector3(pw[0], pw[1], pw[2]));

				// Lock if transform is not used
				if (!node->m_useTransform)
					DisableEnd();

				// Forward / Scale [READ ONLY]
				DisableStart();
				ImGui::Text("Forward: "); ImGui::SameLine(); ImGui::DragFloat3("RotationW", rw, DragSpeed);
				ImGui::Text("Scale:   "); ImGui::SameLine(); ImGui::DragFloat3("ScaleW", sw, DragSpeed);
				DisableEnd();
			}

			ImGui::PopItemWidth();
		}
	}
	void Inspector::DrawMaterial()
	{
		bool multiSelect = Editor.m_selectedNodes.size() > 1;

		if(multiSelect)
		{
			ImGui::TreeNodeEx("Material: Undefined", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen);
			return;
		}

		// First Node
		SceneNode* node = Assets.getSceneNode(Editor.m_selectedNodes[0]);
		SceneNodeType _type = node->getType();

		// Entity only section
		if (_type != SceneNodeType::ENTITY)
			return;

		Entity* _entity = node->getEntity();
		Material* _material = Assets.getMaterial(_entity->getMaterial());
		ShaderProgram* _shaderProgram = _material->getProgram(ShaderMaterialType::CORE);

		std::string tabName = "Material: " + _material->m_name;
		if (ImGui::TreeNodeEx(tabName.c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Section
			ImGui::Text("Textures");
			ImGui::Separator();

			// Textures

			ImGui::Checkbox("Shared Textures", &_material->m_sharedTextures);

			// Check Shader Target Textures
			for (const auto& target : _shaderProgram->m_targetLevels)
			{
				// Name
				ImGui::Text(target.first.c_str());
				ImGui::SameLine();
				// Level
				TextureLevel level = target.second;
				// Texture Index
				TextureIndex index = 0;

				// Find texture
				//
				// Shared Material Textures
				if (_material->m_sharedTextures)
					index = _material->m_textures[level];
				// Entity Texture
				else
					index = _entity->m_textures[level];

				// Check Texture Type
				BaseTexture* texture = Assets.getBaseTexture(index);
				if (texture)
				{
					TextureType type = texture->getType();

					if (type == TextureType::TEX_2D)
					{
						// Display
						if (texture)
							ImGui::Image((void*)(intptr_t)texture->getID(), ImVec2(ImageSize, ImageSize), ImVec2(0, 1), ImVec2(1, 0));
						else
							ImGui::Image((void*)(intptr_t)GlobalAssets.get_Tex2DNullImageCheckerboard()->getID(), ImVec2(ImageSize, ImageSize), ImVec2(0, 1), ImVec2(1, 0));
					}
					// Different format
					else
					{
						ImGui::Image((void*)(intptr_t)GlobalAssets.get_Tex2DNullImageQuestionMarkCube()->getID(), ImVec2(ImageSize, ImageSize), ImVec2(0, 1), ImVec2(1, 0));
					}
				}
				// No Texture
				else
				{
					ImGui::Image((void*)(intptr_t)GlobalAssets.get_Tex2DNullImageCheckerboard()->getID(), ImVec2(ImageSize, ImageSize), ImVec2(0, 1), ImVec2(1, 0));
				}
			}

			// Section
			ImGui::Separator();
			ImGui::Text("Uniforms");
			ImGui::Separator();

			// Uniform Values
			for (auto& uniformPair : _shaderProgram->m_uniformStorage)
			{
				switch (uniformPair.second.m_data.type)
				{
				//
				case UniformType::FLOAT:
				{
					float value = *uniformPair.second.m_data.getData<float>();
					if (ImGui::DragFloat(uniformPair.first.c_str(), &value))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::FLOAT_VEC2:
				{
					Vector2 value = *uniformPair.second.m_data.getData<Vector2>();
					if (ImGui::DragFloat2(uniformPair.first.c_str(), value.GetStartPointer()))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::FLOAT_VEC3:
				{
					Vector3 value = *uniformPair.second.m_data.getData<Vector3>();
					if (ImGui::DragFloat3(uniformPair.first.c_str(), value.GetStartPointer()))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::FLOAT_VEC4:
				{
					Vector4 value = *uniformPair.second.m_data.getData<Vector4>();
					if (ImGui::DragFloat4(uniformPair.first.c_str(), value.GetStartPointer()))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::DOUBLE:
				{
					double value = *uniformPair.second.m_data.getData<double>();
					if (ImGui::DragScalar(uniformPair.first.c_str(), ImGuiDataType_Double, &value, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::DOUBLE_VEC2:
				{
					Vector2d value = *uniformPair.second.m_data.getData<Vector2d>();
					if (ImGui::DragScalarN(uniformPair.first.c_str(), ImGuiDataType_Double, &value, 2, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::DOUBLE_VEC3:
				{
					Vector3d value = *uniformPair.second.m_data.getData<Vector3d>();
					if (ImGui::DragScalarN(uniformPair.first.c_str(), ImGuiDataType_Double, &value, 3, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::DOUBLE_VEC4:
				{
					Vector4d value = *uniformPair.second.m_data.getData<Vector4d>();
					if (ImGui::DragScalarN(uniformPair.first.c_str(), ImGuiDataType_Double, &value, 4, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::INT:
				{
					int value = *uniformPair.second.m_data.getData<int>();
					if (ImGui::DragInt(uniformPair.first.c_str(), &value))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::INT_VEC2:
				{
					Vector2i value = *uniformPair.second.m_data.getData<Vector2i>();
					if (ImGui::DragInt2(uniformPair.first.c_str(), value.GetStartPointer()))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::INT_VEC3:
				{
					Vector3i value = *uniformPair.second.m_data.getData<Vector3i>();
					if (ImGui::DragInt3(uniformPair.first.c_str(), value.GetStartPointer()))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::INT_VEC4:
				{
					Vector4i value = *uniformPair.second.m_data.getData<Vector4i>();
					if (ImGui::DragInt4(uniformPair.first.c_str(), value.GetStartPointer()))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::UNSIGNED_INT:
				{
					uint32_t value = *uniformPair.second.m_data.getData<uint32_t>();
					if (ImGui::DragScalar(uniformPair.first.c_str(), ImGuiDataType_U32, &value, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::UNSIGNED_INT_VEC2:
				{
					Vector2ui value = *uniformPair.second.m_data.getData<Vector2ui>();
					if (ImGui::DragScalarN(uniformPair.first.c_str(), ImGuiDataType_U32, &value, 2, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::UNSIGNED_INT_VEC3:
				{
					Vector3ui value = *uniformPair.second.m_data.getData<Vector3ui>();
					if (ImGui::DragScalarN(uniformPair.first.c_str(), ImGuiDataType_U32, &value, 3, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::UNSIGNED_INT_VEC4:
				{
					Vector4ui value = *uniformPair.second.m_data.getData<Vector4ui>();
					if (ImGui::DragScalarN(uniformPair.first.c_str(), ImGuiDataType_U32, &value, 4, 1.0f))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::BOOL:
				{
					bool value = *uniformPair.second.m_data.getData<bool>();
					if (ImGui::Checkbox(uniformPair.first.c_str(), &value))
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::BOOL_VEC2:
				{
					Vector2b value = *uniformPair.second.m_data.getData<Vector2b>();

					ImGui::Text(uniformPair.first.c_str()); ImGui::SameLine();

					bool change = ImGui::Checkbox("##1", &value.x); ImGui::SameLine();
					change |= ImGui::Checkbox("##2", &value.y);

					if (change)
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::BOOL_VEC3:
				{
					Vector3b value = *uniformPair.second.m_data.getData<Vector3b>();

					ImGui::Text(uniformPair.first.c_str()); ImGui::SameLine();

					bool change = ImGui::Checkbox("##1", &value.x); ImGui::SameLine();
					change |= ImGui::Checkbox("##2", &value.y); ImGui::SameLine();
					change |= ImGui::Checkbox("##3", &value.z);

					if (change)
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				case UniformType::BOOL_VEC4:
				{
					Vector4b value = *uniformPair.second.m_data.getData<Vector4b>();

					ImGui::Text(uniformPair.first.c_str()); ImGui::SameLine();

					bool change = ImGui::Checkbox("##1", &value.x); ImGui::SameLine();
					change |= ImGui::Checkbox("##2", &value.y); ImGui::SameLine();
					change |= ImGui::Checkbox("##3", &value.z); ImGui::SameLine();
					change |= ImGui::Checkbox("##4", &value.w);

					if (change)
					{
						_material->sendUniform(uniformPair.first, value);
					}
					break;
				}
				//
				}
			}
		}
	}
}
#endif