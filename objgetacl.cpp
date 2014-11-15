//
//  objgetacl.cpp
//  
//
//  Created by yihong dai on 9/20/14.
//
//

#include "objgetacl.h"
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

/* This main function check if a certain usr have certain permssion
 * by checking the ACL file 
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
	string usr("") ;
	vector<string> group;
	char operation;
	int containBit;
	getUser_Group(usr,group);
	int userGroup = 0;
	if (lengthCheck(argv[argc-1])){
		fprintf(stderr, "filename length limit exceed\n"); 
		exit(1);
	}
	/* parse the argument passed in and did some sanity check
	 * on the user input
	 */
	aFlag = 0,lFlag = 0 ;

	int kflag = 0;  string passwd("");
	parseCommand(argc,argv,aFlag,lFlag,operation,kflag,passwd);
	/* check if some options exists. 
	For example -u and -g must appear in the user input */
	if(aFlag == 1 || lFlag == 1 || argc != 2 || kflag == 1){
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
	/* check if we have read permission to read the ACL*/
	int haveorNot  = checkPermission('v',val);
	if(haveorNot != 1)
	{
		fprintf(stderr,"do not have permission");
		exit(EXIT_FAILURE);
	}
	FILE *openACL;
	
	openACL = fopen(temp.c_str(),"r");
	if(openACL == NULL){
		fprintf(stderr,"no such file\n");
		exit(EXIT_FAILURE);
	}
	char *buffer = NULL;;
	size_t dum;
	int revalue;
	cout<<"\n";
	while((revalue = (int)getline(&buffer,&dum,openACL))!= -1)
			cout<<buffer;
	if(buffer)
		free(buffer);
	fclose(openACL);
	
	free(val);
	return 0;
		
}
