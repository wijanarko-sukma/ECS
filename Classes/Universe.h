/*
 *  Universe.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_UNIVERSE_H_
#define _ECS_UNIVERSE_H_

#include <iostream>
#include <stdint.h>
#include "ECSCore.h"

namespace MaleoECS
{
    class Universe
    {
    public:
        Universe (const std::string & name);
        virtual ~Universe ();
        
        static uint64_t getTopLevelGroupUniqueIdBase ();
        static bool isTopLevelGroupId (uint64_t uniqueId);
        static bool hasEntityOwner (uint64_t uniqueId);
        const std::string & getName () const;
        void resetUniqueId ();
        uint64_t getNextUniqueId ();
    private:
        std::string _name;
        uint64_t _nextUniqueId;
    };
}

#endif
