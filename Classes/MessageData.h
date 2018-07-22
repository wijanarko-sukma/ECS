/*
 *  MessageData.h
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#ifndef _ECS_MESSAGE_DATA_H_
#define _ECS_MESSAGE_DATA_H_

#include <iostream>

#define INVALID_MESSAGE_ID 0xFFFFFFFF

namespace MaleoECS
{
	class MessageData
	{
	public:
		MessageData ();
        virtual ~MessageData ();
        
        virtual void reset ();
        uint64_t getMessageId () const;
        void setMessageId (uint64_t messageId);
		bool isHandled () const;
        void setHandled (bool handled);
    protected:
        uint64_t _messageId;
		bool _isHandled;
	};
}

#endif
