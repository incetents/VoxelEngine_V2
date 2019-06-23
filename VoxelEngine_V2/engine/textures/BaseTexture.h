// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../opengl/Enums.h"
#include "../utilities/Asset.h"
#include "../math/Color.h"

#include <map>
#include <string>

namespace Vxl
{
	void FlipTextureY(UCHAR* array, GLuint width, GLuint height, GLuint channels);

	enum class Anisotropic_Mode : GLubyte
	{
		NONE = 1,
		LOW = 4,
		MEDIUM = 8,
		HIGH = 16
	};

	class BaseTexture
	{
	protected:
		// Data //
		GLuint		 m_id = -1;
		bool		 m_loaded = false;
		bool		 m_mipMapping = false;
		int			 m_width;
		int			 m_height;
		int			 m_channelCount;
		Texture_Type m_type;
		Wrap_Mode	 m_wrapMode;
		Filter_Mode  m_filterMode;
		Format_Type  m_formatType;
		Channel_Type m_channelType;
		Pixel_Type	 m_pixelType;
		Anisotropic_Mode m_anisotropicMode;
		Color4F		 m_borderColor = Color4F(0, 0, 0, 1);

		// Tracker //
		static std::map<Texture_Type, UINT> m_activeTextures;

	public:
		void updateParameters();
		void updateStorage();
		void updateStorage(const GLvoid* pixels);
		void updateTexImageCubemap(unsigned int side = 0, const GLvoid* pixels = nullptr);
		void updateMipmapping();

	public:
		BaseTexture(const BaseTexture&) = delete;
		BaseTexture(
			Texture_Type Type,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode FilterMode = Filter_Mode::LINEAR,
			Format_Type FormatType = Format_Type::RGBA8,
			Channel_Type ChannelType = Channel_Type::RGBA,
			Pixel_Type PixelType = Pixel_Type::UNSIGNED_BYTE,
			Anisotropic_Mode AnisotropicMode = Anisotropic_Mode::NONE,
			bool MipMapping = false
		);
		virtual ~BaseTexture();

		void Bind(Active_Texture layer) const;
		void Bind() const;
		void Unbind() const;

		void setWrapMode(Wrap_Mode W);
		void setFilterMode(Filter_Mode filter);
		void setAnistropicMode(Anisotropic_Mode Anso);
		// only works if min filter is [clamp to border]
		void setBorderColor(Color4F color);

		inline GLuint GetID(void) const
		{
			return m_id;
		}
		inline bool IsMipMapping(void) const
		{
			return m_mipMapping;
		}
		inline int GetWidth(void) const
		{
			return m_width;
		}
		inline int GetHeight(void) const
		{
			return m_width;
		}
		inline int GetChannelCount(void) const
		{
			return m_channelCount;
		}
		inline Texture_Type GetType(void) const
		{
			return m_type;
		}
		inline Wrap_Mode GetWrapMode(void) const
		{
			return m_wrapMode;
		}
		inline Filter_Mode GetFilterMode(void) const
		{
			return m_filterMode;
		}
		inline Format_Type GetFormatType(void) const
		{
			return m_formatType;
		}
		inline Channel_Type GetChannelType(void) const
		{
			return m_channelType;
		}
		inline Pixel_Type GetPixelType(void) const
		{
			return m_pixelType;
		}
		inline Color4F GetBorderColor(void) const
		{
			return m_borderColor;
		}

		virtual bool IsLoaded(void) const
		{
			return true;
		}
	};
}