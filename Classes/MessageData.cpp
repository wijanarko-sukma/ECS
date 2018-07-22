/*
 *  MessageData.cpp
 *
 *  Created on: Aug 4, 2014
 *  Author: Wijanarko Sukma
 */

#include "MessageData.h"

namespace MaleoECS
{
    MessageData::MessageData () :
        _messageId (INVALID_MESSAGE_ID),
        _isHandled (false)
    {
        
    }
    
    MessageData::~MessageData ()
    {
        
    }
    
    void MessageData::reset ()
    {
        _messageId = INVALID_MESSAGE_ID;
        _isHandled = false;
    }
    
    uint64_t MessageData::getMessageId () const
    {
        return _messageId;
    }
    
    void MessageData::setMessageId (uint64_t messageId)
    {
        _messageId = messageId;
    }
    
    bool MessageData::isHandled () const
    {
        return _isHandled;
    }
    
    void MessageData::setHandled (bool handled)
    {
        _isHandled = handled;
    }
}