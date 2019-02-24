// Copyright (c) 2018 Emmanuel Lajeunesse
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
		GLuint		 m_id = -1;
		bool		 m_loaded = false;
		int			 m_width;
		int			 m_height;
		int			 m_channels;
		Texture_Type m_type;
		Wrap_Mode	 m_wrapMode;
		Filter_Mode_Min  m_minFilter;
		Filter_Mode_Mag  m_magFilter;
		Color4F		 m_borderColor = Color4F(0, 0, 0, 1);

		void updateParameters();

	public:
		BaseTexture(const BaseTexture&) = delete;
		BaseTexture(
			Texture_Type Type,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR
		);
		virtual ~BaseTexture();

		virtual void Bind();
		virtual void Unbind();

		void setWrapMode(Wrap_Mode W);
		void setFilterMode(Filter_Mode_Min Min, Filter_Mode_Mag Mag);
		// only works if min filter is [clamp to border]
		void setBorderColor(Color4F color);

		inline GLuint GetID(void) const
		{
			return m_id;
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
		inline Wrap_Mode GetWrapMode(void) const
		{
			return m_wrapMode;
		}
		inline Filter_Mode_Min GetFilterModeMin(void) const
		{
			return m_minFilter;
		}
		inline Filter_Mode_Mag GetFilterModeMag(void) const
		{
			return m_magFilter;
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
	protected:
		Format_Type m_format;
	public:
		RenderTexture(const RenderTexture&) = delete;
		RenderTexture(
			int Width, int Height,
			Format_Type Format = Format_Type::RGBA,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR
		);
	};

	class Texture : public BaseTexture
	{
		friend class RenderManager;
	protected:
		UCHAR*		m_image = nullptr;

		Texture(
			const std::string& filePath,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR,
			bool InvertY = true
		);
		Texture(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR
		);
		// Database
		static Database<Texture> m_database;
	public:
		static Texture* Create(
			const std::string& name,
			const std::string& filePath,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR,
			bool InvertY = true
		);
		static Texture* Create(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR
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
			Wrap_Mode WrapMode = Wrap_Mode::CLAMP_STRETCH,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR,
			bool InvertY = true
		);
		// Database
		static Database<Cubemap> m_database;
	public:
		static Cubemap* Create(
			const std::string& name,
			const std::string& filePath1,
			const std::string& filePath2,
			const std::string& filePath3,
			const std::string& filePath4,
			const std::string& filePath5,
			const std::string& filePath6,
			Wrap_Mode WrapMode = Wrap_Mode::CLAMP_STRETCH,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR,
			bool InvertY = true
		);
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

