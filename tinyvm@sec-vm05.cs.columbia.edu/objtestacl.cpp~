//
//  objtestacl.cpp
//  
//
//  Created by yihong dai on 9/21/14.
//
//

#include "objtestacl.h"
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
	char *val;
	int containBit;
	int userGroup = 0;
	getUser_Group(usr,group);
	if (lengthCheck(argv[argc-1])) {
		fprintf(stderr, "filename length limit exceed\n"); 
		exit(1);
	}
	/* parse the argument passed in and did some sanity check
	 * on the user input
	 */
	aFlag = 0,lFlag = 0 ;
	parseCommand(argc,argv,aFlag,lFlag,operation);
 	/* check if some options exists. 
	For example -u and -g must appear in the user input */
	if (aFlag != 1 || lFlag == 1 || argc != 4) {
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
	val = NULL;

	/* find the permission list for certain group and user by reading ACL file*/
	cout<<temp<<endl;
	findPermission(temp,(char *)usr.c_str(),group,&val);
	if(val == NULL){
		printf("\ndenied\n");
		exit(0);
	}
	/* check if a certain permission exists in the permission list*/
	int haveorNot  = checkPermission(operation,val);
	free(val);
	if(haveorNot!= 1)
	{
		printf("\ndenied\n");
		return 0;
	}
	printf("\nallowed\n");
	return 0;

}
