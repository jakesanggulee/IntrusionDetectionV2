#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define SETTINGS 4


void read_settings(int *settings)
{
char str[SETTINGS][30]={"PORT	%d",
		"DATA_WIDTH	%d",
		"DATA_HEIGHT	%d",
		"GOOGLE_PUSH_ALARM	%d"
		};
FILE *fd = fopen(".settings","rt");

if(fd ==NULL){
	printf(".settings not found!\n");
	sleep(0.3);
	printf("Using Default Settings...\n");

}
else
	for(int i=0; i < SETTINGS; i++)
		fscanf(fd,str[i],&(settings[i]));


fclose(fd);

}
