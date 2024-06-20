using namespace geode::prelude;
#include "ServerSwitchLayer.hpp"

class ServerPopup : public Popup<ServerSwitchLayer *, ServerNode *>
{
private:
    CCTextInputNode *m_nameInput;
    CCTextInputNode *m_urlInput;
    ServerSwitchLayer *m_layer;
    ServerNode *m_node;

protected:
    bool setup(ServerSwitchLayer *layer, ServerNode *node) override
    {
        m_layer = layer;
        m_node = node;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle((node == nullptr) ? "Add Server" : "Edit Server");

        auto menu = CCMenu::create();
        menu->setID("data-menu");
        menu->setZOrder(10);
        m_mainLayer->addChild(menu);

        auto nameBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
        nameBg->setContentSize({440, 60});
        nameBg->setScale(0.5f);
        nameBg->setPosition(ccp(0, 25));
        nameBg->setID("name-bg");
        nameBg->setOpacity(50);
        nameBg->setColor({0, 0, 0});
        menu->addChild(nameBg);

        m_nameInput = CCTextInputNode::create(220.f, 30.f, "Enter a name", "bigFont.fnt");
        m_nameInput->setLabelPlaceholderColor(ccColor3B{200, 200, 200});
        m_nameInput->setPosition(ccp(0, 25));
        m_nameInput->setID("name-input");
        m_nameInput->setAllowedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;= ");
        m_nameInput->setMaxLabelLength(24);
        if (node != nullptr)
            m_nameInput->setString(node->getServer().name);
        menu->addChild(m_nameInput);

        auto urlBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
        urlBg->setContentSize({440, 60});
        urlBg->setScale(0.5f);
        urlBg->setPosition(ccp(0, -15));
        urlBg->setID("url-bg");
        urlBg->setOpacity(50);
        urlBg->setColor({0, 0, 0});
        menu->addChild(urlBg);

        m_urlInput = CCTextInputNode::create(220.f, 30.f, "Enter a URL", "bigFont.fnt");
        m_urlInput->setLabelPlaceholderColor(ccColor3B{200, 200, 200});
        m_urlInput->setPosition(ccp(0, -15));
        m_urlInput->setID("url-input");
        m_urlInput->setAllowedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=");
        if (node != nullptr)
            m_urlInput->setString(node->getServer().url);
        menu->addChild(m_urlInput);

        auto saveBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", .8f),
            this, menu_selector(ServerPopup::onSave));
        saveBtn->setPosition(ccp(0, -53));
        saveBtn->setID("save-button");
        menu->addChild(saveBtn);

        return true;
    }

    void onClose(cocos2d::CCObject *)
    {
        if (m_node != nullptr)
        {
            if (m_nameInput->getString() == m_node->getServer().name && m_urlInput->getString() == m_node->getServer().url)
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
                return;
            }
            else
            {
                geode::createQuickPopup("Cancel?", "Your changes will not be saved.", "No", "Yes", [this](auto, bool yes)
                                        {
            if (yes)
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
            } });
            }
        }
        else
        {
            if (m_nameInput->getString().empty() && m_urlInput->getString().empty())
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
                return;
            }
            else
            {
                geode::createQuickPopup("Cancel?", "Your entry will be discarded.", "No", "Yes", [this](auto, bool yes)
                                        {
            if (yes)
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
            } });
            }
        }
    }

    void onSave(CCObject *)
    {
        auto name = m_nameInput->getString();
        auto url = m_urlInput->getString();
        if (name.empty() || std::all_of(name.begin(), name.end(), isspace))
        {
            FLAlertLayer::create("Error", "Name cannot be empty.", "Ok")->show();
            return;
        }
        if (url.empty())
        {
            FLAlertLayer::create("Error", "URL cannot be empty.", "Ok")->show();
            return;
        }
        auto servers = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
        if (m_node != nullptr)
        {
            auto index = std::distance(servers.begin(), std::find_if(servers.begin(), servers.end(), [this](auto &entry)
                                                                     { return entry.name == m_node->getServer().name; }));
            servers[index] = {name, url};
        }
        else
        {
            servers.push_back({name, url});
        }
        Mod::get()->setSavedValue("saved-servers", servers);
        m_layer->update(servers, false);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
        this->removeFromParentAndCleanup(true);
    }

public:
    static ServerPopup *create(ServerSwitchLayer *layer, ServerNode *node = nullptr)
    {
        auto ret = new ServerPopup();
        if (ret && ret->init(240.f, 160.f, layer, node))
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
