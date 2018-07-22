/*
 *  SystemManager.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_SYSTEM_MANAGER_H_
#define _ECS_SYSTEM_MANAGER_H_

#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <map>
#include <vector>
#include "ECSCore.h"
#include "Entity.h"
#include "System.h"

namespace MaleoECS
{
	class SystemManager
	{
	public:
		SystemManager (MaleoECS::EntityManager * em);
		virtual ~SystemManager ();

		void addSystem (MaleoECS::System * system);
        void removeSystem (MaleoECS::System * system);
		void updateSystems (float dt);
        void fixedUpdateSystems (float dt);
        void debugDraw ();
		bool sendMessage (MaleoECS::Entity * target, const std::string & messageId, void * data, void * sender);
		const std::map<uint64_t, MaleoECS::System *> & getSystems(void) const;
        MaleoECS::System * getSystemWithId (uint64_t Id) const;

	protected:
		MaleoECS::EntityManager * _entityManager;
		std::map<uint64_t, MaleoECS::System *> _systems;
		//std::map<uint64_t, MaleoECS::System *> _messagesSystemDispatch;
        std::map<std::string, std::vector<MaleoECS::System *> *> _messagesSystemDispatch;
		std::vector<MaleoECS::System *> _globalMessageHandlers;
	};
}

#endif
