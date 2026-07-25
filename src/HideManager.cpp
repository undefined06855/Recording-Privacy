#include "HideManager.hpp"
#include <Geode/ui/Button.hpp>

HideManager::HideManager()
    : m_ticketCount(0) {}

HideManager& HideManager::get() {
    static HideManager instance;
    return instance;
}

void HideManager::init() {
    cocos2d::CCScheduler::get()->scheduleUpdateForTarget(this, 0, false);
}

void HideManager::registerTicket(cocos2d::CCNode* node) {
    geode::log::trace("registering ticket on {}", node);

    if (node->isRunning()) {
        geode::log::trace("...added ticket (running)");
        m_ticketCount += 1;
    } else {
        node->addEventListener("on-enter-ticket-callback"_spr, geode::NodeEvent(node, geode::NodeEventType::OnEnter), [this] {
            geode::log::trace("...added ticket (event)");
            m_ticketCount += 1;
        });
    }

    node->addEventListener("on-exit-ticket-callback"_spr, geode::NodeEvent(node, geode::NodeEventType::OnExit), [this] {
        geode::Loader::get()->queueInMainThread([this] {
            geode::log::trace("removed ticket from node");
            m_ticketCount -= 1;
        });
    });
}

void HideManager::unregisterTicket(cocos2d::CCNode* node) {
    geode::log::trace("unregistering ticket on {}", node);

    if (node->isRunning()) {
        m_ticketCount -= 1;
    }

    node->removeEventListener("on-enter-ticket-callback"_spr);
    node->removeEventListener("on-exit-ticket-callback"_spr);
}

void HideManager::registerVisibleTicket(cocos2d::CCNode* node) {
    geode::log::trace("registering visible ticket on {}", node);

    if (node->isRunning()) {
        geode::log::trace("...added visible ticket (running)");
        m_visibleTicketCount += 1;
    } else {
        node->addEventListener("on-enter-vis-ticket-callback"_spr, geode::NodeEvent(node, geode::NodeEventType::OnEnter), [this] {
            geode::log::trace("...added visible ticket (event)");
            m_visibleTicketCount += 1;
        });
    }

    node->addEventListener("on-exit-vis-ticket-callback"_spr, geode::NodeEvent(node, geode::NodeEventType::OnExit), [this] {
        geode::log::trace("removed visible ticket from node");
        m_visibleTicketCount -= 1;
    });
}

void HideManager::unregisterVisibleTicket(cocos2d::CCNode* node) {
    geode::log::trace("unregistering visible ticket on {}", node);

    if (node->isRunning()) {
        m_visibleTicketCount -= 1;
    }

    node->removeEventListener("on-enter-vis-ticket-callback"_spr);
    node->removeEventListener("on-exit-vis-ticket-callback"_spr);
}

void HideManager::addHideButton(cocos2d::CCNode* node) {
    auto targetNode = node;
    if (auto cast = geode::cast::typeinfo_cast<FLAlertLayer*>(targetNode)) {
        targetNode = cast->m_mainLayer;
    }

    auto label = cocos2d::CCLabelBMFont::create("Hidden", "bigFont.fnt");
    label->setID("label");
    label->setScale(.3f);

    auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("hideBtn_001.png");
    sprite->setScale(.5f);
    sprite->setID("sprite");

    auto wrapper = geode::Button::create([node, label](geode::Button* button) {
        auto hidden = geode::cast::typeinfo_cast<cocos2d::CCBool*>(button->getUserObject("is-hidden"_spr));
        if (!hidden) return;

        if (hidden->getValue()) {
            hidden->setValue(false);
            HideManager::get().registerVisibleTicket(node);
            button->setOpacity(255);
            label->setString("Visible");
        } else {
            hidden->setValue(true);
            HideManager::get().unregisterVisibleTicket(node);
            button->setOpacity(70);
            label->setString("Hidden");
        }

        button->updateLayout();
    });

    wrapper->setID("hide-button"_spr);
    wrapper->setUserObject("is-hidden"_spr, cocos2d::CCBool::create(true));
    wrapper->setContentSize({ 90.f, 8.f });
    wrapper->setLayout(
        geode::SimpleRowLayout::create()
            ->setGap(5.f)
            ->setMainAxisScaling(geode::AxisScaling::None)
    );

    auto pos = cocos2d::CCPoint{ cocos2d::CCDirector::get()->getWinSize().width / 2.f, 20.f };
    auto localPos = node->convertToNodeSpace(pos);
    wrapper->setPosition(localPos);
    targetNode->addChild(wrapper);

    wrapper->addChild(sprite);
    wrapper->addChild(label);

    // do this afterwards so opacity is cascaded
    wrapper->setCascadeOpacityEnabled(true);
    wrapper->setOpacity(70);

    wrapper->updateLayout();
}

void HideManager::update(float dt) {
    auto hwnd = WindowFromDC(wglGetCurrentDC());

    if (m_ticketCount < 0) {
        geode::log::warn("Ticket count is less than zero! ({})", m_ticketCount);
        m_ticketCount = 0;
    }

    if (m_visibleTicketCount < 0) {
        geode::log::warn("Visible ticket count is less than zero! ({})", m_visibleTicketCount);
        m_visibleTicketCount = 0;
    }

    BOOL ret;
    if (m_ticketCount > 0 && m_visibleTicketCount == 0) {
        // WDA_EXCLUDEFROMCAPTURE sounds better, but in fact makes the window transparent, whereas WDA_MONITOR blacks
        // out the window
        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowdisplayaffinity
        // https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/issues/64#issuecomment-663771707
        ret = SetWindowDisplayAffinity(hwnd, WDA_MONITOR);
    } else {
        ret = SetWindowDisplayAffinity(hwnd, WDA_NONE);
    }

    static bool shownWarning = false;
    if (ret != TRUE && !shownWarning) {
        shownWarning = true;
        geode::log::warn("Failed to set window display affinity! (this message will not be shown again for this session)");
    }
}

$on_mod(Loaded) {
    HideManager::get().init();
}
