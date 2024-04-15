#include <YaProject.h>


using json = nlohmann::json;


YaScheduleApp::YaScheduleApp(std::string openWeatherApiKey, std::string yaScheduleApiKey,
	std::string cityFrom, std::string cityTo) {
	this->openWeatherClient = new httplib::Client("http://api.openweathermap.org");
	this->yandexScheduleClient = new httplib::Client("http://api.rasp.yandex.net");
	this->cityFrom = cityFrom;
	this->cityTo = cityTo;
	this->openWeatherApiKey = openWeatherApiKey;
	this->yaScheduleApiKey = yaScheduleApiKey;
}


json YaScheduleApp::makeRequest(httplib::Client* client, std::string request) {
	if (auto response = client->Get(request)) {
		if (response->status == 200) {
			return json::parse(response->body);
		}
		else {
			std::cout << "Bad request " << response->status << std::endl;
		}
	}
	return *new json;
}


std::vector <std::string> YaScheduleApp::getLatLon(std::string city) {
	std::string request = this->openWeatherRequest;
	request += "q=" + city;
	request += "&appid=" + this->openWeatherApiKey;
	json result = this->makeRequest(this->openWeatherClient, request);
	std::vector <std::string> latLon = { to_string(result["coord"]["lon"]), to_string(result["coord"]["lat"]) };
	return latLon;
}


std::string YaScheduleApp::getCityCode(std::vector <std::string> latLon) {
	std::string request = this->yaCityCodeRequest;
	request += "apikey=" + this->yaScheduleApiKey;
	request += "&lat=" + latLon[1];
	request += "&lng=" + latLon[0];
	std::string code = this->makeRequest(this->yandexScheduleClient, request)["code"].get<std::string>();
	return code;
}


std::vector <std::vector < std::map < std::string, std::string>>> YaScheduleApp::getScheduleBetweenCities() {
	std::string request = this->yaScheduleRequest;
	request += "apikey=" + this->yaScheduleApiKey;
	request += "&from=" + this->getCityCode(this->getLatLon(this->cityFrom));
	request += "&to=" + this->getCityCode(this->getLatLon(this->cityTo));
	json result = this->makeRequest(this->yandexScheduleClient, request);
	std::map <std::string, std::string> raceTitle;
	std::map <std::string, std::string> raceNumber;
	std::map <std::string, std::string> transportType;
	map <string, string> transportCodeType;
	map <string, string> transportModelType;
	map <string, string> timeFromTo;
	map <string, string> ifJump;
	map <string, string> priceOf;
	for (int i = 0; i < result["segments"].size(); i++) {
		std::vector <std::map <std::string, std::string>> race{};
		raceTitle["Race Title"] = result["segments"][i]["thread"]["title"].get<std::string>();
		raceNumber["Race Number"] = result["segments"][i]["thread"]["number"].get<std::string>();
		transportType["Transport Type"] = result["segments"][i]["thread"]["transport_type"].get<std::string>();
		transportCodeType["Transport Code Type"] = result["thread"][i]["carrier"]["code"].get<std::string>();
		transportModelType["Transport Model Type"] = result["thread"][i]["carrier"]["codes"]["logo"].get<string>();
		timeFromTo["Time From To"] = result ["segments"][i]["departure"]["arrival"].get<string>();
		ifJump["If Jump"] = result["has_transfers"][i].get<string>();
		priceOf["Price Of"] = result["et_marker"][i]["currency"]["cents"];
		race.push_back(raceTitle);
		race.push_back(raceNumber);
		race.push_back(transportType);
		race.push_back(transportCodeType);
		race.push_back(transportModelType);
		race.push_back(timeFromTo);
		race.push_back(ifJump);
		race.push_back(priceOf);
		this->schedule.push_back(race);
	}
	return this->schedule;
}

