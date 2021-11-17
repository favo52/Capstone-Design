#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace Chesster
{
	template<typename Resource, typename Identifier>
	class ResourceHolder
	{
	public:
		void Load(Identifier id, const std::string& filename);

		template <typename Parameter>
		void Load(Identifier id, const std::string& filename, const Parameter& secondParam);

		Resource& Get(Identifier id);
		const Resource& Get(Identifier id) const;

	private:
		void InsertResource(Identifier id, std::unique_ptr<Resource> resource);

	private:
		std::map<Identifier, std::unique_ptr<Resource>> m_ResourceMap;
	};

	/**************************************************************************************
				Implementation
	**************************************************************************************/

	template<typename Resource, typename Identifier>
	void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename)
	{
		// Create and load resource
		std::unique_ptr<Resource> resource(new Resource());
		if (!resource->loadFromFile(filename))
			throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

		// If loading successful, insert resource to map
		InsertResource(id, std::move(resource));
	}

	template <typename Resource, typename Identifier>
	template <typename Parameter>
	void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename, const Parameter& secondParam)
	{
		// Create and load resource
		std::unique_ptr<Resource> resource(new Resource());
		if (!resource->loadFromFile(filename, secondParam))
			throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

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
	inline const Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id) const
	{
		auto found = m_ResourceMap.find(id);
		assert(found != m_ResourceMap.end());

		return *found->second;
	}

	template<typename Resource, typename Identifier>
	inline void ResourceHolder<Resource, Identifier>::InsertResource(Identifier id, std::unique_ptr<Resource> resource)
	{
		// Insert and check success
		auto inserted = m_ResourceMap.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}
}
