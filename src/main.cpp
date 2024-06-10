#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "ServerSwitchLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCHttpClient.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		auto myButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSprite("switchServer.png"_spr),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();

		return true;
	}

	void onMyButton(CCObject*) {
		ServerSwitchLayer::scene();
	}
};

void gdpsSend(CCHttpClient* self, CCHttpRequest* req)
    {
		std::string url = req->getUrl();
		auto newUrl = Mod::get()->getSavedValue<std::string>("server");
		if (newUrl.empty()) {
			newUrl = "https://www.boomlings.com/database";
			Mod::get()->setSavedValue("server", newUrl);
		}
        if (newUrl.ends_with("/")) {
			if (url.starts_with("https://www.boomlings.com/database/")) {
				req->setUrl(url.replace(0, 35, newUrl).c_str());
			} else if (url.starts_with("http://www.boomlings.com/database/")) {
				req->setUrl(url.replace(0, 34, newUrl).c_str());
			}
		} else {
			if (url.starts_with("https://www.boomlings.com/database")) {
				req->setUrl(url.replace(0, 34, newUrl).c_str());
			} else if (url.starts_with("http://www.boomlings.com/database")) {
				req->setUrl(url.replace(0, 33, newUrl).c_str());
			}
		}
		log::info("Sending request to {}", req->getUrl());
        self->send(req);
    }

$execute
{
    Mod::get()->hook(
        reinterpret_cast<void*>(
			geode::addresser::getNonVirtual(&cocos2d::extension::CCHttpClient::send)
        ),
        &gdpsSend,
        "cocos2d::extension::CCHttpClient::send",
        tulip::hook::TulipConvention::Thiscall
    );
}