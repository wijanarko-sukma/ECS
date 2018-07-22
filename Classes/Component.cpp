/*
 *  Component.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "Component.h"

namespace MaleoECS
{
	Component::Component (bool active) :
			_isActive(active),
			_isInitialized(false),
			_componentType(0)
	{
	}

	Component::~Component ()
	{
	}
    
    bool Component::init ()
    {
        _isInitialized = true;
        return _isInitialized;
    }
    
    void Component::shutDown ()
    {
        _isActive = false;
    }
    
    void Component::copy (MaleoECS::Component * component)
    {
        _isActive = component->isActive();
    }

	void Component::copyTo (MaleoECS::Component * component)
	{
		component->setActive(_isActive);
	}

	bool Component::isActive () const
	{
		return _isActive;
	}
    
    void Component::setActive (bool active)
    {
        _isActive = active;
    }

	bool Component::isInitialized () const
	{
		return _isInitialized;
	}

	void Component::setInitialized (bool initialized)
	{
		_isInitialized = initialized;
	}

	uint64_t Component::getComponentType () const
	{
		return _componentType;
	}

	void Component::setComponentType (uint64_t componentType)
	{
		_componentType = componentType;
	}
}
