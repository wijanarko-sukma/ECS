/*
 *  Component.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 *
 */

#ifndef _ECS_COMPONENT_H_
#define _ECS_COMPONENT_H_

#include <iostream>
#include <stdint.h>
#include "ECSCore.h"

namespace MaleoECS
{
	class ComponentTypeIdHelper
	{
	public:
		static int getBit (uint64_t componentTypeId)
		{
			return 0x1 << componentTypeId;
		}

		static const int AllMask = 0x7fffffff;
	};

	class Component
	{
	public:
		Component (bool active = false);
		virtual ~Component ();

		virtual bool init ();
		virtual void shutDown ();
        virtual void copy (MaleoECS::Component * component);
		virtual void copyTo (MaleoECS::Component * component);
		bool isActive () const;
        void setActive (bool active);

		virtual bool isInitialized () const;
		virtual void setInitialized (bool initialized);
		virtual uint64_t getComponentType () const;
		virtual void setComponentType (uint64_t componentType);
	protected:
		bool _isActive;
		bool _isInitialized;
		uint64_t _componentType;
	};
}

#endif
