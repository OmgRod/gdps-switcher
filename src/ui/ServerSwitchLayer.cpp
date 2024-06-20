#include "ServerSwitchLayer.hpp"
#include "ServerNode.hpp"
#include "ServerPopup.hpp"

#include <Geode/utils/cocos.hpp>

std::string current;

bool ServerSwitchLayer::init()
{
    if (!CCLayer::init())
        return false;

    this->setID("ServerSwitchLayer");

    this->setKeypadEnabled(true);

    auto background = createLayerBG();
    background->setID("background");
    this->addChild(background);

    auto winSize = CCDirector::get()->getWinSize();

    auto title = CCLabelBMFont::create("Private Server Switcher", "goldFont.fnt");
    title->setPosition(winSize.width / 2, winSize.height - 22.5f);
    title->setID("title-text");
    this->addChild(title);

    auto menu = CCMenu::create();
    menu->setID("menu");
    menu->setZOrder(10);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this, menu_selector(ServerSwitchLayer::onGoBack));
    backBtn->setPosition(-winSize.width / 2 + 25.f, winSize.height / 2 - 25.f);
    backBtn->setID("back-button");
    menu->addChild(backBtn);

    auto applyBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .8f),
        this, menu_selector(ServerSwitchLayer::onApply));

    applyBtn->setPosition(0, -135);
    applyBtn->setID("apply-button");
    applyBtn->setOpacity(50);
    menu->addChild(applyBtn);

    this->addChild(menu);

    auto scrollBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(90);
    scrollBg->setContentSize({winSize.width - 85, winSize.height - 100});
    scrollBg->setPosition(winSize / 2);
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    this->addChild(scrollBg);

    scroll = ScrollLayer::create({winSize.width - 100, winSize.height - 100});
    scroll->setPosition((winSize - scroll->getContentSize()) / 2);
    scroll->setID("server-scroll");
    this->addChild(scroll);

    auto servers = Mod::get()->getSavedValue<std::vector<ServerEntry>>("saved-servers");
    log::info("Loaded {} servers", servers.size());
    current = Mod::get()->getSavedValue<std::string>("server");
    update(servers, true);

    auto cornerMenu = CCMenu::create();
    cornerMenu->setID("corner-menu");
    cornerMenu->setPosition(winSize.width - 30.5, 68.f);
    cornerMenu->setLayout(ColumnLayout::create()->setAxisAlignment(AxisAlignment::Start));
    cornerMenu->setContentSize({ 50.f, 130.f });
    this->addChild(cornerMenu);

    auto newBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png"),
        this, menu_selector(ServerSwitchLayer::onNew));
    newBtn->setID("new-button");
    cornerMenu->addChild(newBtn);
    cornerMenu->updateLayout();
    
    return true;
}

void ServerSwitchLayer::onNew(CCObject *) {
    ServerPopup::create(this, nullptr)->show();
}

void ServerSwitchLayer::keyBackClicked()
{
    this->onGoBack(nullptr);
}

void ServerSwitchLayer::onGoBack(CCObject *)
{
    if (current == Mod::get()->getSavedValue<std::string>("server"))
    {
        CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, MenuLayer::scene(false)));
    }
    else
    {
        geode::createQuickPopup(
            "Exit without saving?",      // title
            "You have unsaved changes.", // content
            "No", "Yes",                 // buttons
            [](auto, bool btn2)
            {
                if (btn2)
                {
                    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, MenuLayer::scene(false)));
                }
            });
    }
}

ServerSwitchLayer *ServerSwitchLayer::create()
{
    auto ret = new ServerSwitchLayer;
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ServerSwitchLayer::onApply(CCObject *)
{
    update(Mod::get()->getSavedValue<std::vector<ServerEntry>>("saved-servers"), false);
    Mod::get()->setSavedValue("server", current);
    geode::createQuickPopup(
        "Restart to apply",
        "You must restart the game for changes to take effect.",
        "Later", "Restart",
        [](auto, bool btn2)
        {
            if (btn2)
            {
                game::restart();
            }
        });
}

void ServerSwitchLayer::selectServer(ServerNode *node)
{
    current = node->getServer().url;
    update(Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers"), false);
}

void ServerSwitchLayer::update(std::vector<ServerEntry> const &servers, bool resetPos)
{
    scroll->m_contentLayer->removeAllChildren();
    float totalHeight = 0.f;
    std::vector<ServerNode *> rendered;

    for (auto &server : servers)
    {
        auto node = ServerNode::create(this, server, {scroll->m_contentLayer->getContentSize().width, 45}, server.url == current);
        node->setPosition(0, totalHeight);
        scroll->m_contentLayer->addChild(node);
        totalHeight += 50;
        rendered.push_back(node);
    }
    totalHeight -= 5;

    if (totalHeight < scroll->m_contentLayer->getContentSize().height)
    {
        totalHeight = scroll->m_contentLayer->getContentSize().height;
    }

    for (auto &node : rendered)
    {
        node->setPositionY(totalHeight - node->getPositionY() - 45);
    }

    scroll->m_contentLayer->setContentSize({scroll->m_contentLayer->getContentWidth(), totalHeight});

    if (resetPos)
    {
        scroll->moveToTop();
    }
}