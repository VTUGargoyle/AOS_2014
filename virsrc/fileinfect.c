#include <stdio.h>
#include <unistd.h>

char sig[] = "file infecter";

int main()
{
    
    char c[] = "\xff\xff";
    FILE *fp = fopen("./mbr", "a");
    fwrite(c, 1, sizeof(c), fp);
    fclose(fp);
    printf("%s", sig);
    return 0;
}
