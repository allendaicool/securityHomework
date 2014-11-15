//
//  objget.cpp
//  
//
//  Created by yihong dai on 9/20/14.
//
//

#include "objget.h"
#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include "functionCall.h"
#include <pwd.h>
#include <grp.h>
#include <vector>
using namespace std;

/* This main function parse the use input and does the sanity check
 *  on input, and display the file content in the stdout if we have permission
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
	char operation;
	int readPermission;
	char *val;
	int containBit;
	FILE *filestream;
	string usr("") ;
	vector<string> group;
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
	if(aFlag == 1 || lFlag == 1 || argc != 2 || kflag == 1){
		fprintf(stderr, "invalid argument input");
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
	/* append string after the username and get a new string
	 * for example user+doc1 as name for doc file for user*/
	
	containBit = checkIfContainPlus((char *)argv[argc-1]);
	
	string fileName("");
	if (containBit){
		if (sanityCheck((char *)argv[argc-1],1) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}	
		fileName.append(argv[argc-1]);
		fileName.append("+ACL");
	}
	else{
		fileName.append(usr);
		if(sanityCheck((char *)argv[argc-1],0) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}
		addPathName(fileName,(char *)argv[argc-1],1,0,0);
	}
	
	

	string fileNameACL(fileName);
	if (!containBit)
		addPathName(fileNameACL,NULL,0,1,0);
	val = NULL;

	string relativePath("");
	relativePath.append("filesystem/");
	relativePath.append(fileNameACL);
	/* find if we have permission to display the file*/
	findPermission(relativePath, (char *)usr.c_str(),group
		       ,&val);
	if(val == NULL){
		fprintf(stderr,"permission denied\n");
		exit(EXIT_FAILURE);
	}
	
	readPermission = checkPermission('r',val);
	free(val);
	/* if we have permssion, then we display the file content in stdout*/
	if(readPermission == 1){
		string fileNameRelative("");
		fileNameRelative.append("filesystem/");
		if (containBit)
			fileNameRelative.append(argv[argc-1]);	
		else
			fileNameRelative.append(fileName);

		filestream = fopen(fileNameRelative.c_str(),"r");
		if(filestream == NULL){
			fprintf(stderr,"no suc file\n");
			exit(EXIT_FAILURE);
		}
		char *buffer = NULL;;
		size_t dum;
		int revalue;
		cout<<"\n";
		while((revalue = (int)getline(&buffer,&dum,filestream))!= -1)
		{
			cout<<buffer;
		}
		if(buffer)
			free(buffer);
		fclose(filestream);
		cout<<"\n";
	}
	else{
		fprintf(stderr,"we dont have read permssion");
		exit(EXIT_FAILURE);
	}
	
	return 0;
	
}
