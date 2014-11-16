/*****************************************************
 * FILE NAME: help.cpp
 *
 * Created on: Sept 25 2014
 * Author: Yihong Dai
 * this function creates new file for specified user
 *****************************************************/
#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <dirent.h>
#include "help.h"
#include <pwd.h>
#include <grp.h>
#include "functionCall.h"
#include <vector>

using namespace std;




/* This main function parse the use input and does the sanity check
 *  on input, and  create a file or overwrite a file if certain 
 * permission is possessed by the specified user. 
 */
void store_encryptedkey_iv_iv(unsigned char *, unsigned char *, unsigned char *, string);
int main(int argc, const char * argv[])
{
	/* uFlag stands for the user option
	 * gFlag stands for the group otion
	* aFlag stands for the opertaion
	* lFlag stands for -l option in objlist
	*/	
	int  aFlag;
	int  lFlag;
	int containBit;
	if (lengthCheck(argv[argc-1])){
		fprintf(stderr, "filename length limit exceed\n"); 
		exit(1);
	}
	/* the usr, operation and group store the user inputs 
	 * and several file steams are created to open or overwrite
	 * the file 		
	 */
	string usr("") ;
	vector<string> group;
	
	char operation;
	string fileNameACL("");
	char *storeVal;
	int overWritePermission;
	//ofstream newFile ;
	FILE *filestream;
	FILE *aclList;
	int overWrite;
	string fileName("");
	getUser_Group(usr,group);
	int userGroup = 0;
	/* parse the argument passed in and did some sanity check
	 * on the user input
	 */	
	aFlag = 0,lFlag = 0, overWrite = 0;
	

	/* added in for third assignment 
	* =----------------------------
	*/	


	int kFlag = 0;
	string password("");
	parseCommand(argc,argv,aFlag,lFlag,operation,kFlag,password);
	if(!kFlag) {
		fprintf(stderr, "missing password\n");
		exit(EXIT_FAILURE);
	}

	cout<<"our passwd is ----->"<< password << "<--------"<<endl;


	unsigned char * MD5digest = MD5Sequence(password);
		
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


	/* check if some options exists. */


	/* added for third homework ---------------------*/
	if(aFlag == 1 || lFlag == 1 || argc != 4){
		fprintf(stderr, "invalid argument input\n");		
		//perror("invalid argument input");
		exit(EXIT_FAILURE);
	}
	

	containBit = checkIfContainPlus((char *)argv[argc-1]);	
	if(containBit){
		if (sanityCheck((char *)argv[argc-1],1) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}
		string fileNameExist("/home/encryption/filesystem/");
		fileNameExist.append(argv[argc-1]);
		FILE  *openFIleTry;
		openFIleTry = fopen(fileNameExist.c_str(),"r");
		if (openFIleTry == NULL){
			fprintf(stderr,"no such file\n");
			exit(EXIT_FAILURE);
		}
		fclose(openFIleTry);		
		fileName.append(argv[argc-1]);
	} else{
		if (sanityCheck((char *)argv[argc-1],0) == 0){
			fprintf(stderr,"invalid filename argument");
			exit(EXIT_FAILURE);	
		}
		fileName.append(usr);
		addPathName(fileName,(char *)argv[argc-1],1,0,0);
	}
		
	

	storeVal = NULL;
	string relativePath("");
	relativePath.append("/home/encryption/filesystem/");
	relativePath.append(fileName);
	
	filestream = fopen(relativePath.c_str(),"r");
	if(filestream != NULL){
		overWrite = 1;
		fileNameACL.append("/home/encryption/filesystem/");
		fileNameACL.append(fileName);
		
		addPathName(fileNameACL,NULL,0,1,0);
		
		/*find if we have permission to overwrite the existing file
  		 * allowed when there is p character among the action list*/
		
		findPermission(fileNameACL, (char *)usr.c_str(),
			       group,&storeVal);
		if(storeVal == NULL){
			fprintf(stderr, "no usr group combo in the ACL\n");		
			exit(EXIT_FAILURE);
		}	
		/* check if permssion is contained among the permission list
		 */	
		overWritePermission = checkPermission('w',storeVal);
		free(storeVal);
		fclose(filestream);
		if(overWritePermission != 1){
			fprintf(stderr, "we dont have overwrite permssion\n");
			exit(EXIT_FAILURE);
		}
	}


	
	/* read file contents from the input file into the usr file
	 */
	unsigned char * iv_enc =  (unsigned char *) malloc(AES_BLOCK_SIZE+1);
	unsigned char * iv_dec = (unsigned char *) malloc(AES_BLOCK_SIZE+1);
 	unsigned char *aes_Key = (unsigned char *) malloc(AES_BLOCK_SIZE+1);
	
	memset(aes_Key, 0, AES_BLOCK_SIZE+1);
	memset(iv_enc,0,AES_BLOCK_SIZE+1);
	memset(iv_dec,0,AES_BLOCK_SIZE+1);

	/*randomly generate aes_key and iv_enc*/
	rand_gen(aes_Key);
	rand_gen(iv_enc);

	cout <<"the IV1 for objput  is  the following ---------------------------------->"<<endl;
	hex_print(iv_enc,16);
	cout <<"the IV1 for objput  is  the above ---------------------------------->"<<endl;
	memcpy(iv_dec, iv_enc, AES_BLOCK_SIZE);

	size_t cipher_length = 0;

	int  plaintext_len = 0;

	/* first write the stdin input into the file and
	obtain the file size. Then we write the ciphertext into file based on its file size*/	
	unsigned char *plaintext = write_Into_file_and_out(relativePath, plaintext_len);

	int encslength = ((plaintext_len + AES_BLOCK_SIZE) /AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
	
	
	unsigned char *ciphertext = (unsigned char *)malloc(encslength+1);
	memset(ciphertext,0,encslength+1);
	

   	cipher_length = encrypt(plaintext,plaintext_len, aes_Key,iv_enc,ciphertext);


	write_Into_file_cipher(relativePath,ciphertext, cipher_length);



	free(ciphertext);
	/* creates a new aclFile for the created file
	 */
	
	if(overWrite == 0){
		if (containBit){
			fprintf(stderr, "wrong filename input\n");
			exit(EXIT_FAILURE);
		}
		string aclListName("/home/encryption/filesystem/");
		aclListName.append(fileName);
		addPathName(aclListName,NULL,0,1,0);	
		aclList = fopen(aclListName.c_str(),"w+");
		if(aclList == NULL)
			exit(EXIT_FAILURE);
		fputs(usr.c_str(),aclList);
		fputs(".*	",aclList);
		fputs("rwxpv", aclList);
		fclose(aclList);
	}



	store_encryptedkey_iv_iv(MD5digest,iv_dec ,aes_Key , relativePath);

	
	cout <<"the aes_key is the following ----------------->"<<endl;
	hex_print(aes_Key, 16);
	cout <<"the aes_key is the above ----------------->"<<endl;
	cout <<"successful"<<endl;
	free(iv_enc);
	free(iv_dec);
	free(aes_Key);
	return 0;
}





/* this method is used to store the key, iv1 and iv2 into the file
 * create u1+doc+key  u1+doc+v1  u1+doc+v2
*/
void store_encryptedkey_iv_iv(unsigned char *MD5digest, unsigned char *iv_dec, unsigned char *aes_Key, string relativePath)
{

	string Key_path(relativePath);
	Key_path.append("+key");
	string v1_path(relativePath);
	v1_path.append("+v1");
	string v2_path(relativePath);
	v2_path.append("+v2");

	unsigned char *iv = (unsigned char *)malloc(AES_BLOCK_SIZE+1);
	unsigned char *iv_2 = (unsigned char *)malloc(AES_BLOCK_SIZE+1);
	memset(iv,0,AES_BLOCK_SIZE+1);	
	memset(iv_2,0,AES_BLOCK_SIZE+1);	

	int encrypted_key_length = ((16 + AES_BLOCK_SIZE) /AES_BLOCK_SIZE) * AES_BLOCK_SIZE;

	unsigned char *encypted_key = (unsigned char *)malloc(encrypted_key_length+1);
	memset(encypted_key,0,encrypted_key_length+1);

        int encrypt_length = encrypt_random_key(iv,iv_2, MD5digest, aes_Key,encypted_key);

	hex_print(iv_2, 16);

	FILE *keyList;
	keyList = fopen(Key_path.c_str(),"w+");
	if (keyList == NULL)
		exit(EXIT_FAILURE);


	fwrite(encypted_key,1,encrypted_key_length,keyList);


	fclose(keyList);
	
	FILE *v1List;
	v1List = fopen(v1_path.c_str(),"w+");
	if(keyList == NULL)
		exit(EXIT_FAILURE);

	fwrite(iv_dec,1,16,v1List);

	fclose(v1List);

	FILE *v2List;
	v2List = fopen(v2_path.c_str(),"w+");
	if(keyList == NULL)
		exit(EXIT_FAILURE);

	fwrite(iv_2,1,16,v2List);
	fclose(v2List);

	free(iv);free(iv_2);free(encypted_key);
}





