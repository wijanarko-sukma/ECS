/*
 *  EntityManager.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_ENTITY_MANAGER_H_
#define _ECS_ENTITY_MANAGER_H_

#include <iostream>
#include <stdint.h>
#include <map>
#include "ECSCore.h"
#include "Universe.h"
#include "SystemManager.h"
#include "ComponentManager.h"
#include "EntityTemplateManager.h"
#include "Entity.h"
#include "EntityTemplate.h"
#include "MessagePool.h"

namespace MaleoECS
{
	class EntityManager
	{
	public:
        EntityManager (short defaultPoolSize);
		EntityManager (MaleoECS::Universe * universe, MaleoECS::EntityTemplateManager * templateMgr, short defaultPoolSize);
		virtual ~EntityManager (void);

		bool initialize (short defaultPoolSize);

		template <class T>
		void addComponentType (uint64_t componentType, short defaultSize)
		{
			_componentManagers.insert(std::make_pair(componentType, this->createComponentManager<T>(componentType, defaultSize)));
		}
        
        template <class T>
        void createMessagePool (short defaultSize)
        {
            _messagePool = new MaleoECS::MessagePool<T>(0, defaultSize);
        }

		template <class T>
		MaleoECS::ComponentManager<T> * getComponentManager (uint64_t componentType) const
		{
			return (MaleoECS::ComponentManager<T> *)_componentManagers.at(componentType);
		}
        
        template <class T>
        MaleoECS::MessagePool<T> * getMessagePool () const
        {
            return (MaleoECS::MessagePool<T> *)_messagePool;
        }

		MaleoECS::IComponentManagerBase * getBaseComponentManager (uint64_t componentType) const;

		template <class T>
		T * getComponent (MaleoECS::Entity * entity, uint64_t componentType) const
		{
			uint64_t uniqueId = entity->getUniqueId();
			return this->getComponent<T>(uniqueId, componentType);
		}

		template <class T>
		T * getComponent (uint64_t uniqueId, uint64_t componentType) const
		{
			T * component = this->getComponentManager<T>(componentType)->getComponent(uniqueId);
			return component;
		}

        MaleoECS::Entity * createEntity (const std::string & key, uint64_t ownerId);
        MaleoECS::Entity * createEntity (MaleoECS::EntityTemplate * temp, uint64_t ownerId);
        MaleoECS::Entity * getEntity (uint64_t uniqueId) const;
        void registerEntityToBeRemoved (uint64_t uniqueId);
		void freeAllEntities ();
        bool isEntityExists (uint64_t uniqueId) const;
        uint64_t getOwnerIdFrom (uint64_t uniqueId) const;
		void updateSystems (float dt);
        void fixedUpdateSystems (float dt);
        void postUpdateSystems (float dt);
        void flushRemovedEntities ();
        void debugDraw ();

        template <class T>
        T* getMessage () const
        {
            return this->getMessagePool<T>()->getMessage();
        }
        
        template <class T>
        void restoreMessage (T* message)
        {
            this->getMessagePool<T>()->restoreMessage(message);
        }
        
		bool sendMessage (MaleoECS::Entity * entity, const std::string & messageId, void * sender);
		bool sendMessage (MaleoECS::Entity * entity, const std::string & messageId, void * data, void * sender);
		bool sendMessage (uint64_t uniqueId, const std::string & message, void * data, void * sender);
		bool deliverMessage (MaleoECS::Entity * target, const std::string & messageId, void * data, void * sender);

        MaleoECS::Universe * getUniverse () const;
        void setUniverse (MaleoECS::Universe * universe);
        MaleoECS::EntityTemplateManager * getTemplateManager () const;
        void setTemplateManager (MaleoECS::EntityTemplateManager * templateMgr);
		MaleoECS::SystemManager * getSystemManager () const;
		void setSystemManager (MaleoECS::SystemManager * mgr);
		void addEntityToSystems (MaleoECS::Entity * entity);
        const std::string & getUniverseName () const;
        bool isPaused () const;
        void pause ();
        void resume ();
        void triggerPauseOrResume ();
        void setRemovalCallback (const std::function<void(MaleoECS::Entity *)> & callback);
	protected:
		MaleoECS::Universe * _universe;
        MaleoECS::EntityTemplateManager * _templateManager;
		MaleoECS::SystemManager * _systemManager;
		std::map<uint64_t, MaleoECS::IComponentManagerBase *> _componentManagers;
		std::vector<MaleoECS::Entity *> _entityPool;
		std::map<uint64_t, MaleoECS::Entity *> _uniqueIdToEntity;
        std::vector<uint64_t> _entityToBeRemoved;
        MaleoECS::MessagePoolBase * _messagePool;
        short _entityPoolSize;
        short _availableEntityCount;
        bool _isPaused;
        std::function<void(MaleoECS::Entity *)> _removalCallback;

		template <class T>
		MaleoECS::IComponentManagerBase *  createComponentManager (uint64_t componentTypeId, short defaultSize)
		{
			MaleoECS::IComponentManagerBase * componentMgr = nullptr;
			componentMgr = new MaleoECS::ComponentManager<T>(componentTypeId, defaultSize);

			componentMgr->setEntityManager(this);
			return componentMgr;
		}

		MaleoECS::Entity * allocateEntity (uint64_t uniqueId, uint64_t ownerId);
		void restoreEntity (MaleoECS::Entity * entity);
		void removeEntity (MaleoECS::Entity * entity);
        void removeEntityFromSystems (MaleoECS::Entity * entity);
        void removeEntityFromOwner (MaleoECS::Entity * entity);
        void growEntityPool (short poolSize);
	};
}

#endif
