/*
 *  System.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "System.h"
#include "Component.h"

namespace MaleoECS
{
	System::System (bool debug) :
        _isDebugable(debug),
		_isEnabled(false),
		_flags(MaleoECS::SystemFlags::Normal),
		_updateOrder(0),
		_requiredComponentTypes(0),
		_requiredAtLeastOneComponentTypes(0),
        _forbiddenComponentTypes (0),
		_entityManager(nullptr),
        _callbackCollection (nullptr)
		//_supportedMessages(nullptr)
	{
		//_entityIds = new std::map<uint64_t, MaleoECS::Entity *>();
	}

	System::~System ()
	{
        _entityIds.clear();
        _supportedMessages.clear();
        if (_callbackCollection) {
            _callbackCollection->clear();
            delete _callbackCollection;
        }
        _callbackCollection = nullptr;
        _entityManager = nullptr;
	}

    void System::construct (int updateOrder, std::vector<uint64_t> * requiredComponentTypeIds, std::map<std::string, MaleoECS::MessageCallback> * callbackCollection, std::vector<uint64_t> * optionalButOneRequiredComponentTypeIds, std::vector<uint64_t> * forbiddenComponentTypeIds, SystemFlags flags)
	{
		_flags = flags;
		_updateOrder = updateOrder;
		_isEnabled = true;
		for(std::vector<uint64_t>::iterator it = requiredComponentTypeIds->begin(); it != requiredComponentTypeIds->end(); ++it) {
			_requiredComponentTypes |= MaleoECS::ComponentTypeIdHelper::getBit(*it);
		}

		if (optionalButOneRequiredComponentTypeIds == nullptr)	{
			_requiredAtLeastOneComponentTypes = MaleoECS::ComponentTypeIdHelper::AllMask;
		} else {
			for (auto iter = optionalButOneRequiredComponentTypeIds->begin(); iter != optionalButOneRequiredComponentTypeIds->end(); ++iter) {
				_requiredAtLeastOneComponentTypes |= MaleoECS::ComponentTypeIdHelper::getBit(*iter);
			}
		}
        
        if (forbiddenComponentTypeIds == nullptr) {
            _forbiddenComponentTypes = 0;
        } else {
            for (auto iter = forbiddenComponentTypeIds->begin(); iter != forbiddenComponentTypeIds->end(); ++iter) {
                _forbiddenComponentTypes |= MaleoECS::ComponentTypeIdHelper::getBit(*iter);
            }
        }
        
        if (callbackCollection) {
            _callbackCollection = callbackCollection;
            
            for (auto iter = _callbackCollection->begin(); iter != _callbackCollection->end(); ++iter) {
                _supportedMessages.push_back(iter->first);
            }
        }
	}

	bool System::initialize (MaleoECS::EntityManager * entityMgr)
	{
		this->setEntityManager(entityMgr);
		return this->onInitialized();
	}
    
    MaleoECS::EntityManager * System::getEntityManager ()
    {
        return _entityManager;
    }
    
    void System::setEntityManager (MaleoECS::EntityManager *entityMgr)
    {
        _entityManager = entityMgr;
    }

	MaleoECS::SystemFlags System::getFlags () const
	{
		return _flags;
	}

	void System::setFlags (MaleoECS::SystemFlags flags)
	{
		_flags = flags;
	}
    
    bool System::isDebugable () const
    {
        return _isDebugable;
    }

	bool System::isEnabled () const
	{
		return _isEnabled;
	}

	void System::setEnabled (bool enabled)
	{
		if(_isEnabled != enabled) {
			_isEnabled = enabled;
			this->onEnabledChanged();
		}
	}

	int System::getUpdateOrder () const
	{
		return _updateOrder;
	}

	void System::setUpdateOrder (int updateOrder)
	{
		_updateOrder = updateOrder;
	}

	const std::vector<std::string> & System::getSupportedMessages () const
	{
		return _supportedMessages;
	}

	void System::setSupportedMessages (const std::vector<std::string> & supportedMsgs)
	{
		_supportedMessages = supportedMsgs;
	}

	uint64_t System::getRequiredComponentTypesBitField () const
	{
		return _requiredComponentTypes;
	}
/*
	void System::setRequiredComponentTypesBitField (uint64_t requiredComponentTypes)
	{
		_requiredComponentTypes = requiredComponentTypes;
	}
*/
	uint64_t System::getRequiredAtLeastOneComponentTypesBitField () const
	{
		return _requiredAtLeastOneComponentTypes;
	}
/*
	void System::setRequiredAtLeastOneComponentTypesBitField (uint64_t requiredAtLeastOne)
	{
		_requiredAtLeastOneComponentTypes = requiredAtLeastOne;
	}
*/
    uint64_t System::getForbiddenComponentTypesBitField () const
    {
        return _forbiddenComponentTypes;
    }
    
	void System::addEntity (MaleoECS::Entity * entity)
	{
        if (_entityIds.count(entity->getUniqueId()) == 1)
            return;
		_entityIds.insert(std::pair<uint64_t, MaleoECS::Entity*>(entity->getUniqueId(), entity));

		this->onEntityAdded(entity, entity->getUniqueId());
	}

	void System::removeEntity (MaleoECS::Entity * entity)
	{
		this->onEntityRemoved(entity, entity->getUniqueId());

		_entityIds.erase(entity->getUniqueId());
	}

	bool System::sendMessage (MaleoECS::Entity * target, const std::string & messageId, void * data, void * sender)
	{
		bool success = false;

		if (_isEnabled && _callbackCollection->count(messageId) > 0) {
            success = _callbackCollection->at(messageId)(target, messageId, data, sender);
            //success = this->onHandleMessage(target, messageId, data, sender);
		}

		return success;
	}
}
