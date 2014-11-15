//
//  objsetacl.cpp
//  
//
//  Created by yihong dai on 9/20/14.
//
//

#include "objsetacl.h"
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

/* This main function sets the acl file
 * if certain usr and group have 'p' permssion
 */
int main(int argc, const char * argv[])
{
 	/* uFlag stands for the user option
	 * gFlag stands for the group otion
	 * aFlag stands for the opertaion
	 * lFlag stands for -l option in objlist
	 */
	int containBit;
	int  aFlag;
	int  lFlag;
	string usr("") ;
	vector<string> group;
	char operation;
	int value ;
	size_t dum;
	char * bufferReadIn;
	getUser_Group(usr,group);
	int userGroup = 0;
	if (lengthCheck(argv[argc-1])){
		fprintf(stderr, "filename length limit exceed\n"); 
		exit(1);
	}
	aFlag = 0,lFlag = 0 ;
	
	int kflag = 0;  string passwd("");
	parseCommand(argc,argv,aFlag,lFlag,operation,kflag,passwd);
	/* check if some options exists. 
	For example -u and -g must appear in the user input */
	if(aFlag == 1 || lFlag == 1 || argc != 2 || kflag == 1){
		fprintf(stderr,"invalid argument input");
		exit(EXIT_FAILURE);
	}
	/* we do  allow shell redirect in the objget*/
	/*if(checkShellRedirect()){
		fprintf(stderr,"There is no shell redirect. program stopped");
		exit(EXIT_FAILURE);
	}*/
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

	
	string temp("filesystem/");
	containBit = checkIfContainPlus((char *)argv[argc-1]);
	if (containBit){	
		if (sanityCheck((char *)argv[argc-1],1) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}
		temp.append(argv[argc-1]);
		temp.append("+ACL");
	}
	else{
		temp.append(usr);
		if(sanityCheck((char *)argv[argc-1],0) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}
		addPathName(temp,(char *)argv[argc-1],1,1,0);
	}
	
	char *val = NULL;
	/* find the permission list for certain group and user by reading ACL file*/
	findPermission(temp,(char *)usr.c_str(),group,&val);
	if(val == NULL){
		fprintf(stderr,"permission denied\n");
		exit(EXIT_FAILURE);
	}
	/* check if we have change permission to change the ACL*/
	int haveorNot = checkPermission('p',val);
	free(val);
	if(haveorNot!= 1){
		fprintf(stderr,"do not have permission\n");
		exit(EXIT_FAILURE);
	}
	FILE *filestream;
	filestream = fopen(temp.c_str(),"w");
	if(filestream == NULL)
		fprintf(stderr,"something happends");
	
	bufferReadIn = NULL;
	/* dump the content from user input file into ACL file*/
	while((value = (int)getline(&bufferReadIn,&dum,stdin))!=-1){
		fputs(bufferReadIn,filestream);
		free(bufferReadIn);
		bufferReadIn = NULL;
	}
	fclose(filestream);
	return 0;	
}
