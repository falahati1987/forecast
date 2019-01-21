#ifndef H_COMMONS
#define H_COMMONS
#define FAHRENHEIT_CODE 10
#define CELCIUS_CODE 20

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
#endif // H_COMMONS