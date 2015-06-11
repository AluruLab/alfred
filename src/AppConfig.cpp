#include "util.hpp"
#include "AppConfig.hpp"
#include <unistd.h>

// PARAMETERS -------------------------------------------------------
void AppConfig::write(std::ostream& ots){
    ots << "\"params\"      : {" << std::endl;
    ots << " \t\"in_dir\"   : \"" << dir << "\"," << std::endl;
    ots << " \t\"in_files\" : \"" << ifiles.size() << "\"," << std::endl;
    ots << " \t\"out_file\" : \"" << outf << "\"," << std::endl;
    ots << " \t\"log_file\" : \"" << logf << "\"," << std::endl;
    ots << " }," << std::endl;
}

void AppConfig::printHelp(std::ostream& ots){
    ots << "Usage " << app << " "
        << "-i <input dir> or <input files sepreated by comma> "
        << "-o <output file> "
        << "[OPTIONS]" << std::endl << std::endl
        << "Available options:" << std::endl
        << "\t -i <dir>   path to directory with fasta/fastq files" << std::endl
        << "\t -f <list>  list of paths to fasta/fastq files" << std::endl
        << "\t -o <file>  path to output file (should be writable)" << std::endl
        << "\t -l <file>  path to log file (should be writable) ["
        << "/path/to/output/file.log]" << std::endl
        << "\t -k <int>   number of mismatches [1]" << std::endl
        << "\t -h         print this help " << std::endl
        << std::endl;
}

AppConfig::AppConfig(int argc, char** argv){
    char c;
    const char* params = "i:l:f:o:k:hH";
    help = false;
    app = argv[0];
    dir = "";
    outf = "";
    logf = "";
    kv = 1;
    std::string fstr = "";

    while ((c = getopt(argc, argv, params)) != -1) {
        switch (c) {
        case 'i':
            dir = optarg;
            break;
        case 'f':
            fstr = optarg;
            split(fstr, ',', ifiles);
            break;
        case 'o':
            outf = optarg;
            break;
        case 'l':
            logf = optarg;
            break;
        case 'h':
            help = true;
            break;
        case 'H':
            help = true;
            break;
        case 'k':
            kv = atoi(optarg);
            break;
        }
    }
    if(logf.length() == 0)
        logf = outf + ".log";
}

bool AppConfig::validate(std::ostream& ots){
    bool validCfg = true;
    if(help){
        printHelp(ots);
        return false;
    }
    if(dir.length() == 0 && ifiles.size() == 0){
        ots << "Invalid directory " << dir << std::endl;
        validCfg = false;
    }

    if(outf.length() == 0){
        ots << "Invalid output file " << outf << std::endl;
        validCfg = false;
    }

    if(outf.length() > 0){
        ofs.open(outf, std::ofstream::out);
        if(!ofs.is_open()){
            ots << "Error opening output file : " << outf << std::endl;
            validCfg = false;
        }
    }

    if(logf.length() > 0){
        lfs.open(logf, std::ofstream::out);
        if(!lfs.is_open()){
            ots << "Error opening output file : " << logf << std::endl;
            validCfg = false;
        }
    }

    if(kv <= 0){
        ots << "Invalid k value (" << kv
            << "). Using default value of 1" << std::endl;
        kv = 1;
    }

    if(!validCfg){
        printHelp(ots);
        ots << "...exiting" << std::endl;
    }
    return validCfg;
}

AppConfig::~AppConfig(){
    if(ofs.is_open())
        ofs.close();
    if(lfs.is_open())
        lfs.close();
}
