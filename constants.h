#ifndef CONSTANTS_H
#define CONSTANTS_H

#define UNINTIALIZED_INSTANCE 0
#define HEIGHT_OF_WEATHER_CONDITION_ICON 60 // experimental
#define WIDTH_OF_WEATHER_CONDITION_ICON 60  // experimental
#define FAHRENHEIT_CODE 10
#define CELCIUS_CODE 20

#include <gtkmm.h>
#include <vector>
#include <string>
enum TemperatureSystems
{
  FAHRENHEIT = FAHRENHEIT_CODE,
  CELCIUS = CELCIUS_CODE

};

enum ErrorCodes
{
  NO_ERROR = 0,
  DATA_INSERTION_ERROR = 1 << 1,
  DATA_READ_ERROR = 1 << 2,
  DATA_UPDATE_ERROR = 1 << 3
};
class Constants
{
private:
  static Constants *instance;

  Constants();
  virtual ~Constants();

  std::map<std::string, GdkRGBA> map_WeatherThemes;
  Gdk::RGBA
      rgba01,
      rgba02,
      rgba03,
      rgba04,
      rgba09,
      rgba10,
      rgba11,
      rgba13,
      rgba50;
  Glib::RefPtr<Gdk::Pixbuf> image;
  std::map<std::string, Glib::RefPtr<Gdk::Pixbuf>> map_Images;
  std::vector<std::string> vec_WeatherConditions = {"01", "02", "03", "04", "09", "10", "11", "13", "50"};

  int iconsWidth;
  TemperatureSystems chosenTemperature;
  bool dynamicUpdateContinue, showAllDataAtOnce;

public:
  static Constants *getInstance();
  std::string apiKey;
  /**
     * @return std::map<std::string , GdkRGBA>
     * @code
     * int x
     * @endcode
     */
  std::map<std::string, GdkRGBA> get_map_of_weather_theme_colors();
  GdkRGBA get_map_of_weather_theme_colors(std::string key);
  Glib::RefPtr<Gdk::Pixbuf> get_image_pointer_by_keyword(std::string keyword);
  void get_url_by_city_id(std::string &url, const int &id);
  void get_standard_resource_width(int &width);
  /**
   * Dynaimc update of the main canvas
   * */
  bool get_dynamic_update_status();
  void set_dynamic_update_status(bool status);

  bool get_show_all_data_consent();
  void set_show_all_data_consent(bool status);

  TemperatureSystems get_chosen_temprature();
  void convert_temperature(std::string temp, int &converetdTemp);
  void set_temprature(TemperatureSystems temp);
  std::string get_selected_temperature_symbol();
};

#endif //CONSTANTS_H