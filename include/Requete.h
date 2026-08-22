#ifndef REQUETE_H
#define REQUETE_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <tinyxml2.h>

class Requete
{
 private:
  static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp);
  static size_t write_to_file(void *ptr, size_t size, size_t nmemb, void *userdata);
  
 public:
  // static tinyxml2::XMLDocument* request(const std::string& url);
  static tinyxml2::XMLDocument* api_to_xml(const std::string& base_url, std::vector<std::string> parameter_names, std::vector<std::string> parameter_values);
  static void api_to_save_file(std::string url, std::string outfile);
  
};

#endif
