#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#include "chjdkver.h"

int check_exists_dir(char *path){
    struct stat *statbuf;
    int flag;

    statbuf = (struct stat *)malloc(sizeof(struct stat));

    flag = stat(path, statbuf);
    if(flag != 0){
        perror(path);
    }
    if((statbuf->st_mode & S_IFDIR) != S_IFDIR){
        flag = 1;
    }

    free(statbuf);
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
    int flag;

    flag = symlink(path, CURRENT_JDK_PATH);
    if(flag != 0){
        perror(path);
    }

    return flag;
}

int change_link(char *path){
    int flag = unlink_old();
    if(flag == 0){
        link_new(path);
    }
    return flag;
}

int change_version(char *version){
    char *buffer;
    int value;

    buffer = (char *)malloc(sizeof(char) * (strlen(TARGET_DIR) + strlen(version) + 2));
    sprintf(buffer, "%s/%s", TARGET_DIR, version);

    value = check_exists_dir(buffer);
    if(value == 0){
        value = change_link(buffer);
    }
    free(buffer);

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

