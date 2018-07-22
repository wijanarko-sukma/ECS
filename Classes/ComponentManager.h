/*
 *  ComponentManager.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_COMPONENT_MANAGER_H_
#define _ECS_COMPONENT_MANAGER_H_

#include <iostream>
#include <stdint.h>
#include <map>
#include "ECSCore.h"
#include "Component.h"
#include "Entity.h"

namespace MaleoECS
{
	class IComponentManagerBase
	{
	public:
        virtual ~IComponentManagerBase () { };
		virtual short elementsLeft (void) const = 0;
		virtual bool isComponentAllocated (MaleoECS::Entity * entity) const = 0;
		virtual bool isComponentAllocated (uint64_t uniqueId) const = 0;
		virtual MaleoECS::Component * getComponent (MaleoECS::Entity * entity) = 0;
		virtual MaleoECS::Component * getComponent (uint64_t uniqueId) = 0;
		virtual MaleoECS::Component * allocateComponent (MaleoECS::Entity * entity) = 0;
		virtual MaleoECS::Component * allocateComponent (uint64_t uniqueId) = 0;
		virtual void restoreComponent (MaleoECS::Entity * entity) = 0;
		virtual void restoreComponent (uint64_t uniqueId) = 0;
		virtual void setEntityManager (MaleoECS::EntityManager * em) = 0;
	};


	template<class T>
	class IComponentMapper
	{
	public:
		virtual T * getComponentFor (MaleoECS::Entity * entity) = 0;
		virtual T * getComponentFor (uint64_t uniqueId) = 0;
		virtual const std::map<uint64_t, T *> & enumerateComponents () = 0;
	};


	template<class T>
	class ComponentManager : public IComponentManagerBase, public IComponentMapper<T>
	{
	public:
		ComponentManager (uint64_t componentTypeId, short defaultPoolSize) :
			_poolSize(0),
			_elementsCount(0)
		{
			//_componentPool = new std::vector<T *>();
			//_uniqueIdToComponent = new std::map<uint64_t, T *>();
			if(defaultPoolSize > 0) {
				this->growPool(defaultPoolSize);
			}

			_componentTypeId = componentTypeId;
		}

		virtual ~ComponentManager ()
		{
			_entityManager = nullptr;
            for (auto iter = _componentPool.begin(); iter != _componentPool.end(); ++iter) {
                auto component = (*iter);
                delete component;
                component = nullptr;
            }
			_componentPool.clear();
            
            for (auto iter = _uniqueIdToComponent.begin(); iter != _uniqueIdToComponent.end(); ++iter) {
                auto component = iter->second;
                delete component;
                component = nullptr;
            }
			_uniqueIdToComponent.clear();
            /*
            delete _componentPool;
            delete _uniqueIdToComponent;
            _componentPool = nullptr;
            _uniqueIdToComponent = nullptr;
             */
		}

		void setEntityManager (MaleoECS::EntityManager * em)
		{
			_entityManager = em;
		}

		short elementsLeft (void) const
		{
			return _elementsCount;
		}

		bool isComponentAllocated (MaleoECS::Entity * entity) const
		{
			return this->isComponentAllocated(entity->getUniqueId());
		}

		bool isComponentAllocated (uint64_t uniqueId) const
		{
			bool allocated = (_uniqueIdToComponent.find(uniqueId) != _uniqueIdToComponent.end());
			return allocated;
		}

		T * getComponent (MaleoECS::Entity * entity)
		{
			return this->getComponent(entity->getUniqueId());
		}

		T * getComponent (uint64_t uniqueId)
		{
            T * component = nullptr;
            if (_uniqueIdToComponent.count(uniqueId) == 1)
                component = _uniqueIdToComponent.at(uniqueId);
			return component;
		}

		T * allocateComponent (MaleoECS::Entity * entity)
		{
			return this->allocateComponent(entity->getUniqueId());
		}

		T * allocateComponent (uint64_t uniqueId)
		{
			if (_elementsCount == 0) {
				this->growPool(_poolSize + 1); // Actually, it will double.
			}

			T * component = _componentPool.back();
			_componentPool.pop_back();
			//Component * result = dynamic_cast<Component *>data;
			component->init();
			_elementsCount--;

			_uniqueIdToComponent.insert(_uniqueIdToComponent.begin(), std::make_pair(uniqueId, component));
			return component;
		}

		void restoreComponent (MaleoECS::Entity * entity)
		{
			this->restoreComponent(entity->getUniqueId());
		}

		void restoreComponent (uint64_t uniqueId)
		{
			T * component = _uniqueIdToComponent.at(uniqueId);
            component->shutDown();
			_componentPool.push_back(component);
			_elementsCount++;
			_uniqueIdToComponent.erase(uniqueId);
		}

		T * getComponentFor (MaleoECS::Entity * entity)
		{
			return this->getComponentFor(entity->getUniqueId());
		}

		T * getComponentFor (uint64_t uniqueId)
		{
			T * component = nullptr;
            if (_uniqueIdToComponent.count(uniqueId) == 1)
                component = _uniqueIdToComponent.at(uniqueId);
			return component;
		}

		const std::map<uint64_t, T *> & enumerateComponents ()
		{
			return _uniqueIdToComponent;
		}
	protected:
		MaleoECS::EntityManager * _entityManager;
		int	_componentTypeId;
		std::vector<T *> _componentPool;
		std::map<uint64_t, T *> _uniqueIdToComponent;
		short _poolSize;
		short _elementsCount;
        
		void growPool (short poolSize)
		{
			//assert(poolSize > _iPoolSize);
            
			short oldSize = _poolSize;
			short newSize = _poolSize * 2;
			newSize = (newSize > poolSize) ? newSize : poolSize;
			short diffSize = newSize - oldSize;
            
			for(int index = oldSize; index < newSize; index++) {
				T * data = new T();
				_componentPool.push_back(data);
			}
            
			_poolSize = newSize;
			_elementsCount += diffSize;
		}
	};
}

#endif
