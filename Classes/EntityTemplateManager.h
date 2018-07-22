/*
 *  EntityTemplateManager.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_ENTITY_TEMPLATE_MANAGER_H_
#define _ECS_ENTITY_TEMPLATE_MANAGER_H_

#include <iostream>
#include <string>
#include <stdint.h>
#include <map>
#include "EntityTemplate.h"

namespace MaleoECS
{
	class EntityTemplateManager
	{
	public:
		EntityTemplateManager ();
		virtual ~EntityTemplateManager ();

		bool registerTemplate (MaleoECS::EntityTemplate * entity);
		bool isTemplateExist (const std::string & key) const;
		MaleoECS::EntityTemplate * getTemplate (const std::string & key) const;

	private:
		std::map<std::string, MaleoECS::EntityTemplate *> _entityTemplates;

	};
}

#endif
