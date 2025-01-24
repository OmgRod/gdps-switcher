#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <Geode/Geode.hpp>

$on_mod(Loaded) {
	handleURI("/gdps/add", [](std::string const& path) {
		FLAlertLayer::create("GDPS Switcher", path, "Ok")->show();
	});
}