/*
 *  EntityTemplate.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "EntityTemplate.h"
#include <algorithm>
namespace MaleoECS
{
	EntityTemplate::EntityTemplate (const std::string & name, uint64_t type) :
			_name(name),
			_templateType(type)
	{
		//_components = new std::vector<MaleoECS::Component *>();
	}

	EntityTemplate::~EntityTemplate ()
	{
        for (auto iter = _components.begin(); iter != _components.end(); ++iter) {
            auto component = (*iter);
            delete component;
            component = nullptr;
        }
        _components.clear();
        //delete _components;
        //_components = nullptr;
	}

	bool EntityTemplate::addComponent (MaleoECS::Component * component)
	{
		bool success = false;

		if(std::find(_components.begin(), _components.end(), component) == _components.end()) {
			_components.push_back(component);
			success = true;
		}

		return success;
	}

	const std::string & EntityTemplate::getTemplateKey () const
	{
		return _name;
	}

	uint64_t EntityTemplate::getTemplateType () const
	{
		return _templateType;
	}

	void EntityTemplate::setTemplateType (uint64_t templateType)
	{
		_templateType = templateType;
	}

	const std::vector<MaleoECS::Component *> & EntityTemplate::getComponents () const
	{
		return _components;
	}
}
