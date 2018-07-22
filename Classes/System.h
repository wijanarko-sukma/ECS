/*
 *  System.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_SYSTEM_H_
#define _ECS_SYSTEM_H_

#include <iostream>
#include <stdint.h>
#include <functional>
#include <vector>
#include <map>
#include "ECSCore.h"
#include "Entity.h"

#define BIND_MESSAGE_CALLBACK(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

namespace MaleoECS
{
	enum SystemFlags
	{
		Normal = 0x00000000,
		HandleAllMessages = 0x00000001
	};

    typedef std::function<bool(MaleoECS::Entity *, const std::string &, void *, void *)> MessageCallback;

	class System
	{
	public:
		System (bool debug = false);
		virtual ~System ();

		bool initialize (MaleoECS::EntityManager * entityMgr);
		virtual void addEntity (MaleoECS::Entity * entity);
		virtual void removeEntity (MaleoECS::Entity * entity);
		virtual void update (float dt) = 0;
        virtual void fixedUpdate (float dt) = 0;
        virtual void debugDraw () = 0;
        virtual void onPause () = 0;
        virtual void onResume () = 0;
		virtual bool sendMessage (MaleoECS::Entity * target, const std::string & messageId, void * data, void * sender);

        virtual MaleoECS::EntityManager * getEntityManager ();
        void setEntityManager (MaleoECS::EntityManager * entityMgr);
		virtual MaleoECS::SystemFlags getFlags () const;
		virtual void setFlags (MaleoECS::SystemFlags flags);
        virtual bool isDebugable () const;
		virtual bool isEnabled () const;
		virtual void setEnabled (bool enabled);
		virtual int getUpdateOrder () const;
		virtual void setUpdateOrder (int updateOrder);
		virtual const std::vector<std::string> & getSupportedMessages () const;
		virtual void setSupportedMessages (const std::vector<std::string> & supportedMessages);
		virtual uint64_t getRequiredComponentTypesBitField () const;
		//virtual void setRequiredComponentTypesBitField (uint64_t requiredComponents);
		virtual uint64_t getRequiredAtLeastOneComponentTypesBitField () const;
		//virtual void setRequiredAtLeastOneComponentTypesBitField (uint64_t requiredAtLeastOne);
        virtual uint64_t getForbiddenComponentTypesBitField () const;

	protected:
		MaleoECS::SystemFlags _flags;
        bool _isDebugable;
		bool _isEnabled;
		MaleoECS::EntityManager * _entityManager;
		std::map<uint64_t, MaleoECS::Entity *> _entityIds;
		int _updateOrder;
		std::vector<std::string> _supportedMessages;
		uint64_t _requiredComponentTypes;
		uint64_t _requiredAtLeastOneComponentTypes;
        uint64_t _forbiddenComponentTypes;
        std::map<std::string, MaleoECS::MessageCallback> * _callbackCollection;

        void construct (int updateOrder, std::vector<uint64_t> * requiredComponentTypeIds, std::map<std::string, MaleoECS::MessageCallback> * callbackCollection = nullptr, std::vector<uint64_t> * optionalButOneRequiredComponentTypeIds = nullptr, std::vector<uint64_t> * forbiddenComponentTypeIds = nullptr, SystemFlags flags = SystemFlags::Normal);

		virtual bool onInitialized (void) = 0;
		virtual void onEnabledChanged (void) = 0;
		virtual void onEntityAdded (MaleoECS::Entity * entity, uint64_t uniqueId) = 0;
		virtual void onEntityRemoved (MaleoECS::Entity * entity, uint64_t uniqueId) = 0;
	};
}

#endif
