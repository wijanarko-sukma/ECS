/*
 *  Entity.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "Entity.h"
#include <algorithm>
namespace MaleoECS
{
  Entity::Entity() :
    _uniqueId(INVALID_ENTITY_UNIQUE_ID)
    , _ownerUniqueId(INVALID_ENTITY_UNIQUE_ID)
    , _entityType(0)
    , _componentTypesBitField(0)
    , _isAlive(false)
  {
    //_childrenIds = new std::vector<uint64_t>();
  }

  Entity::~Entity()
  {
    this->reset();
    //delete _childrenIds;
    //_childrenIds = nullptr;
  }

  void Entity::reset()
  {
    _uniqueId = INVALID_ENTITY_UNIQUE_ID;
    _ownerUniqueId = INVALID_ENTITY_UNIQUE_ID;
    _entityType = 0;
    _componentTypesBitField = 0;
    _childrenIds.clear();
    _childrenIds.shrink_to_fit();
    _isAlive = false;
  }

  uint64_t Entity::getUniqueId() const
  {
    return _uniqueId;
  }

  void Entity::setUniqueId(uint64_t uniqueId)
  {
    _uniqueId = uniqueId;
  }

  uint64_t Entity::getOwnerUniqueId() const
  {
    return _ownerUniqueId;
  }

  void Entity::setOwnerUniqueId(uint64_t ownerId)
  {
    _ownerUniqueId = ownerId;
  }

  uint64_t Entity::getEntityType() const
  {
    return _entityType;
  }

  void Entity::setEntityType(uint64_t entityType)
  {
    _entityType = entityType;
  }

  uint64_t Entity::getComponentTypesBitField() const
  {
    return _componentTypesBitField;
  }

  void Entity::setComponentTypesBitField(uint64_t componentBit)
  {
    _componentTypesBitField = componentBit;
  }

  void Entity::registerChild(uint64_t childId)
  {
    _childrenIds.push_back(childId);
  }

  int Entity::numChildren() const
  {
    return _childrenIds.size();
  }

  uint64_t Entity::getChild(int index) const
  {
    return _childrenIds[index];
  }

  void Entity::unregisterChild(uint64_t childId)
  {
    _childrenIds.erase(std::remove_if(_childrenIds.begin(), _childrenIds.end(), [&](uint64_t Id) { return childId == Id; }));
  }

  bool Entity::isAlive() const
  {
    return _isAlive;
  }

  void Entity::setAlive(bool alive)
  {
    _isAlive = alive;
  }
}
