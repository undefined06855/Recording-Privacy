#include "HideManager.hpp"

// adds the ticket while checking bool return type
#define ADD_TICKET_ON_INIT(_ClassName, _InitFunction, _InitFunctionParamNames) \
    class $modify(_ClassName) { \
        _InitFunction { \
            if (!_ClassName::_InitFunctionParamNames) return false; \
            auto& manager = HideManager::get(); \
            manager.registerTicket(this); \
            manager.addHideButton(this); \
            return true; \
        } \
    }

// adds ticket without checking return type
#define ADD_TICKET_ON_SETUP(_ClassName, _InitFunction, _InitFunctionParamNames) \
    class $modify(_ClassName) { \
        _InitFunction { \
            !ClassName::_InitFunctionParamNames \
            auto& manager = HideManager::get(); \
            manager.registerTicket(this); \
            manager.addHideButton(this); \
        } \
    }

#include <Geode/modify/MessagesProfilePage.hpp>
ADD_TICKET_ON_INIT(MessagesProfilePage, bool init(bool sent), init(sent));

#include <Geode/modify/GJMessagePopup.hpp>
ADD_TICKET_ON_INIT(GJMessagePopup, bool init(GJUserMessage* message), init(message));

#include <Geode/modify/FriendsProfilePage.hpp>
ADD_TICKET_ON_INIT(FriendsProfilePage, bool init(UserListType type), init(type));

#include <Geode/modify/GJWriteMessagePopup.hpp>
ADD_TICKET_ON_INIT(GJWriteMessagePopup, bool init(int accountID, int messageID), init(accountID, messageID));

#include <Geode/modify/FRequestProfilePage.hpp>
ADD_TICKET_ON_INIT(FRequestProfilePage, bool init(bool sent), init(sent));

#include <Geode/modify/FriendRequestPopup.hpp>
ADD_TICKET_ON_INIT(FriendRequestPopup, bool init(GJFriendRequest* friendRequest), init(friendRequest));
