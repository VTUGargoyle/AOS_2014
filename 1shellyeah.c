#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* Function to get a substring: copies "len" characters starting from "begin"
 * if len == 0, copies from "begin" till the end 
 */
void substr(char *dst, char *src, int begin, int len)
{
	int tmplen = len;
	src += begin;
	if (!len) {
		while (*src) {
			*dst = *src;
			dst++;
			src++;
		}
	} else {
		while (tmplen) {
			*dst = *src;
			dst++;
			src++;
			tmplen--;
		}
	}
	*dst = 0;
}

int main()
{
	char cmdtmp[100];
	char cmd[100], path[100];
	printf("$ ");
	fgets(cmd, 100, stdin);
	while (strncmp(cmd, "quit", 4) != 0) {
		substr(cmdtmp, cmd, 0, 3);
		if (strncmp(cmdtmp, "cd", 2) == 0) {
			substr(path, cmd, 3, 0);
			strtok(path, "\n");	// To remove the newline 
			chdir(path);
		} else if (strncmp(cmdtmp, "dir", 3) == 0) {	// let dir == ls, etc.
			substr(path, cmd, 3, 0);
			strcpy(cmdtmp, "ls");
			strcat(cmdtmp, path);
			system(cmdtmp);
		} else if (strncmp(cmdtmp, "md", 2) == 0) {
			substr(path, cmd, 3, 0);
			strcpy(cmdtmp, "mkdir ");
			strcat(cmdtmp, path);
			system(cmdtmp);
		} else if (strncmp(cmdtmp, "rd", 2) == 0) {
			substr(path, cmd, 3, 0);
			strcpy(cmdtmp, "rmdir ");
			strcat(cmdtmp, path);
			system(cmdtmp);
		} else {
			system(cmd);
		}
		printf("$ ");
		fgets(cmd, 100, stdin);
	}
	return 0;
}
