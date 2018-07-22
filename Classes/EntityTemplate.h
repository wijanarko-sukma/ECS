/*
 *  EntityTemplate.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_ENTITY_TEMPLATE_H_
#define _ECS_ENTITY_TEMPLATE_H_

#include <iostream>
#include <stdint.h>
#include <vector>
#include <string>
#include "ECSCore.h"
#include "Component.h"

namespace MaleoECS
{
	class EntityTemplate
	{
	public:
		EntityTemplate (const std::string & name, uint64_t type);
		virtual ~EntityTemplate ();

		bool addComponent (MaleoECS::Component * component);
		const std::vector<MaleoECS::Component *> & getComponents () const;

		const std::string & getTemplateKey () const;
		uint64_t getTemplateType () const;
		void setTemplateType (uint64_t templateType);
	protected:
        std::string	_name;
		uint64_t _templateType;
		std::vector<MaleoECS::Component *> _components;
	};
}

#endif
