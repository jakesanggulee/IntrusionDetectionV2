#include <stdio.h>

#define DEF_PORT	9190
#define DEF_WIDTH	52
#define DEF_HEIGHT	52
#define GOOGLE		1

void default_settings(void)
{
FILE* fd = fopen(".settings","wt");

if(fd !=NULL){
fprintf(fd,"PORT	%d\n",DEF_PORT);
fprintf(fd,"DATA_WIDTH	%d\n",DEF_WIDTH);
fprintf(fd,"DATA_HEIGHT	%d\n",DEF_HEIGHT);
fprintf(fd,"GOOGLE_PUSH_ALARM	%d\n",GOOGLE);

fclose(fd);
}


}
