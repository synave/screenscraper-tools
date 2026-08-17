#ifndef CONFIGSS_H
#define CONFIGSS_H

class ConfigSS {
  public :
  inline static std::string devid = std::getenv("SSDEVID");
  inline static std::string devpassword = std::getenv("SSDEVPASSWD");
  inline static std::string devpassword_debug = std::getenv("SSDEVPASSWDDEBUG");
  inline static std::string ssid = std::getenv("SSID");
  inline static std::string sspassword = std::getenv("SSPASSWD");

  std::vector<std::string> extensions_connues = {
    ".zip", ".gb", ".gba", ".gbc", ".cue", ".sfc", ".md", ".nes"
  };
};

#endif
