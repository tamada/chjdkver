#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#define VERSION_STRING "1.1"

#define CURRENT_JDK_PATH "/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK"
#define TARGET_DIR       "/Library/Java/JavaVirtualMachines"
#define VERSION_PATTERN  "0123456789-_."

#define LIST    0x001
#define CHANGE  0x002
#define HELP    0x004
#define VERSION 0x008

int check_exists(char *path){
    char *buffer;
    struct stat *statbuf;
    int flag;

    buffer = (char *)malloc(sizeof(char) * (strlen(TARGET_DIR) + strlen(path) + 2));
    sprintf(buffer, "%s/%s", TARGET_DIR, path);

    statbuf = (struct stat *)malloc(sizeof(struct stat));

    flag = stat(buffer, statbuf);
    if(flag != 0){
        perror(path);
    }
    if((statbuf->st_mode & S_IFMT) != S_IFDIR){
        printf("%s: not directory\n", path);
        flag = 1;
    }

    free(statbuf);
    free(buffer);
    return flag;
}

int unlink_old(){
    int flag = unlink(CURRENT_JDK_PATH);
    if(flag != 0){
        perror(CURRENT_JDK_PATH);
    }
    return flag;
}

int link_new(char *path){
    char *buffer;
    int flag;

    buffer = (char *)malloc(sizeof(char) * (strlen(TARGET_DIR) + strlen(path) + 2));
    sprintf(buffer, "%s/%s", TARGET_DIR, path);

    flag = symlink(buffer, CURRENT_JDK_PATH);
    if(flag != 0){
        perror(buffer);
    }
    free(buffer);

    return flag;
}

int change_link(char *version){
    int flag = unlink_old();
    if(flag == 0){
        link_new(version);
    }
    return flag;
}

int change_version(char *version){
    int value = check_exists(version);
    if(value == 0){
        value = change_link(version);
    }
    return value;
}

/*
 * check name is satisfying certain pattern.
 */
int check_name(int length, char *name){
    int i;
    int flag = 1;
    for(i = 0; i < length; i++){
        if(strchr(VERSION_PATTERN, name[i]) == NULL){
            flag = 0;
            break;
        }
    }
    return flag;
}

char *find_current_version(char *current){
    char *buffer;
    int flag;
    int i;
    char *version;

    buffer = (char *)malloc(sizeof(char) * (MAXPATHLEN + 1));
    if(buffer == NULL){
        perror("memory");
        return NULL;
    }

    flag = readlink(CURRENT_JDK_PATH, buffer, MAXPATHLEN + 1);
    if(flag < 0){
        perror(CURRENT_JDK_PATH);
        return NULL;
    }

    version = strrchr(buffer, '/');
    version++;
    for(i = 0; version[i] != '\0'; i++){
        current[i] = version[i];
    }

    return current;
}

int list_versions(){
    DIR *dir;
    struct dirent *dp;
    char current[64];
    int i;

    for(i = 0; i < 64; i++){
        current[i] = '\0';
    }

    find_current_version(current);

    if((dir = opendir(TARGET_DIR)) == NULL){
        perror(TARGET_DIR);
        return 1;
    }
    while((dp = readdir(dir)) != NULL){
        if(strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".") == 0){
            continue;
        }
        if(check_name(dp->d_namlen, dp->d_name) == 1){
            printf("%s %s\n", dp->d_name, (strcmp(dp->d_name, current) == 0)?"(*)":"");
        }
    }
    return 0;
}

void show_version(char *argv){
    printf("%s version %s\n", argv, VERSION_STRING);
}

void show_help(char *argv){
    printf("Usage: %s [OPTIONS] [<version>]\n", argv);
    printf("OPTIONS\n");
    printf("    -v, --version: print the version and exit.\n");
    printf("    -h, --help:    print this message.\n\n");
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
    printf("fatal error: unknown command");
    return 1;
}

int main(int argc, char *argv[]){
    int type = parse_option(argc, argv);

    return execute_option(type, argc, argv);
}
