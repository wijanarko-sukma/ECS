/*
 *  Universe.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "Universe.h"

static uint64_t	 TopLevelGroupUniqueIdBase = 0x40000000;

namespace MaleoECS
{
    Universe::Universe (const std::string & name) :
        _name (name)
    {
        _nextUniqueId = 0x00000001;
    }

    Universe::~Universe ()
    {
    }

    bool Universe::isTopLevelGroupId (uint64_t uniqueId)
    {
        // This does *not* include the global group.
        return uniqueId >= 0x40000000;
    }

    bool Universe::hasEntityOwner (uint64_t uniqueId)
    {
        return (uniqueId < 0x40000000) && (uniqueId != INVALID_ENTITY_UNIQUE_ID);
    }

    void Universe::resetUniqueId ()
    {
        _nextUniqueId = 0x00000001;
    }
    
    const std::string & Universe::getName () const
    {
        return _name;
    }

    uint64_t Universe::getNextUniqueId ()
    {
        return _nextUniqueId++;
    }

    uint64_t Universe::getTopLevelGroupUniqueIdBase ()
    {
        return TopLevelGroupUniqueIdBase;
    }
}
