/*
 *  EntityTemplateManager.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "EntityTemplateManager.h"


namespace MaleoECS
{
	EntityTemplateManager::EntityTemplateManager ()
	{
		//_entityTemplates = new std::map<std::string, EntityTemplate *>();
	}

	EntityTemplateManager::~EntityTemplateManager ()
	{
        for (auto iter = _entityTemplates.begin(); iter !=_entityTemplates.end(); ++iter) {
            auto entityTemplate = iter->second;
            delete entityTemplate;
            entityTemplate = nullptr;
        }
        _entityTemplates.clear();
        //delete _entityTemplates;
        //_entityTemplates = nullptr;
	}

	bool EntityTemplateManager::registerTemplate (MaleoECS::EntityTemplate * entity)
	{
		if (this->isTemplateExist(entity->getTemplateKey()))
			return false;

		_entityTemplates.insert(std::pair<std::string, MaleoECS::EntityTemplate *>(entity->getTemplateKey(), entity));
		return true;
	}

	bool EntityTemplateManager::isTemplateExist (const std::string & key) const
	{
		if (_entityTemplates.count(key) == 1)
			return true;

		return false;
	}

	MaleoECS::EntityTemplate * EntityTemplateManager::getTemplate (const std::string & key) const
	{
		if (this->isTemplateExist(key))
			return _entityTemplates.at(key);

		return nullptr;
	}
}
