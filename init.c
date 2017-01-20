#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARRAYSIZE(n,type)  sizeof(n)/sizeof(type)
#define DEFALUT_LEN        BUFSIZ
#define BEGIN              "{\n"
#define END                "}\n"

FILE *fp;
char *g_list[100];  //array to save the list
int  g_listlen =0;

typedef struct {
	char *name;        //item name
	char *defVal;    //item default value
	int  type;         //item type: object, list etc.
}raw;

typedef enum {
	J_STRING,
	J_LIST,
	J_OBJECT
}jtype;


raw jInit[] = {
	{"name", 	   NULL,	J_STRING},
	{"version",    "1.0.0", J_STRING},
	{"description",NULL,	J_STRING},
	{"scripts",	   "test",  J_OBJECT},
	{"keywords",   NULL,	J_LIST},
	{"author", 	   NULL,	J_STRING},
	{"license",	   "MIT",	J_STRING}
};

char * Strdup(char *str){
	int len = strlen(str) + 1;
	char *buf = malloc(len);
	if(buf) memcpy(buf,str,len);
	return buf;
}


void getwords(char *word){
	char c = getchar();
	int i = 0;

	while(c != '\n'){
		word[i++] = c;
		c = getchar();
	}

	word[i] = '\0';
}


void GetInput(raw *item){

	//get the input
	char input[DEFALUT_LEN];
	getwords(input);

	//if the input is newline we will check the default value
	if(strcmp(input,"") == 0){
		if(item->defVal == NULL){
			item->defVal = Strdup(" \n"); // " \n"
		}
	}else{
		item->defVal = Strdup(input);
	}

}

void WriteStr(raw item){
	char *s;

	int len = strlen(item.name) + strlen(item.defVal) + 9;
	s = malloc(len);
	if(!s){
		printf("cannot malloc space\n");
		exit(1);
	}
	
	if(strcmp(item.name,"license") == 0){
		sprintf(s, "\"%s\": \"%s\"\n",item.name, item.defVal);
		fwrite(s,len-2,1,fp);
	}else{	
		sprintf(s, "\"%s\": \"%s\",\n",item.name, item.defVal);
		fwrite(s,len-1,1,fp);
	}
}

void split(char *list){
	char *token;
	const char s[1] = " ";
	int i = 0;

	token = strtok(list,s);

	while(token){
		 g_list[i] = token;
		 i++;
		 token = strtok(NULL, s);
	}

	g_listlen = i;
}

int IsAllSpace(char *list){
	char *p = list;
 	 int bSpace = 0;

	while(*p++){
		if(!isspace(*p))
			return bSpace;
	}

	bSpace = 1;
	return bSpace;
}

void WriteList(raw item){
	//[
	// "xxx",
	// "xxx",
	//]
	char *s;
	int i;
	s = malloc(200);
	if(s == NULL){
		printf("cannot malloc space for s\n");
		exit(1);
	}

	if((strcmp(item.defVal," \n") == 0) || (IsAllSpace(item.defVal))){
		sprintf(s, "\"%s\": \"%s\",\n",item.name, item.defVal);
	}else{

		split(item.defVal);
		sprintf(s,"\"%s\": [\n", item.name,item.defVal);
		fwrite(s,strlen(s),1,fp);
		for(i = 0; i < g_listlen; i++){
				if(i == g_listlen-1) {
					sprintf(s,"\t\"%s\"\n",g_list[i]);
				}else {
					sprintf(s,"\t\"%s\",\n",g_list[i]);
				}
				fwrite(s,strlen(s),1,fp);
		}
		fwrite("],\n",3,1,fp);
	}

}

void WriteObj(raw item){
	// {
	//  xxx:  xxxx,
 	// }
	char *s;

	s = malloc(200);
	if(s == NULL){
		printf("cannot malloc space for s\n");
		exit(0);
	}

	if(strcmp(item.defVal,"test") == 0){
		sprintf(s,"\"%s\": {\n \t\"test\": \"test command not specify\"\n },\n", item.name);
	}else {
		sprintf(s,"\"%s\": {\n \"test\":\"%s\"\n },\n",item.name, item.defVal);
	}

	fwrite(s,strlen(s),1,fp);

}

void FormatString(raw item){

	switch(item.type) {
		case J_STRING:
			WriteStr(item);
			break;
		case J_LIST:
			WriteList(item);
			break;
		case J_OBJECT:
			WriteObj(item);
			break;
	}


}



void getUserInput(raw item){


	if(item.defVal == NULL)
		printf("%s: ", item.name);
	else
		printf("%s(%s): ",item.name, item.defVal);

	GetInput(&item);

	FormatString(item);

}


int main(int argc, char **argv){

	int i = 0;

	fp = fopen("package.json", "w+");
	if(fp == NULL){
		printf("cannot create package.json file...\n");
		exit(1);
	}

	fwrite(BEGIN,2,1,fp);
	for(i = 0;i < ARRAYSIZE(jInit,raw); i++)
	{
		getUserInput(jInit[i]);
	}

	fwrite(END,2,1,fp);
	fclose(fp);
	return 0;
}
