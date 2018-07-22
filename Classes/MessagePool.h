/*
 *  MessagePool.h
 *
 *  Created on: Dec 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_MESSAGE_POOL_H_
#define _ECS_MESSAGE_POOL_H_

#include <iostream>
#include <vector>
#include <map>
#include "MessageData.h"

namespace MaleoECS
{
    class MessagePoolBase
    {
    public:
        virtual ~MessagePoolBase () { }
    };
    
    template <class T>
    class MessagePool :
        public MessagePoolBase
    {
    public:
        MessagePool (uint64_t messageType, short defaultPoolSize) :
            _poolSize (0),
            _elementsCount (0),
            _nextMessageId (0)
        {
            if (defaultPoolSize > 0)
                this->growPool(defaultPoolSize);
        }
        
        virtual ~MessagePool ()
        {
            for (auto iter = _availableMessages.begin(); iter != _availableMessages.end(); ++iter) {
                auto element = *iter;
                delete element;
            }
            _availableMessages.clear();
            
            for (auto iter = _usedMessages.begin(); iter != _usedMessages.end(); ++iter) {
                auto element = iter->second;
                delete element;
            }
            _usedMessages.clear();
        }
        
        T* getMessage ()
        {
            if (_elementsCount == 0) {
                this->growPool(_poolSize + 1);
            }
            
            T * message = _availableMessages.back();
            _availableMessages.pop_back();
            _elementsCount--;
            message->setMessageId(_nextMessageId);
            _usedMessages.insert(std::make_pair(_nextMessageId++, message));
            
            return message;
        }
        
        void restoreMessage (T * message)
        {
            _usedMessages.erase(message->getMessageId());
            message->reset();
            _elementsCount++;
            _availableMessages.push_back(message);
        }
        
        const std::map<uint64_t, T*> & enumerateUsedMessages ()
        {
            return _usedMessages;
        }
    protected:
        int _messageType;
        std::vector<T *> _availableMessages;
        std::map<uint64_t, T *> _usedMessages;
        short _poolSize;
        short _elementsCount;
        uint64_t _nextMessageId;
        
		void growPool (short poolSize)
		{
			short oldSize = _poolSize;
			short newSize = _poolSize * 2;
			newSize = (newSize > poolSize) ? newSize : poolSize;
			short diffSize = newSize - oldSize;
            
			for(int index = oldSize; index < newSize; index++) {
				T * msg = new T();
				_availableMessages.push_back(msg);
			}
            
			_poolSize = newSize;
			_elementsCount += diffSize;
		}
    };
}

#endif /* _ECS_MESSAGE_POOL_H_ */
