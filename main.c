#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chjdkver.h>

char *nopath(char *path){
    char *nopath;

    nopath = strrchr(path, '/');
    nopath++;

    return nopath;
}

void show_version(char *argv){
    printf("%s version %s\n", nopath(argv), VERSION_STRING);
}

void show_help(char *argv){
    printf("Usage: %s [OPTIONS] [<version>]\n", nopath(argv));
    printf("OPTIONS\n");
    printf("    -i, --init:      perform init_env and init_jdks.\n");
    printf("    -e, --init_env:  prepare and check the environment.\n");
    printf("    -j, --init_jdks: create symlinks for installed jdk.\n");
    printf("    -v, --version:   print the version and exit.\n");
    printf("    -h, --help:      print this message.\n\n");
    printf("The program prints installed JDK versions on your environemnt\n");
    printf("with no command line arguments.\n");
    printf("The program switches JDK version given command line argument.\n");
}

int parse_option(int argc, char **argv){
    int i;
    int type = 0;
    for(i = 1; i < argc; i++){
        if(*argv[i] == '-'){
            if(strcmp(argv[i], "-v") == 0 ||
               strcmp(argv[i], "--version") == 0){
                type = type | VERSION;
            }
            if(strcmp(argv[i], "-h") == 0 ||
               strcmp(argv[i], "--help") == 0){
                type = type | HELP;
            }
            if(strcmp(argv[i], "-i") == 0 ||
               strcmp(argv[i], "--init") == 0){
                type = type | INIT_ENV | INIT_JDKS;
            }
            if(strcmp(argv[i], "-e") == 0 ||
               strcmp(argv[i], "--init_env") == 0){
                type = type | INIT_ENV;
            }
            if(strcmp(argv[i], "-j") == 0 ||
               strcmp(argv[i], "--init_jdks") == 0){
                type = type | INIT_JDKS;
            }
        }
        else{
            type = type | CHANGE;
        }
    }
    if(type == 0){
        type = LIST;
    }
    return type;
}

int execute_option(int type, int argc, char **argv){
    if((type & VERSION) == VERSION){
        show_version(argv[0]);
    }
    if((type & HELP) == HELP){
        show_help(argv[0]);
    }
    if((type & (HELP | VERSION)) != 0){
        return 0;
    }

    if((type & LIST) == LIST){
        return list_versions();
    }
    else if((type & CHANGE) == CHANGE){
        int i;
        for(i = 1; i < argc; i++){
            if(*argv[i] != '-'){
                return change_version(argv[i]);
            }
        }
    }
    else{
        if((type & INIT_ENV) == INIT_ENV){
            init_env();
        }
        if((type & INIT_JDKS) == INIT_JDKS){
            init_jdks();
        }
    }
    return 1;
}

int main(int argc, char *argv[]){
    int type = parse_option(argc, argv);

    return execute_option(type, argc, argv);
}
