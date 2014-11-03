#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

void clearbuf(char *buf)
{
    while (*buf) {
        *buf = 0;
        buf++;
    }
}

int main()
{
    int i, j, found, macvir;
    char buf[1000], fname[100], c, filesig[] = { 'P', 'K', 3, 4, 0 }; // all m$ office files are viruses, bwahahaha !
    FILE *fp;
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    dir = opendir("./virfiles");
    while ((ent = readdir(dir)) != NULL) {
        stat(ent->d_name, &st);
        strcpy(fname, "./virfiles/");
        strcat(fname, ent->d_name);
        
        if (ent->d_type == 8) {
            fp = fopen(fname, "rb");
            j = 0;
            found = 0;
            macvir = 1;
            while (!feof(fp) && !found) {
                fread(&c, 1, 1, fp);
                if (j < 4) {
                    if (filesig[j++] != c) {
                        macvir = 0;
                    }
                }
                if (j == 4 && macvir) {
                    printf("Found a macro virus: %s\n", fname);
                    break;
                }
                i = 0;
                while (isprint(c) && j > 3) {
                    buf[i++] = c;
                    fread(&c, 1, 1, fp);
                    if (i > 999)
                        break;
                    if (strstr(buf, "mbr virus")) {
                        printf("Found an mbr virus: %s\n", fname);
                        found = 1;
                        clearbuf(buf);
                        break;
                    } else if (strstr(buf, "file infecter")) {
                        printf("Found a File infecter: %s\n", fname);
                        found = 1;
                        clearbuf(buf);
                        break;
                    }
                }
            }
            fclose(fp);
        }
    }
    return 0;  // There, proud like McAfee !
}
