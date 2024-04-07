#ifndef _SENSEDRIVER_SAMPLES_CAPI_PARSEJSON_HPP_
#define _SENSEDRIVER_SAMPLES_CAPI_PARSEJSON_HPP_

#include <fstream>
#include <iostream>
#include <json11/json11.hpp>

static int ParseFromFilename(const std::string& param_json, json11::Json& root) {
    std::string args_str, line;

    std::ifstream ifile_args(param_json);
    if (ifile_args.is_open()) {
        while (!ifile_args.eof()) {
            getline(ifile_args, line);
            args_str += line + "\n";
        }
    }
    std::string error;
    root = json11::Json::parse(args_str, error);
    if (!error.empty()) {
        std::cout << "parse json failed: " << error << std::endl;
        return -1;
    }
    return 0;
}

static int CheckJsonKeyExistence(const json11::Json& json_config_obj,
                                 const std::vector< std::string >& config_keys) {
    const auto& json_map = json_config_obj.object_items();
    for (const auto& key : config_keys) {
        if (json_map.count(key) <= 0u) {
            std::cerr << "this is not key in current json: " << key << std::endl;
            return -1;
        }
    }
    return 0;
}

#endif