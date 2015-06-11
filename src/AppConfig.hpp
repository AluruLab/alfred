#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <vector>
#include <string>
#include <fstream>

// PARAMETERS   ------------------------------------------------------
struct AppConfig{
    std::string dir;
    std::vector<std::string> ifiles;
    std::string outf;
    std::string logf;
    std::string app;
    bool help;
    std::ofstream ofs;
    std::ofstream lfs;
    int kv;

    void printHelp(std::ostream& ots);
    bool validate(std::ostream& ots);
    void write(std::ostream& ots);
    AppConfig(int argc, char** argv);
    ~AppConfig();
};


#endif /* APPCONFIG_H */
