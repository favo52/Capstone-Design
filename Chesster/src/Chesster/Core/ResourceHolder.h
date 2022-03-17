#pragma once

#include "pch.h"
#include "Chesster/Renderer/Texture.h"

namespace Chesster
{
	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="Resource"></typeparam>
	/// <typeparam name="Identifier"></typeparam>
	template<typename Resource, typename Identifier>
	class ResourceHolder
	{
	public:
		void Load(Identifier id, const std::string& filename);

		template<typename Parameter>
		void Load(Identifier id, const std::string& filename, const Parameter& secondParam);

		Resource& Get(Identifier id);
		const Resource& Get(Identifier id) const;

	private:
		void InsertResource(Identifier id, std::unique_ptr<Resource> resource);

	private:
		std::unordered_map<Identifier, std::unique_ptr<Resource>> m_ResourceMap;
	};

	/*****************************************************
	*	TextureHolder Implementation
	*****************************************************/

	template<typename Resource, typename Identifier>
	void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename)
	{
		std::unique_ptr<Resource> resource(new Resource());
		if (!resource->LoadFromFile(filename))
			throw std::runtime_error("ResourceHolder::Load - Failed to load " + filename);

		// If loading successful, insert resource to map
		InsertResource(id, std::move(resource));
	}

	template<typename Resource, typename Identifier>
	template<typename Parameter>
	void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename, const Parameter& secondParam)
	{
		std::unique_ptr<Resource> resource(new Resource());
		if (!resource->LoadFromFile(filename, secondParam))
			throw std::runtime_error("ResourceHolder::Load - Failed to load " + filename);

		// If loading successful, insert resource to map
		InsertResource(id, std::move(resource));
	}

	template<typename Resource, typename Identifier>
	Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id)
	{
		auto found = m_ResourceMap.find(id);
		assert(found != m_ResourceMap.end());

		return *found->second;
	}

	template<typename Resource, typename Identifier>
	const Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id) const
	{
		auto found = m_ResourceMap.find(id);
		assert(found != m_ResourceMap.end());

		return *found->second;
	}

	template<typename Resource, typename Identifier>
	void ResourceHolder<Resource, Identifier>::InsertResource(Identifier id, std::unique_ptr<Resource> resource)
	{
		// Insert and check success
		auto inserted = m_ResourceMap.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	enum class TextureID
	{
		GroupLogo,
		ChessterLogo,
		Pieces
	};

	enum class FontID
	{
		OpenSans,
		OpenSans_Bold,
		AbsEmpire
	};

	// A few type definitions
	typedef ResourceHolder<Texture, TextureID> TextureHolder;
	typedef ResourceHolder<Font, FontID> FontHolder;
}
