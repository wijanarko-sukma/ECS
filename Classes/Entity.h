/*
 *  Entity.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_ENTITY_H_
#define _ECS_ENTITY_H_

#include <iostream>
#include <stdint.h>
#include <vector>
#include "ECSCore.h"

namespace MaleoECS
{
	class Entity
	{
	public:
		Entity ();
		virtual ~Entity ();
        
        void reset();
		virtual uint64_t getUniqueId () const;
		virtual void setUniqueId (uint64_t objectId);
		virtual uint64_t getOwnerUniqueId () const;
		virtual void setOwnerUniqueId (uint64_t ownerId);
		virtual uint64_t getEntityType () const;
		virtual void setEntityType (uint64_t type);
		virtual uint64_t getComponentTypesBitField () const;
		virtual void setComponentTypesBitField (uint64_t componentTypesBit);
        virtual void registerChild (uint64_t childId);
        virtual int numChildren () const;
        virtual uint64_t getChild (int index) const;
        virtual void unregisterChild (uint64_t childId);
        virtual bool isAlive () const;
        virtual void setAlive (bool alive);

	protected:
		uint64_t _uniqueId;
		uint64_t _ownerUniqueId;
		uint64_t _entityType;
		uint64_t _componentTypesBitField;
        std::vector<uint64_t> _childrenIds;
        bool _isAlive;
	};
}

#endif
