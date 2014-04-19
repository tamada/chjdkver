#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include <chjdkver.h>

typedef struct _list {
    char *jdkname;
    char *linkto;
    struct _list *next;
} jdks;

jdks *create_list(jdks *first){
    DIR *dir;
    struct dirent *dp;

    if((dir = opendir(TARGET_DIR)) == NULL){
        perror(TARGET_DIR);
        return NULL;
    }
    while((dp = readdir(dir)) != NULL){
        jdks *item;

        if(strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".") == 0){
            continue;
        }
        item = (jdks *)malloc(sizeof(jdks));
        item->jdkname = (char *)malloc(sizeof(char) * (strlen(dp->d_name) + 1));
        strcpy(item->jdkname, dp->d_name);
        item->next = first;
        first = item;
    }
    closedir(dir);
    return first;
}

/**
 * This function checks given base parameter ends with given str parameter.
 * If base ends with str, this function returns 1, otherwise returns 0.
 */
int ends_with(char *base, char *str){
    int i, j;
    int length = strlen(base);
    int match = strlen(str);

    for(i = j = 0; i < length; i++){
        if(base[i] == str[j]){
            j++;
        }
        else{
            j = 0;
        }
        if(match == j){
            return 1;
        }
    }
    return 0;
}

jdks *sort_list(jdks *first){
    jdks *current, *next;
    current = first;
    next = first->next;

    while(next != NULL){
        while(next != NULL){
            if(strcmp(current->jdkname, next->jdkname) < 0){
                char *buffer = current->jdkname;
                current->jdkname = next->jdkname;
                next->jdkname = buffer;
            }
            next = next->next;
        }
        current = current->next;
        next = current->next;
    }
    return first;
}

jdks *build_item(char *newname, char *linkto){
    jdks *item;
    item = (jdks *)malloc(sizeof(jdks));
    item->jdkname = (char *)malloc(sizeof(char) * (strlen(newname) + 1));
    item->linkto = (char *)malloc(sizeof(char) * (strlen(linkto) + 1));
    strcpy(item->jdkname, newname);
    strcpy(item->linkto, linkto);

    return item;
}

jdks *insert_item(jdks *first, char *newname, char *linkto){
    jdks *current, *prev;
    jdks *item = build_item(newname, linkto);

    prev = NULL;
    current = first;

    while(current != NULL){
        if(strcmp(current->jdkname, newname) < 0){
            if(prev == NULL){
                first = item;
            }
            else{
                item->next = prev->next;
                prev->next = item;
            }
            break;
        }
        prev = current;
        current = current->next;
    }
    if(current == NULL){
        item->next = NULL;
        prev->next = item;
    }

    return first;
}

int find_name(jdks *first, char *name){
    jdks *current;
    current = first;

    while(current != NULL){
        if(strcmp(name, current->jdkname) == 0){
            return 1;
        }
        current = current->next;
    }
    return 0;
}

char *get_short_name(char *base, char *dest){
    int i = 0, j = 0;
    int length = strlen(base);
    for(i = j = 0; i < length; i++){
        if(strchr(VERSION_PATTERN, base[i]) != NULL){
            dest[j] = base[i];
            j++;
        }
        else{
            dest[j] = '\0';
        }
    }
    for(i = strlen(dest); i >= 0; i--){
        if(dest[i] >= '0' && dest[i] <= '9'){
            break;
        }
        else{
            dest[i] = '\0';
        }
    }
    return dest;
}

char *more_shorten(char *base, char *dest){
    int i;

    strcpy(dest, base);
    for(i = strlen(dest) - 1; i >= 0; i--){
        if(dest[i] >= '0' && dest[i] <= '9'){
            dest[i] = '\0';
        }
        else{
            break;
        }
    }
    for(i = strlen(dest); i >= 0; i--){
        if(dest[i] >= '0' && dest[i] <= '9'){
            break;
        }
        else{
            dest[i] = '\0';
        }
    }
    if(strlen(dest) == 1){
        dest[i] = '\0';
        return NULL;
    }
    return dest;
}

void show_list(jdks *first){
    jdks *current;
    current = first;

    while(current != NULL){
        printf("%s", current->jdkname);
        if(current->linkto != NULL){
            printf(" <- %s", current->linkto);
        }
        printf("\n");

        current = current->next;
    }
}

void build_link_name(jdks *first){
    jdks *current;
    current = first;

    while(current != NULL){
        char name[128];
        if(ends_with(current->jdkname, ".jdk") == 1){
            get_short_name(current->jdkname, name);
            if(find_name(first, name) == 0){
                insert_item(first, name, current->jdkname);
            }
        }
        else{
            if(more_shorten(current->jdkname, name) != NULL){
                if(find_name(first, name) == 0){
                    insert_item(first, name, current->jdkname);
                }
            }
        }
        current = current->next;
    }
}

void free_jdks(jdks *first){
    jdks *current;
    current = first;

    while(current != NULL){
        jdks *item = current;
        current = current->next;
        if(item->linkto != NULL){
            free(item->linkto);
        }
        free(item->jdkname);
        free(item);
    }
}

void link_all(jdks *first){
    jdks *current = first;

    while(current != NULL){
        if(current->linkto != NULL){
            char *path;
            int flag;
            path = (char *)malloc(
                sizeof(char) * (strlen(TARGET_DIR) + strlen(current->jdkname) + 2)
            );
            sprintf(path, "%s/%s", TARGET_DIR, current->jdkname);

            flag = symlink(current->linkto, path);
            if(flag == 0){
                printf("%s -> %s/%s\n", current->jdkname, TARGET_DIR, current->linkto);
            }
            else{
                perror(path);
            }
            free(path);
        }
        current = current->next;
    }
}

int create_symlink_for_jdks(){
    jdks *first = NULL;

    first = create_list(first);
    first = sort_list(first);

    build_link_name(first);
    link_all(first);

    free_jdks(first);
    return 1;
}

int init_jdks(){
    return create_symlink_for_jdks();
}

