/*
 *  EntityManager.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "EntityManager.h"
#include "ECSCore.h"
#include "Component.h"
#include "MessageData.h"

namespace MaleoECS
{
    EntityManager::EntityManager (short defaultPoolSize) :
        _universe (nullptr),
        _templateManager (nullptr),
        _systemManager (nullptr),
        _isPaused (false),
        _entityPoolSize (0),
        _availableEntityCount (0),
        _removalCallback (nullptr)
    {
        this->initialize(defaultPoolSize);
    }
    
	EntityManager::EntityManager (MaleoECS::Universe * universe, MaleoECS::EntityTemplateManager * templateMgr, short defaultPoolSize) :
		_universe (universe),
        _templateManager (templateMgr),
		_systemManager (nullptr),
        _isPaused (false),
        _entityPoolSize (0),
        _availableEntityCount (0)
	{
		//_componentManagers = new std::map<uint64_t, MaleoECS::IComponentManagerBase *>();
		//_entityPool = new std::vector<MaleoECS::Entity *>();
		//_uniqueIdToEntity = new std::map<uint64_t, MaleoECS::Entity *>();
        //_entityToBeRemoved = new std::vector<uint64_t>();

		this->initialize(defaultPoolSize);
	}

	EntityManager::~EntityManager (void)
	{
        for (auto iter = _entityPool.begin(); iter != _entityPool.end(); ++iter) {
            auto entity = (*iter);
            if (_removalCallback) {
                _removalCallback(entity);
            }
            delete entity;
            entity = nullptr;
        }
		_entityPool.clear();
        
        for (auto iter = _uniqueIdToEntity.begin(); iter != _uniqueIdToEntity.end(); ++iter) {
            auto entity = iter->second;
            if (_removalCallback) {
                _removalCallback(entity);
            }
            delete entity;
            entity = nullptr;
        }
		_uniqueIdToEntity.clear();
        
        _entityToBeRemoved.clear();
		
        for (auto iter = _componentManagers.begin(); iter != _componentManagers.end(); ++iter) {
            auto componentMgr = iter->second;
            delete componentMgr;
            componentMgr = nullptr;
        }
        _componentManagers.clear();
        
        delete _templateManager;
		delete _systemManager;
        delete _universe;
        delete _messagePool;
        
        _systemManager = nullptr;
        _universe = nullptr;
        _messagePool = nullptr;
	}

	bool EntityManager::initialize (short defaultPoolSize)
	{
        if (defaultPoolSize > 0) {
            this->growEntityPool(defaultPoolSize);
        }

		return true;
	}
    
    void EntityManager::growEntityPool (short poolSize)
    {
        short oldSize = _entityPoolSize;
        short newSize = _entityPoolSize * 2;
        newSize = (newSize > poolSize) ? newSize : poolSize;
        short diffSize = newSize - oldSize;
        
        for (int index = oldSize; index < newSize; index++) {
            MaleoECS::Entity * entity = new MaleoECS::Entity();
            _entityPool.push_back(entity);
        }
        
        _entityPoolSize = newSize;
        _availableEntityCount = diffSize;
    }

	MaleoECS::IComponentManagerBase * EntityManager::getBaseComponentManager (uint64_t componentType) const
	{
		return _componentManagers.at(componentType);
	}

	void EntityManager::updateSystems (float dt)
	{
		_systemManager->updateSystems(dt);
	}
    
    void EntityManager::fixedUpdateSystems (float dt)
    {
        _systemManager->fixedUpdateSystems(dt);
    }
    
    void EntityManager::postUpdateSystems (float dt)
    {
        
    }
    
    void EntityManager::flushRemovedEntities ()
    {
        //for (auto iter=_entityToBeRemoved.begin(); iter!= _entityToBeRemoved.end(); ++iter) {
        for (int index = 0; index < _entityToBeRemoved.size(); index++) {
            uint64_t uniqueId = _entityToBeRemoved.at(index);
            MaleoECS::Entity * entity = nullptr;
            if (_uniqueIdToEntity.count(uniqueId) > 0)
                entity = _uniqueIdToEntity.at(uniqueId);
            
            if (entity)
                this->removeEntity(entity);
        }
        _entityToBeRemoved.clear();
    }
    
    void EntityManager::debugDraw ()
    {
        _systemManager->debugDraw();
    }

	bool EntityManager::sendMessage (MaleoECS::Entity * entity, const std::string & messageId, void * sender)
	{
		MaleoECS::MessageData * data = new MaleoECS::MessageData();
		return this->sendMessage(entity, messageId, data, sender);
	}

	bool EntityManager::sendMessage (MaleoECS::Entity * entity, const std::string & messageId, void * data, void * sender)
	{
		return this->sendMessage(entity->getUniqueId(), messageId, data, sender);
	}

	bool EntityManager::sendMessage (uint64_t uniqueId, const std::string & messageId, void * data, void * sender)
	{
		bool success = false;
		Entity * target = NULL;

		if (uniqueId == INVALID_ENTITY_UNIQUE_ID) {
			success = this->deliverMessage(NULL, messageId, data, sender);
		} else {
			auto iter = _uniqueIdToEntity.find(uniqueId);
			if (iter != _uniqueIdToEntity.end()) {
				target = iter->second;
				success = deliverMessage(target, messageId, data, sender);
			}
		}

		return success;
	}

	bool EntityManager::deliverMessage (MaleoECS::Entity * target, const std::string & messageId, void * data, void * sender)
	{
		bool success = _systemManager->sendMessage(target, messageId, data, sender);

		MaleoECS::MessageData * msg = (MaleoECS::MessageData *)data;
		if (!msg->isHandled()) {
			if (target) {
                msg->setHandled(true);
			}
		}

		return success;
	}

	MaleoECS::Entity * EntityManager::createEntity (const std::string & key, uint64_t ownerId)
	{
		return this->createEntity(_templateManager->getTemplate(key), ownerId);
	}

	MaleoECS::Entity * EntityManager::createEntity (MaleoECS::EntityTemplate * temp, uint64_t ownerId)
	{
		MaleoECS::Entity * entity = this->allocateEntity(_universe->getNextUniqueId(), ownerId);
		entity->setEntityType(temp->getTemplateType());
		std::vector<MaleoECS::Component *> components = temp->getComponents();
		for(auto iter = components.begin(); iter != components.end(); ++iter) {
			MaleoECS::Component * componentTemplate= *iter;
			MaleoECS::Component * component;
			MaleoECS::IComponentManagerBase * manager = this->getBaseComponentManager(componentTemplate->getComponentType());
			if(! manager->isComponentAllocated(entity)) {
				component = manager->allocateComponent(entity);
                componentTemplate->copyTo(component);
				entity->setComponentTypesBitField(entity->getComponentTypesBitField() | MaleoECS::ComponentTypeIdHelper::getBit(componentTemplate->getComponentType()));
			} else {
				component = manager->getComponent(entity);
			}
		}

		//this->addEntityToSystems(entity);

		return entity;
	}
    
    MaleoECS::Entity * EntityManager::getEntity (uint64_t uniqueId) const
    {
        if (this->isEntityExists(uniqueId))
            return _uniqueIdToEntity.at(uniqueId);
        
        return nullptr;
    }
    
    void EntityManager::registerEntityToBeRemoved (uint64_t uniqueId)
    {
        if (_uniqueIdToEntity.count(uniqueId) == 0)
            return;
        
        MaleoECS::Entity * entity = _uniqueIdToEntity.at(uniqueId);
        
        if (entity->numChildren() > 0) {
            for (int i=0; i < entity->numChildren(); i++) {
                uint64_t childId = entity->getChild(i);
                _entityToBeRemoved.push_back(childId);
            }
        }
        
        _entityToBeRemoved.push_back(uniqueId);
    }

	void EntityManager::removeEntity (MaleoECS::Entity * entity)
	{
        if (entity->getOwnerUniqueId() != _universe->getTopLevelGroupUniqueIdBase() && entity->getOwnerUniqueId() != INVALID_ENTITY_UNIQUE_ID)
            this->removeEntityFromOwner(entity);
        this->removeEntityFromSystems(entity);
        uint64_t componentsBitFlag = entity->getComponentTypesBitField();
        
        for (auto iter=_componentManagers.begin(); iter!=_componentManagers.end();++iter) {
            uint64_t componentType = iter->first;
            uint64_t componentBitFlag =  MaleoECS::ComponentTypeIdHelper::getBit(componentType);
            
            if ((componentsBitFlag & componentBitFlag) != componentBitFlag)
                continue;
            
            MaleoECS::IComponentManagerBase * manager = this->getBaseComponentManager(componentType);
            manager->restoreComponent(entity);
        }
        this->restoreEntity(entity);
	}

	void EntityManager::freeAllEntities ()
	{

	}
    
    bool EntityManager::isEntityExists (uint64_t uniqueId) const
    {
        if (_uniqueIdToEntity.count(uniqueId) == 1)
            return true;
        return false;
    }
    
    uint64_t EntityManager::getOwnerIdFrom (uint64_t uniqueId) const
    {
        return _uniqueIdToEntity.at(uniqueId)->getOwnerUniqueId();
    }

	MaleoECS::Entity * EntityManager::allocateEntity (uint64_t uniqueId, uint64_t ownerId)
	{
        if (_availableEntityCount == 0) {
            this->growEntityPool(_entityPoolSize + 1);
        }
		MaleoECS::Entity * entity = _entityPool.back();
		_entityPool.pop_back();
        _availableEntityCount--;

		entity->setUniqueId(uniqueId);
        entity->setOwnerUniqueId(ownerId);
        if (ownerId != _universe->getTopLevelGroupUniqueIdBase() && ownerId != INVALID_ENTITY_UNIQUE_ID) {
            MaleoECS::Entity * owner = _uniqueIdToEntity.at(ownerId);
            owner->registerChild(uniqueId);
        }
		_uniqueIdToEntity.insert(std::pair<uint64_t, MaleoECS::Entity *>(uniqueId, entity));
		return entity;
	}

	void EntityManager::restoreEntity (MaleoECS::Entity * entity)
	{
        _uniqueIdToEntity.erase(entity->getUniqueId());
        entity->reset();
		_entityPool.push_back(entity);
        _availableEntityCount++;
	}

	void EntityManager::addEntityToSystems (MaleoECS::Entity * entity)
	{
		for(auto iter = _systemManager->getSystems().begin(); iter != _systemManager->getSystems().end(); ++iter) {
			MaleoECS::System * system = iter->second;
			bool shouldBeInSystem = ((system->getRequiredComponentTypesBitField() & entity->getComponentTypesBitField()) == system->getRequiredComponentTypesBitField());
			shouldBeInSystem = shouldBeInSystem && ((system->getRequiredAtLeastOneComponentTypesBitField() & entity->getComponentTypesBitField()) != 0);
            shouldBeInSystem = shouldBeInSystem && ((system->getForbiddenComponentTypesBitField() & entity->getComponentTypesBitField()) == 0);
			if(shouldBeInSystem) {
				system->addEntity(entity);
			}
		}
	}
    
    void EntityManager::removeEntityFromSystems (MaleoECS::Entity * entity)
    {
        for(auto iter = _systemManager->getSystems().begin(); iter != _systemManager->getSystems().end(); ++iter) {
			MaleoECS::System * system = iter->second;
			bool shouldBeInSystem = ((system->getRequiredComponentTypesBitField() & entity->getComponentTypesBitField()) == system->getRequiredComponentTypesBitField());
			shouldBeInSystem = shouldBeInSystem && ((system->getRequiredAtLeastOneComponentTypesBitField() & entity->getComponentTypesBitField()) != 0);
            shouldBeInSystem = shouldBeInSystem && ((system->getForbiddenComponentTypesBitField() & entity->getComponentTypesBitField()) == 0);
			if(shouldBeInSystem) {
				system->removeEntity(entity);
			}
		}
    }
    
    void EntityManager::removeEntityFromOwner (MaleoECS::Entity * entity)
    {
        if (_uniqueIdToEntity.count(entity->getOwnerUniqueId()) > 0) {
            MaleoECS::Entity * owner = _uniqueIdToEntity.at(entity->getOwnerUniqueId());
            owner->unregisterChild(entity->getUniqueId());
        }
    }
    
    MaleoECS::Universe * EntityManager::getUniverse () const
    {
        return _universe;
    }
    
    void EntityManager::setUniverse (MaleoECS::Universe *universe)
    {
        _universe = universe;
    }
    
    MaleoECS::EntityTemplateManager * EntityManager::getTemplateManager () const
    {
        return _templateManager;
    }
    
    void EntityManager::setTemplateManager (MaleoECS::EntityTemplateManager *templateMgr)
    {
        _templateManager = templateMgr;
    }

	MaleoECS::SystemManager * EntityManager::getSystemManager () const
	{
		return _systemManager;
	}

	void EntityManager::setSystemManager (MaleoECS::SystemManager * mgr)
	{
		_systemManager = mgr;
	}
    
    const std::string & EntityManager::getUniverseName () const
    {
        return _universe->getName();
    }
    
    bool EntityManager::isPaused () const
    {
        return _isPaused;
    }
    
    void EntityManager::pause ()
    {
        _isPaused = true;
        for(auto iter = _systemManager->getSystems().begin(); iter != _systemManager->getSystems().end(); ++iter) {
			MaleoECS::System * system = iter->second;
			system->onPause();
		}
    }
    
    void EntityManager::resume()
    {
        _isPaused = false;
        for(auto iter = _systemManager->getSystems().begin(); iter != _systemManager->getSystems().end(); ++iter) {
			MaleoECS::System * system = iter->second;
			system->onResume();
		}
    }
    
    void EntityManager::triggerPauseOrResume ()
    {
        _isPaused = !_isPaused;
    }
    
    void EntityManager::setRemovalCallback (const std::function<void (MaleoECS::Entity *)> &callback)
    {
        _removalCallback = callback;
    }
}
