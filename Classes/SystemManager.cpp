/*
 *  SystemManager.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "SystemManager.h"
#include "EntityManager.h"
#include "MessageData.h"

namespace MaleoECS
{
	SystemManager::SystemManager (MaleoECS::EntityManager * em)
	{
		_entityManager = em;
		_entityManager->setSystemManager(this);
		//_systems = new std::map<uint64_t, MaleoECS::System *>();
		//_messagesSystemDispatch = new std::map<uint64_t, MaleoECS::System *>();
		//_globalMessageHandlers = new std::vector<MaleoECS::System *>();
	}

	SystemManager::~SystemManager ()
	{
        for (auto iter = _messagesSystemDispatch.begin(); iter != _messagesSystemDispatch.end(); ++iter) {
            auto systems = iter->second;
            systems->clear();
            delete systems;
        }
		_messagesSystemDispatch.clear();
        
        for (auto iter = _systems.begin(); iter != _systems.end(); ++iter) {
            auto system = iter->second;
            delete system;
            system = nullptr;
        }
		_systems.clear();
        
		_globalMessageHandlers.clear();

		_entityManager = nullptr;
	}

	void SystemManager::addSystem (MaleoECS::System * system)
	{
		system->initialize(_entityManager);
		_systems.insert(std::pair<uint64_t, MaleoECS::System *>(system->getUpdateOrder(), system));

		if (system->getSupportedMessages().size() > 0 && system->getFlags() != SystemFlags::HandleAllMessages) {
			for (auto iter = system->getSupportedMessages().begin(); iter != system->getSupportedMessages().end(); ++iter) {
                std::string messageId = *iter;
                auto iter2 = _messagesSystemDispatch.find(messageId);
                std::vector<MaleoECS::System *> * systemList = nullptr;
				if (iter2 == _messagesSystemDispatch.end()) {
                    systemList = new std::vector<MaleoECS::System *>();
					_messagesSystemDispatch.insert(std::make_pair(messageId, systemList));
				} else {
                    systemList = iter2->second;
                }
                systemList->push_back(system);
			}
		}

		if ((system->getFlags() & MaleoECS::SystemFlags::HandleAllMessages) == MaleoECS::SystemFlags::HandleAllMessages) {
			_globalMessageHandlers.push_back(system);
		}
	}
    
    void SystemManager::removeSystem (MaleoECS::System *system)
    {
        _systems.erase(system->getUpdateOrder());
        
        if (system->getSupportedMessages().size() > 0 && system->getFlags() != SystemFlags::HandleAllMessages) {
            for (auto iter = system->getSupportedMessages().begin(); iter != system->getSupportedMessages().end(); ++iter) {
                std::string messageId = *iter;
                auto iter2 = _messagesSystemDispatch.find(messageId);
                std::vector<MaleoECS::System *> * systemList = iter2->second;
                auto iter3 = std::find(systemList->begin(), systemList->end(), system);
                systemList->erase(iter3);
            }
        }
        
        if ((system->getFlags() & MaleoECS::SystemFlags::HandleAllMessages) == MaleoECS::SystemFlags::HandleAllMessages) {
            auto iter = std::find(_globalMessageHandlers.begin(), _globalMessageHandlers.end(), system);
            _globalMessageHandlers.erase(iter);
        }
    }

	const std::map<uint64_t, MaleoECS::System *> & SystemManager::getSystems () const
	{
		return _systems;
	}
    
    MaleoECS::System * SystemManager::getSystemWithId (uint64_t Id) const
    {
        return _systems.at(Id);
    }

	void SystemManager::updateSystems (float dt)
	{
		for (auto iter = _systems.begin(); iter != _systems.end(); ++iter) {
			if (iter->second->isEnabled())
                iter->second->update(dt);
		}
	}
    
    void SystemManager::fixedUpdateSystems (float dt)
	{
		for (auto iter = _systems.begin(); iter != _systems.end(); ++iter) {
			if (iter->second->isEnabled())
                iter->second->fixedUpdate(dt);
		}
	}
    
    void SystemManager::debugDraw ()
    {
        for (auto iter = _systems.begin(); iter != _systems.end(); ++iter) {
            if (iter->second->isEnabled() && iter->second->isDebugable())
                iter->second->debugDraw();
        }
    }

	bool SystemManager::sendMessage (MaleoECS::Entity * target, const std::string & messageId, void * data, void * sender)
	{
		bool success = true;
		MaleoECS::System * system = NULL;
		MaleoECS::MessageData * msg = (MaleoECS::MessageData *)data;
		auto iter = _messagesSystemDispatch.find(messageId);

		if (iter != _messagesSystemDispatch.end()) {
            auto systemList = iter->second;
            for (auto iter2 = systemList->begin(); iter2 != systemList->end(); ++iter2) {
                auto system = *iter2;
                success &= system->sendMessage(target, messageId, data, sender);
            }
        } else {
            success = false;
        }

		// We now allow for global message handlers
		if (!msg->isHandled()) {
			for (auto iter2 = _globalMessageHandlers.begin(); iter2 != _globalMessageHandlers.end(); ++iter2) {
                auto system = *iter2;
				system->sendMessage(target, messageId, data, sender);
				MaleoECS::MessageData * msg2 = (MaleoECS::MessageData *)data;

				if (msg2->isHandled()) {
					break;
				}
			}
		}
		return success;
	}
}
