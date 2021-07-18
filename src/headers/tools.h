#include "config.h"

void displayConsoleInformation(std::map<std::string, float>& cfg);

std::map<std::string, float> readConfig(std::string sConfigPath);

EVENT_CODE eventListener(sf::RenderWindow& window);

int init(sf::RenderWindow& window);
int main();