#include <iostream>
#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>


class YaScheduleApp {
	httplib::Client* openWeatherClient;
	httplib::Client* yandexScheduleClient;
	std::string cityFrom;
	std::string cityTo;
	std::string openWeatherRequest = "/data/2.5/weather?";
	std::string yaCityCodeRequest = "/v3.0/nearest_settlement/?";
	std::string yaScheduleRequest = "/v3.0/search/?";
	std::string openWeatherApiKey;
	std::string yaScheduleApiKey;
	std::vector <std::vector < std::map < std::string, std::string>>> schedule;
public:
	YaScheduleApp(std::string openWeatherApiKey, std::string yaScheduleApiKey, std::string cityFrom, std::string cityTo);
	
	std::vector <std::vector < std::map < std::string, std::string>>> getScheduleBetweenCities();
private:
	nlohmann::json makeRequest(httplib::Client* client, std::string request);

	std::vector <std::string> getLatLon(std::string city);

	std::string getCityCode(std::vector <std::string> latLon);
};