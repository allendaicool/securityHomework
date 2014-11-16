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
	if(aFlag == 1 || lFlag == 1 || argc != 4 || kflag != 1){
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
	//string file_dec_name ("");	
	string fileName("");
	if (containBit){
		if (sanityCheck((char *)argv[argc-1],1) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}	
	//	file_dec_name.append(argv[argc-1]);
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

		

		unsigned char *fake_digest = MD5Sequence(passwd);


		cout<<"fake_digest length is " << strlen((const char *)fake_digest)<<endl;


		string viDec(fileNameRelative);
		viDec.append("+v2");
		int iv2_length=0;
		unsigned char * iv2_enc = read_enc_key_iv_file (viDec,iv2_length); 
		
		string viDec1(fileNameRelative);
		viDec1.append("+v1");
		int iv1_length=0;
		unsigned char * iv1_enc = read_enc_key_iv_file (viDec1,iv1_length); 
	
		//cout<<" -----------------printde here is  " << iv2_length <<endl;
		//cout<<"iv2_enc length is " << strlen((const char *)iv2_enc )<<endl;

		string passwd_enc(fileNameRelative);
		passwd_enc.append("+key");
		int passwd_enc_length = 0;
		unsigned char *key_enc_file = read_enc_key_iv_file (passwd_enc ,passwd_enc_length); 
		cout<<"key_enc_file length is " << strlen((const char *)key_enc_file )<<endl;
		cout<<" -----------------printde here is  " << passwd_enc_length <<endl;
	

		unsigned char *real_key = (unsigned char *)malloc(33);
		memset(real_key,0,33);
	
      		int encrypt_len = decrypt(key_enc_file, passwd_enc_length, fake_digest, iv2_enc,real_key);
		real_key  = (unsigned char *)realloc(real_key,encrypt_len+1);
		real_key [encrypt_len] = '\0';
		cout<<"-----------------------fake key      is aaaaaaaaaaaaaaaaaa!!!!!!!"<<endl;
		hex_print(real_key, 16);
//		cout<<"the real value key is aaaaaaaaaaaaaaaa!!!!!!!!!"<<endl;
//		hex_print(aes_Key, 16);


		//if(compare_key_fakeKey(real_key,aes_Key,16))
		//	cout<<"they match-------------------"<<endl;
	//cout<<"the encrypt len is "<<encrypt_len<<endl;



	FILE * pFile;
 	long lSize;
  	char * buffer;
  	size_t result;

  	pFile = fopen (fileNameRelative.c_str(), "rb" );
 	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  	// obtain file size:
  	fseek (pFile , 0 , SEEK_END);
  	lSize = ftell (pFile);
  	rewind (pFile);
	//cout<<"the size of file is   " << lSize<<endl;
	
  	// allocate memory to contain the whole file:
	 buffer = (char*) malloc (sizeof(char)*lSize+1);
	//cout<<"it is ok here B"<<endl;
	 memset(buffer,0,sizeof(char)*lSize+1);
  	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  	// copy the file into the buffer:
  	result = fread (buffer,1,lSize,pFile);
  	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */

  // terminate
 	 fclose (pFile);



	cout<<"the result is so large ----------------------->" << result<<endl;
	unsigned char *plaintext2 = (unsigned char*)malloc(sizeof(char)*result+1);
	memset(plaintext2,0,sizeof(char)*result+1);
	
//aes_Key

	
	int decslength = decrypt((unsigned char *)buffer,result, real_key,
 	iv1_enc, plaintext2);
	
	cout<<"decryption leng is " << decslength<<endl;
	cout<<"the real decryption length is " << strlen((const char *)plaintext2)<<endl;
	plaintext2 = (unsigned char*)realloc(plaintext2,decslength+1);
	plaintext2[decslength] = '\0';
	
	for ( int i = 0 ; i < decslength ; i++)
		cout<<plaintext2[i];
	
	
	//printf("our text file is %s \n", plaintext2);
	//cout<<"the real decryption length for now is " << strlen((const char *)plaintext2)<<endl;

		cout<<"\n";
	}
	else{
		fprintf(stderr,"we dont have read permssion");
		exit(EXIT_FAILURE);
	}
	
	return 0;
	
}
