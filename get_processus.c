#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>

int filter(const struct dirent *name)
{
    regex_t reegex;
    int reti;
    reti = regcomp(&reegex, "^[0-9]*$", 0);

    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    reti = regexec(&reegex, name->d_name, 0, NULL, 0);
    if (!reti)
        return 1;
    else
        return 0;
}


int main(int argc, char *argv[]) {

    struct dirent **namelist;
    int n;

    /*
    List files in the current working directory.
    These are the files that are in the directory
    from which the process was run.
    */
    n = scandir("/proc", &namelist, filter, alphasort);
    if (n == -1) {
    perror("scandir");
    exit(EXIT_FAILURE);
    }

    //read /proc/pid/status first line file using the pid from the list
    while (n--) {
        char path[100];
        FILE *fp;
        char line[100];
        char *token;
        char *name = namelist[n]->d_name;
        sprintf(path, "/proc/%s/status", name);
        fp = fopen(path, "r");
        if (fp == NULL) {
            printf("Failed to run command\n" );
            exit(1);
        }
        while (fgets(line, sizeof(line), fp) != NULL) {
            token = strtok(line, ":\t");
            if (strcmp(token, "Name") == 0) {
                token = strtok(NULL, ":\t");
                printf("%s : %s", name, token);
            }
        }
        fclose(fp);
        free(namelist[n]);
    }
    free(namelist);

    exit(EXIT_SUCCESS);
}