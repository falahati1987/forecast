#include "constants.h"
#include <iostream>
#include <regex>

using namespace std;

Constants *Constants::instance = UNINTIALIZED_INSTANCE;

Constants *Constants::getInstance()
{

    if (instance == UNINTIALIZED_INSTANCE)
    {
        instance = new Constants();
    }
    return instance;
}

Constants::Constants() : chosenTemperature(CELCIUS),
                         dynamicUpdateContinue(true),
                         showAllDataAtOnce(false)
{

    double max_ = 255.f;
    rgba01.set_rgba(198 / max_, 255.0 / max_, 9.0 / max_, .5);
    rgba02.set_rgba(6.0 / max_, 135.0 / max_, 209.0 / max_, .5);
    rgba03.set_rgba(141.0 / max_, 143.0 / max_, 143.0 / max_, .5);
    rgba04.set_rgba(152.0 / max_, 154.0 / max_, 155.0 / max_, .5);
    rgba09.set_rgba(25.0 / max_, 119.0 / max_, 174.0 / max_, .5);
    rgba10.set_rgba(19.0 / max_, 160.0 / max_, 243.0 / max_, .5);
    rgba11.set_rgba(92.0 / max_, 123.0 / max_, 143.0 / max_, .5);
    rgba13.set_rgba(232.0 / max_, 238.0 / max_, 243.0 / max_, .5);
    rgba50.set_rgba(211.0 / max_, 212.0 / max_, 186.0 / max_, .5);

    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("01", *rgba01.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("02", *rgba02.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("03", *rgba03.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("04", *rgba04.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("09", *rgba09.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("10", *rgba10.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("11", *rgba11.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("13", *rgba13.gobj()));
    map_WeatherThemes.insert(pair<std::string, GdkRGBA>("50", *rgba50.gobj()));

    // Loading the icons that will be used frequently
    for (auto it = vec_WeatherConditions.begin(); it != vec_WeatherConditions.end(); ++it)
    {
        try
        {
            image = Gdk::Pixbuf::create_from_resource("/ir/youconnect/image/" + (*it) + "d", WIDTH_OF_WEATHER_CONDITION_ICON,
                                                      HEIGHT_OF_WEATHER_CONDITION_ICON);
            map_Images.insert(pair<std::string, Glib::RefPtr<Gdk::Pixbuf>>((*it) + "d", image));

            image = Gdk::Pixbuf::create_from_resource("/ir/youconnect/image/" + (*it) + "n", WIDTH_OF_WEATHER_CONDITION_ICON,
                                                      HEIGHT_OF_WEATHER_CONDITION_ICON);
            map_Images.insert(pair<std::string, Glib::RefPtr<Gdk::Pixbuf>>((*it) + "n", image));
        }
        catch (const Gio::ResourceError &ex)
        {
            std::cerr << "Resource error : " << ex.what() << std::endl;
        }
        catch (const Gdk::PixbufError &ex)
        {
            std::cerr << "PixbufError: " << ex.what() << std::endl;
        }
    }
    apiKey = "bd60f7d457d8c11ff79910783c21ebd7";
}

Constants::~Constants()
{
}

std::map<std::string, GdkRGBA> Constants::get_map_of_weather_theme_colors()
{
    return map_WeatherThemes;
}
GdkRGBA Constants::get_map_of_weather_theme_colors(std::string key)
{
    GdkRGBA nullRgba;

    /**
     * since C++11 , standard library is supporting regex
     * Therefore, I use it
     * http://www.cplusplus.com/reference/regex/regex_search/
     * */
    std::regex findNumberPattern("([0-9]+)([a-zA-Z]{1})");
    std::smatch discoveredPattern;
    if (std::regex_match(key, discoveredPattern, findNumberPattern))
    {
        return map_WeatherThemes[discoveredPattern[1]];
    }
    else
        return nullRgba;
}

Glib::RefPtr<Gdk::Pixbuf> Constants::get_image_pointer_by_keyword(std::string keyword)
{
    std::regex findNumberPattern("([0-9]+)([a-zA-Z]{1})");
    std::smatch discoveredPattern;
    if (std::regex_match(keyword, discoveredPattern, findNumberPattern))
    {
        return map_Images[std::string(discoveredPattern[1]) + "d"];
    }
    return map_Images[keyword];
}
void Constants::map_temperature_system(const float &temp, int &convertedTemp, const TemperatureSystems &system)
{
    switch (system)
    {
    case FAHRENHEIT:
        convertedTemp = int((temp - 32) * (5 / 9));
        break;
    case CELCIUS:
        convertedTemp = int(temp);
        break;
    case KELVIN:
        convertedTemp = int(temp- 273.f);
        break;
    default:
        break;
    }
}
void Constants::get_url_by_city_id(std::string &url, const int &id)
{
    url = "https://api.openweathermap.org/data/2.5/forecast?id=" +
          std::to_string(id) +
          "&appid=" + apiKey;
}
void Constants::convert_temperature(float &temp, int &convertedTemp, const TemperatureSystems &system)
{
    map_temperature_system(temp, convertedTemp, system);
    try
    {
        switch (chosenTemperature)
        {
        case CELCIUS:
            break;
        case FAHRENHEIT:
            convertedTemp = convertedTemp * 1.8 + 32;
            break;
        default:
            break;
        }
    }
    catch (...)
    {
        std::exception_ptr p = std::current_exception();
        std::clog << (p ? p.__cxa_exception_type()->name() : "null") << std::endl;
    }
}
void Constants::convert_temperature(std::string temp, int &convertedTemp, const TemperatureSystems &system)
{
    float _temp = std::stof(temp);
    convert_temperature(_temp, convertedTemp, system);
}
std::string Constants::get_selected_temperature_symbol()
{
    switch (chosenTemperature)
    {
    case CELCIUS:
        return "°C";
        break;
    case FAHRENHEIT:
        return "°F";
        break;
    default:
        break;
    }
    return "";
}
void Constants::get_standard_resource_width(int &width)
{
    width = WIDTH_OF_WEATHER_CONDITION_ICON;
}

bool Constants::get_dynamic_update_status()
{

    return dynamicUpdateContinue;
}
void Constants::set_dynamic_update_status(bool status)
{

    dynamicUpdateContinue = status;
}

bool Constants::get_show_all_data_consent()
{
    return showAllDataAtOnce;
}
void Constants::set_show_all_data_consent(bool status)
{
    showAllDataAtOnce = status;
}

TemperatureSystems Constants::get_chosen_temprature()
{
    return chosenTemperature;
}
void Constants::set_temperature_system(TemperatureSystems temp)
{
    chosenTemperature = temp;
}