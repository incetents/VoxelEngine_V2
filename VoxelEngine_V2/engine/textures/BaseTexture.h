// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../rendering/Graphics.h"
#include "../utilities/Types.h"
#include "../utilities/Macros.h"
#include "../math/Color.h"

#include <map>
#include <string>

namespace Vxl
{
	class RenderBuffer;
	
	class BaseTexture
	{
	protected:
		// Data //
		TextureID	 m_id = -1;
		bool		 m_loaded = false;
		bool		 m_mipMapping = false;
		int			 m_width;
		int			 m_height;
		int			 m_channelCount;
		Color4F		 m_borderColor = Color4F(0, 0, 0, 1);
		TextureType			m_type;
		TextureWrapping		m_wrapMode;
		TextureFilter		m_filterMode;
		TextureFormat		m_formatType;
		TextureChannelType	m_channelType;
		TexturePixelType	m_pixelType;
		AnisotropicMode		m_anisotropicMode;

		// Creation/Deletion
		void load();
		void unload();

		// Storage
		virtual void createStorage(); // Create space for texture [immutable, cannot be resized]
		virtual void setStorage(const void* pixels);

		// Utility
		void updateParameters();
		void updateMipmapping();
		void flipImageVertically(uint8_t* imagePixels);

	public:
		BaseTexture(const BaseTexture&) = delete;
		BaseTexture(
			TextureType Type,
			TextureWrapping WrapMode = TextureWrapping::REPEAT,
			TextureFilter FilterMode = TextureFilter::LINEAR,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TextureChannelType ChannelType = TextureChannelType::RGBA,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode AnisotropicMode = AnisotropicMode::NONE,
			bool MipMapping = false
		);
		virtual ~BaseTexture();

		void bind(TextureLevel layer) const;
		void bind() const;
		void unbind() const;

		//void copy(const BaseTexture& _texture);
		//void copy(const RenderBuffer& _texture);

		void setWrapMode(TextureWrapping W);
		void setFilterMode(TextureFilter filter);
		void setAnistropicMode(AnisotropicMode Anso);
		// only works if min filter is [clamp to border]
		void setBorderColor(Color4F color);

		void setGLName(const std::string& glName);

		inline TextureID	getID(void) const
		{
			return m_id;
		}
		inline bool			isMipMapping(void) const
		{
			return m_mipMapping;
		}
		inline int			getWidth(void) const
		{
			return m_width;
		}
		inline int			getHeight(void) const
		{
			return m_height;
		}
		inline int			getChannelCount(void) const
		{
			return m_channelCount;
		}
		inline Color4F		getBorderColor(void) const
		{
			return m_borderColor;
		}
		inline TextureType			getType(void) const
		{
			return m_type;
		}
		inline TextureWrapping		getWrapMode(void) const
		{
			return m_wrapMode;
		}
		inline TextureFilter		getFilterMode(void) const
		{
			return m_filterMode;
		}
		inline TextureFormat		getFormatType(void) const
		{
			return m_formatType;
		}
		inline TextureChannelType	getChannelType(void) const
		{
			return m_channelType;
		}
		inline TexturePixelType		getPixelType(void) const
		{
			return m_pixelType;
		}

		virtual bool isLoaded(void) const
		{
			return true;
		}
	};
}