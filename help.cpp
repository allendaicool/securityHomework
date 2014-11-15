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


unsigned char * MD5Sequence(string);
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

	cout<<"our passwd is "<< password <<endl;


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
		string fileNameExist("filesystem/");
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
	relativePath.append("filesystem/");
	relativePath.append(fileName);
	
	filestream = fopen(relativePath.c_str(),"r");
	if(filestream != NULL){
		overWrite = 1;
		fileNameACL.append("filesystem/");
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



	
//	newFile.open(relativePath.c_str());
//	string bufferLine;
	
	/* read file contents from the input file into the usr file
	 */
	unsigned char iv_enc[AES_BLOCK_SIZE], iv_dec[AES_BLOCK_SIZE];;
 	unsigned char aes_Key[16];
	memset(aes_Key, 0, 16);
	rand_gen(aes_Key);
	rand_gen(iv_enc);
	memcpy(iv_dec, iv_enc, AES_BLOCK_SIZE);
	size_t cipher_length = 0;


	//string relativePath("");
	//relativePath.append("testcase");
	int  plaintext_len = 0;
	
	unsigned char *plaintext = write_Into_file_and_out(relativePath, plaintext_len);

	printf("the string length is %d  string is %s \n", plaintext_len,plaintext );

	cout<<"plaintext_len%AES_BLOCK_SIZE == 0 is " << plaintext_len%AES_BLOCK_SIZE <<endl;
	//(plaintext_len%AES_BLOCK_SIZE == 0)?plaintext_len:
	int encslength = ((plaintext_len + AES_BLOCK_SIZE) /AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
	cout<<"plaintext length is "<< 	plaintext_len<<endl;
	cout<<"our encslength is "<< encslength<<endl;
	unsigned char ciphertext[encslength];

	//unsigned char * enc_out =  AesEncryption(aes_Key, iv_enc, iv_dec,cipher_length);
   	cipher_length = encrypt(plaintext,plaintext_len, aes_Key,iv_enc,ciphertext);
	cout<<"	cipher_length is " << cipher_length<<endl;

	write_Into_file_cipher(relativePath,ciphertext, cipher_length);
	cout<<"ok here"<<endl;
	hex_print(ciphertext, cipher_length);

/*
	int flag = 0;

	
	while(getline(cin,bufferLine)){
		if(flag !=0){
		 	newFile<<"\n";
		}
		flag = 1;
		newFile<<bufferLine;
	}
	newFile.close();
*/










	/* creates a new aclFile for the created file
	 */
	
	if(overWrite == 0){
		if (containBit){
			fprintf(stderr, "wrong filename input\n");
			exit(EXIT_FAILURE);
		}
		string aclListName("filesystem/");
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


	FILE * pFile;
 	long lSize;
  	char * buffer;
  	size_t result;

  	pFile = fopen (relativePath.c_str(), "rb" );
 	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  	// obtain file size:
  	fseek (pFile , 0 , SEEK_END);
  	lSize = ftell (pFile);
  	rewind (pFile);
	cout<<"the size of file is   " << lSize<<endl;
	
  	// allocate memory to contain the whole file:
	 buffer = (char*) malloc (sizeof(char)*lSize);
  	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  	// copy the file into the buffer:
  	result = fread (buffer,1,lSize,pFile);
  	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */

  // terminate
 	 fclose (pFile);
 	// free (buffer);
	 cout<<"it is ok here A"<<endl;
/*	
	while(getline(cin,bufferLine)){
		if(flag !=0){
		 	newFile<<"\n";
		}
		flag = 1;
		newFile<<bufferLine;
	}
	newFile.close();
*/

	
	unsigned char *plaintext2 = (unsigned char*)malloc(sizeof(char)*result+1);
	memset(plaintext2,0,sizeof(char)*result+1);
	
	cout<<"it is ok here B"<<endl;
	int decslength = decrypt((unsigned char *)buffer,result, aes_Key,
 	 iv_dec, plaintext2);
	



	printf("our text file is %s \n", plaintext2);
	cout <<"successful"<<endl;
	
	return 0;

}


void store_encryptedkey_iv_iv(unsigned char *MD5digest, unsigned char *iv_dec, unsigned char *aes_Key, string relativePath)
{
	string Key_path(relativePath);
	Key_path.append("+key");
	unsigned char *iv = (unsigned char *)malloc(AES_BLOCK_SIZE+1);
	unsigned char *iv_2 = (unsigned char *)malloc(AES_BLOCK_SIZE+1);
	memcpy(iv_2, iv, AES_BLOCK_SIZE);
	memset(iv,0,AES_BLOCK_SIZE+1);	
	int encrypted_key_length = ((16 + AES_BLOCK_SIZE) /AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
	unsigned char *encypted_key = (unsigned char *)malloc(encrypted_key_length);
        int encrypt_length = encrypt_random_key(iv, MD5digest, aes_Key,encypted_key);

	cout<<"encrypt_length is " << encrypt_length <<endl;
	hex_print(encypted_key,encrypt_length);   

	FILE *keyList;
	keyList = fopen(Key_path.c_str(),"w+");
	if(keyList == NULL)
		exit(EXIT_FAILURE);
	fputs((const char *)encypted_key,keyList);
	hex_print(encypted_key, encrypt_length);
	fputs("\n",keyList);
	fputs((const char *)iv_dec, keyList);
	hex_print(iv_dec, 16);
	fputs("\n", keyList);
	fputs((const char *)iv_2, keyList);
	hex_print(iv_2, 16);
	fclose(keyList);
	free(iv);free(iv_2);free(encypted_key);
}



unsigned char * MD5Sequence(string input) {
 
	//string input;	
        //cout<<"please enter the password"<<endl;
        //getline(cin, input);
        unsigned char *data = (unsigned char *)input.c_str();
        unsigned char *digest = (unsigned char *)malloc(17);
	memset(digest,0,17);
        int i;
        char tmp[3]={'\0'};
	char *buf= (char *)malloc(sizeof(char)*33);
	buf[32] = '\0';
        MD5(data,strlen((const char *)data),digest);
        for (i = 0; i < 16; i++){
                snprintf(tmp,3,"%2.2x",digest[i]);
                strcat(buf,tmp);
        }
	//cout<<sequenceReturned<<endl;
        
	printf("--------------------------->hashed value is %s\n",buf);
	printf("<-------------------------------------------ending\n");        
	return digest;
}
