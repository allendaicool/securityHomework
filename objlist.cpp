//
//  objlist.cpp
//  
//
//  Created by yihong dai on 9/20/14.
//
//

#include "objlist.h"
#include <iostream>
#include <string.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include "functionCall.h"
#include <vector>
using namespace std;

/* This main function lists all the file possessed by
 * certain usr and might display the file size as well
 */
int main(int argc, const char * argv[])
{
	/* uFlag stands for the user option
	 * gFlag stands for the group otion
	 * aFlag stands for the opertaion
	 * lFlag stands for -l option in objlist
	 */
	int  aFlag;
	int  lFlag;
	string usr("");
	vector<string> group;
	char operation;
	struct dirent *entry;
	struct stat st;
	getUser_Group(usr,group);
	int userGroup = 0;
	/* parse the argument passed in and did some sanity check
	 * on the user input
	 */
	aFlag = 0,lFlag = 0 ;

	int kflag = 0;  string passwd("");
	parseCommand(argc,argv,aFlag,lFlag,operation,kflag,passwd);
	/* check if some options exists. 
	For example -u and -g must appear in the user input */
	if(aFlag == 1 || argc > 2 || kflag == 1){
		fprintf(stderr,"invalid argument input");
		exit(EXIT_FAILURE);
	}

	/* check if user and group combination exists in user+group file*/
	for(int i = 0 ; i < (int)group.size(); i++){	
		if(checkifUserGroup((char *)usr.c_str(), (char *)(group.at(i).c_str()),0) == 1){
			userGroup = 1;
			break;
		}
	}
	if(!userGroup){
		fprintf(stderr, "There is no such usr and group combination");
		exit(EXIT_FAILURE);
	}
	/* we do not allow shell redirect in the objget*/
	/*if(!checkShellRedirect()){
		fprintf(stderr,"don't allow shell redirect\n");		
		exit(EXIT_FAILURE);
	}*/
	/* open the current directory to iterate all the files
	 * contained in that folder and search for the file 
	 * belonged to the user.
	 */

	if(chdir("filesystem/") == -1){
		fprintf(stderr,"chdir() error");
                exit(EXIT_FAILURE);

	}
	DIR *currentDir;
	char *currentDirName = (char *)"./";
	if ((currentDir = opendir(currentDirName)) == NULL){
		fprintf(stderr,"opendir() error");
		exit(EXIT_FAILURE);
	}

	/* ignore the ACL file other file
	 * prints off the filename and maybe its size*/
	while ((entry = readdir(currentDir)) != NULL )
	{
		if(strstr(entry->d_name,usr.c_str()) != NULL &&
		   strstr(entry->d_name,"ACL") == NULL) {
			unsigned long long size;

			stat(entry->d_name, &st);
			size = (unsigned long long)st.st_size;
			char * toBeprint = (char *)malloc(sizeof(char)*(
					strlen(entry->d_name)+1));
			strncpy(toBeprint,entry->d_name,strlen(entry->d_name));
			toBeprint[strlen(entry->d_name)] = '\0';
					
			char *limiter = (char *)"+";
			char *firstName = strtok(toBeprint,limiter);
			firstName = strtok(NULL,limiter);
			if(lFlag)
				printf("\n %s     %llu bytes \n",firstName,size);
			else
				printf("\n %s \n ",firstName);

			if(toBeprint)
				free(toBeprint);
		}
	}
	closedir(currentDir);
	return 0 ;
}
