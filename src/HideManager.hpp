#pragma once

class HideManager : public cocos2d::CCObject {
    HideManager();
public:
    static HideManager& get();
    void init();

    int m_ticketCount;
    int m_visibleTicketCount;

    void registerTicket(cocos2d::CCNode* node);
    void unregisterTicket(cocos2d::CCNode* node);

    void registerVisibleTicket(cocos2d::CCNode* node);
    void unregisterVisibleTicket(cocos2d::CCNode* node);

    void addHideButton(cocos2d::CCNode* node);

    virtual void update(float dt) override;
};
