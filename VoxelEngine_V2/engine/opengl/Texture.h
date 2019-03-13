// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Enums.h"
#include "../utilities/Database.h"
#include "../math/Color.h"
#include <SOIL/SOIL.h>

#include <string>

namespace Vxl
{
	class Color3F;

	void FlipTextureY(UCHAR* array, GLuint width, GLuint height, GLuint channels);

	class BaseTexture
	{
	protected:
		// Data //
		GLuint		 m_id = -1;
		bool		 m_loaded = false;
		bool		 m_useMipMapping = false;
		int			 m_width;
		int			 m_height;
		int			 m_channels;
		Texture_Type m_type;
		Wrap_Mode	 m_wrapMode;
		Filter_Min  m_minFilter;
		Filter_Mag  m_magFilter;
		Format_Type  m_formatType;
		Channel_Type m_channelType;
		Data_Type	 m_dataType;
		Color4F		 m_borderColor = Color4F(0, 0, 0, 1);

		// Tracker //
		static Active_Texture m_activeSlot;
		static std::unordered_map<Texture_Type, UINT> m_activeTextures;

		void updateParameters();
		void updateTexImage2D(const GLvoid* pixels = nullptr);
		void updateTexImageCubemap(int side = 0, const GLvoid* pixels = nullptr);

	public:
		BaseTexture(const BaseTexture&) = delete;
		BaseTexture(
			Texture_Type Type,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type FormatType = Format_Type::RGBA,
			Channel_Type ChannelType = Channel_Type::RGBA,
			Data_Type DataType = Data_Type::UNSIGNED_BYTE
		);
		virtual ~BaseTexture();

		void Bind(Active_Texture layer) const;
		void Bind() const;
		void Unbind() const;

		static void ClearTrackingData()
		{
			m_activeSlot = Active_Texture::LEVEL0;
			m_activeTextures.clear();
		}

		void setWrapMode(Wrap_Mode W);
		void setFilterMode(Filter_Min Min, Filter_Mag Mag);
		// only works if min filter is [clamp to border]
		void setBorderColor(Color4F color);

		inline GLuint GetID(void) const
		{
			return m_id;
		}
		inline bool IsMipMapping(void) const
		{
			return m_useMipMapping;
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
			return m_channels;
		}
		inline Texture_Type GetType(void) const
		{
			return m_type;
		}
		inline Wrap_Mode GetWrapMode(void) const
		{
			return m_wrapMode;
		}
		inline Filter_Min GetFilterModeMin(void) const
		{
			return m_minFilter;
		}
		inline Filter_Mag GetFilterModeMag(void) const
		{
			return m_magFilter;
		}
		inline Format_Type GetFormatType(void) const
		{
			return m_formatType;
		}
		inline Channel_Type GetChannelType(void) const
		{
			return m_channelType;
		}
		inline Data_Type GetDataType(void) const
		{
			return m_dataType;
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

	class RenderTexture : public BaseTexture
	{
	public:
		RenderTexture(const RenderTexture&) = delete;
		RenderTexture(
			int Width, int Height,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type FormatType = Format_Type::RGBA,
			Channel_Type ChannelType = Channel_Type::RGBA,
			Data_Type DataType = Data_Type::UNSIGNED_BYTE
		);
	};

	class Texture : public BaseTexture
	{
		friend class Loader;
		friend class RenderManager;
	protected:
		UCHAR*		m_image = nullptr;

		Texture(
			const std::string& filePath,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA,
			Data_Type		DataType = Data_Type::UNSIGNED_BYTE
		);
		Texture(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA,
			Data_Type		DataType = Data_Type::UNSIGNED_BYTE
		);
		// Database
		static Database<Texture> m_database;
	public:
		// Create custom texture
		static Texture* CreateCustom(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA,
			Data_Type		DataType = Data_Type::UNSIGNED_BYTE
		);

		// Database Getter
		static Texture* Get(const std::string& name)
		{
			return m_database.Get(name);
		}

		Texture(const Texture&) = delete;
		~Texture();

		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
	};

	class Cubemap : public BaseTexture
	{
		friend class Loader;
		friend class RenderManager;
	protected:
		UCHAR**		m_image = new UCHAR*[6];

		Cubemap(
			const std::string& filePath1,
			const std::string& filePath2,
			const std::string& filePath3,
			const std::string& filePath4,
			const std::string& filePath5,
			const std::string& filePath6,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA,
			Data_Type		DataType = Data_Type::UNSIGNED_BYTE
		);
		// Database
		static Database<Cubemap> m_database;
	public:
		// Database Getter
		static Cubemap* Get(const std::string& name)
		{
			return m_database.Get(name);
		}

		Cubemap(const Cubemap&) = delete;
		~Cubemap();
		
		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
	};
}

