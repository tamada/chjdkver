#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#include "chjdkver.h"

#define COMMANDS_PATH "/System/Library/Frameworks/JavaVM.framework/Versions/Current/Commands"
#define TARGET_PATH   "/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Contents/Home/bin"
#define TARGET_PATH2  "../CurrentJDK/Contents/Home/bin"

/**
 * check path is exists and check type.
 * return value is as follows.
 *    -1: not exists.
 *    0: exists but type is not specified type.
 *    1: exists and type is specified type.
 */
int check_exists(char *path, mode_t mask){
    struct stat *statbuf;
    int flag = 0;

    statbuf = (struct stat *)malloc(sizeof(struct stat));
    flag = lstat(path, statbuf);

    if(flag == -1){
        flag = -1;
    }
    else{
        if((statbuf->st_mode & mask) == mask){
            flag = 1;
        }
        else{
            flag = 0;
        }
    }

    free(statbuf);
    return flag;
}

int check_link(){
    char buffer[MAXPATHLEN + 1];
    int size;

    size = readlink(COMMANDS_PATH, buffer, MAXPATHLEN + 1);
    if(size < 0){
        perror("readlink");
        return -1;
    }

    if(strcmp(buffer, TARGET_PATH) == 0 ||
       strcmp(buffer, TARGET_PATH2) == 0){
        return 1;
    }
    return 0;
}

int backup_commands_dir(){
    char *newdir;
    int flag;
    int return_value = 0;

    newdir = (char *)malloc(sizeof(char) * (strlen(COMMANDS_PATH) + 6));
    sprintf(newdir, "%s.back", COMMANDS_PATH);

    flag = rename(COMMANDS_PATH, newdir);
    if(flag != 0){
        perror(newdir);
    }
    else{
        return_value = 1;
    }
    free(newdir);

    return return_value;
}

int create_symlink_for_commands(){
    int flag = symlink(TARGET_PATH2, COMMANDS_PATH);
    return flag;
}

int init_env(){
    int return_value = 0;

    int commands_dir = check_exists(COMMANDS_PATH, S_IFLNK);

    if(commands_dir == -1){
        create_symlink_for_commands();
        printf("preparation is done.\n");
    }
    else if(commands_dir == 0){
        int flag = backup_commands_dir();
        if(flag == 0){
            printf("back up of Command dir is failed\n");
            return 0;
        }
        create_symlink_for_commands();
        printf("preparation is done.\n");
        return_value = 1;
    }
    else if(commands_dir == 1){
        int flag = check_link();
        if(flag == 0){
            printf("invalid link at %s\n", COMMANDS_PATH);
        }
        else if(flag == 1){
            printf("preparation is already done.\n");
            return_value = 1;
        }
    }

    return return_value;
}
