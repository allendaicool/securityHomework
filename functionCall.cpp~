/*****************************************************
 * FILE NAME: functionCall.cpp
 *
 * Created on: Sept 22 2014
 * Author: Yihong Dai
 * this function keeps all the fucntion call the other source fils required
 *****************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "functionCall.h"
#include <pwd.h>
#include <grp.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

 void hex_print(const void* pv, size_t len)
{
    const unsigned char * p = (const unsigned char*)pv;
    if (NULL == pv)
        printf("NULL");
    else
    {
        size_t i = 0;
        for (; i<len;++i){

           	 printf("hexdecimal is%02X ", *p++);

	}	

	}
    printf("\n");
} 

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}

int lengthCheck(const char *input)
{
	int len;
	len = strlen(input);
	if (len > FILENAME_MAX)
  		return 1;
	return 0;
}

int checkIfContainPlus(char *input)
{
	string 	checkIt(input);
	size_t  pos;	
	pos = checkIt.find('+');
	if (pos != std::string::npos) {
		if (pos == (checkIt.length()-1)) {
			fprintf(stderr, "plus sign should not be at the end\n"); 
			exit(1);
		}
		string str2 = checkIt.substr(pos+1);
		if (str2.find('+') != std::string::npos) {
			fprintf(stderr, "should not exist two plus sign\n");
			exit(1);
		}
	}
	else 
		return 0;
	return 1;
}

/* This function checks if a certain user and group combinatin
 * exists in our meta file 
 *  user: to be checked if a user exist
 *  group: to be checked if a group exists
 *  listFlag: indicates if function is called by objlist
 *  return: 1   if it exist
 *          -1  if it does not exists
 */

int checkifUserGroup (char *user, char *group, int listFlag)
{
	size_t dum;
	char *bufferReadin;
	char *usrGroup = (char *)"user+group";
	FILE *newFile ;
	int val;
	size_t found;
	size_t found2;
	size_t space;
	bufferReadin = NULL;
	newFile = fopen(usrGroup,"r");	

	if(newFile == NULL){
		fprintf(stderr,"failure open file");
		exit(EXIT_FAILURE);

	}
	while((val = (int)getline(&bufferReadin,&dum,newFile))!=-1){
		string findpatter(bufferReadin);
		found = findpatter.find((const char *)user);
		space = findpatter.find_first_not_of(" \t");		
		if(listFlag == 1){
			fclose(newFile);
			if((int)found != -1 && found == space){			
				if(bufferReadin)
					free(bufferReadin);
				return 1;
			}
			else{
				fprintf(stderr, "username is not in the first A %d %d \n", (int)found, (int)space);
				exit(2);
			}
		}
			
		if((int)found == -1)
			continue;
		found2  = findpatter.find((const char *)group);
		if((int)found2 == -1)
			continue;
		if(bufferReadin)
			free(bufferReadin);
		if(found > found2 || found!= space){
  			fprintf(stderr, "username is not in the first B %d %d %d \n", (int)found, (int)found2,(int)space);
			exit(2);
		}
		fclose(newFile);
		return 1;
	}
	
	if(bufferReadin)
		free(bufferReadin);
	fclose(newFile);
	
	return 0;
}



/* This function checks if the shell redirect command exist
 * Parameters:void
 * return: 0;
 */
int checkShellRedirect()
{
    
	if (isatty(fileno(stdin))){
		return 1;
	}	
	/*else
    		puts("stdin is NOT connected to a terminal");*/
  	return 0;
}

/* This function adds string constant to the username
 * string constant appended can be ACL, meta which are used to 
 * create either fileACL or filemeta filename
 *
 * Parameters:
 *  temp: user name
 *  fileFlag: 1 or 0 indicate whether to append fileName
 *  ACL: 1 or 0 indicate whether to append ACL
 *  metaFlag: 1 or 0 indicate whether to append metaFlag
 *  return : 1
 *
 */
int addPathName(string &temp, char * fileName, int FileFlag, int ACL,int metaFlag)
{
	if(FileFlag){
		temp.append("+");
		temp.append(fileName);
	}
	if(ACL){
		temp.append("+");
		temp.append("ACL");
	}
	if(metaFlag){
		temp.append("+");
		temp.append("meta");
	}
	return 1;
	
}

/* This function  checks if certain permission exist among the ACL
 * Parameters:
 *  c: permission to be check
 *  val: The ACL of a certain file
 *  return : 1 if such permission exist
 *	     -1 if such permission does not exist
 */
int checkPermission(char c, char *val)
{
	
	int loop;
	int Permission;
	int strlength;
	strlength = (int)strlen(val);
	Permission = -1 ;
	/* loop is used to check char c appear among the sting val
	 * if exists, then method immmediately gets returned
	 */
	for(loop = 0; loop < strlength;loop++)
	{
		if(val[loop] == c)
		{
			Permission = 1;
			break;
		}
	}
	if(Permission == 1)
	{
		return 1;
	}
	else
	{
		return -1;
	}
	return 1;
}

/* This function finds the ACL for a certain file
 *  filename: ACL file
 *  first: the username
 *  second: the group
 *  val: val written to be the permission list and returned back
 *  return : 0;
 */
int findPermission(string &filename, char *first, vector<string>& second, char **val)
{
	/* open the file and find the corresponding permission
	 * list for certain users and group
	 */
	 
	ifstream infile;
	infile.open(filename.c_str());
	
	if(infile.fail()){
		fprintf(stderr, "file does not exist\n");
		exit(EXIT_FAILURE);
	}
	
	
    	char *token;
	char *token2;
	char *insideToken1;
	char *insideToken2;
	char *array;
	char *delimiterInside;
	char * delimiter = (char *)" \t";
	string temp;
	int check;
	
	/* read the line by line to find the corresponding
	 * permission list for certain usr and group combinatin
	 */
	while(getline(infile,temp))
	{
		/* we assume that the format for each line is as follows
		 * u1.*  rwxpv
		 * we find the first topken and second token
		 * by using delimiter space or tab
		 */
		array = (char *)temp.c_str();
		token = strtok(array,delimiter);
		if(token == NULL){
			continue;
		}
		token2 = strtok(NULL,delimiter);	
		/* we pass the first token u1.* again to
		 * retrieve the user and group
		 */
		delimiterInside = (char *)".";
		insideToken1 =strtok(token,delimiterInside);
		insideToken2 = strtok(NULL,delimiterInside);
		/*  call the checkMatch to check if there is permission
		 *  list exist for usr and group combination
		 */
		check = checkMatch(first,second,insideToken1,insideToken2);
		if(check)
		{
			infile.close();
			*val = (char *)malloc(sizeof(char)*(strlen(token2)+1));
			strcpy(*val,token2);
			return 1;
		}
	}
	infile.close();
	return 0;
}


/* This function check if there is acl permission match
 * for certain usr and group
 *  first: the username
 *  second: the group
 *  std1: users in the ACL to be compared against
 *  std2: groups in the ACL to be compared against
 *  return : 1 if such combination is found;
 *	     0 if no such combination is found;
 */
int checkMatch ( char * first , vector<string> &second, char *std1, char *std2)
{
	if(strcmp(std1,first) == 0|| strcmp(std1,"*") == 0){
		if(strcmp(std2,"*")==0)
			return 1;	
		for (int i = 0 ; i < (int)second.size();i++){
			if (strcmp(second.at(i).c_str(),std2) == 0)
				return 1;
		}
	}
		
	return  0;
}

/* This function checks if a character is among the rwx options
 *  a: character to be checked
 *  return : true if it is one of rwx
 *	     false if it is not
 */
bool isdefaultPermission(char a)
{
	if(a != 'r'&& a!='w' && a!='x' && a!='v' && a!='p')
		return false;
	return true;
}



int sanityCheck(char  *str, int plusIndicator)
{
	int i;
	int len = strlen(str);
	for (i = 0; i < len; i++) {
		if((str[i]<'A'&&(str[i] > '9'||str[i] < '0')) ||(str[i]<'a'&&str[i]>'Z'&&str[i] != '_')||str[i]>'z'){
			if (str[i] == '+') {
				if(!plusIndicator)
					return 0;
			}
			else
				return 0;
		}
	}
	return 1;
}



int getUser_Group(string &user, vector<string> &groups)
{
	char *userName;
	struct passwd *passTemp;
	struct group *groupTemp;
	int loop;
	gid_t *gid;
	uid_t uid;
	
	uid = getuid();
	passTemp = getpwuid(uid);	

	userName = passTemp->pw_name;
	int count_gourp = getgroups(0,NULL);
	gid = (gid_t *)malloc(count_gourp * sizeof(gid_t));
	getgroups(count_gourp,gid);
	for (loop = 0; loop < count_gourp; loop++){
		groupTemp = getgrgid(gid[loop]);
		string tempName(groupTemp->gr_name);
		groups.push_back(tempName);
	}

	user.append(userName);
	return 1;
}

/* This function parse the commands passed in by using getopt and
 * set corresponding flag
 *  argc: the number of arguments
 *  command: the argument line
 *  uFlag: user name flag
 *  gFlag: gourp name flag
 *  aFlag: action flag
 *  lFlag: list flag 
 *  usr: usr to be assgined the usrname
 *  group: group to be assigned the groupname
 *  operation: option following the -a
 *  return 1;
 */
int parseCommand(int argc , const char ** command,
		 int &aFlag,
		 int &lFlag, char &operation, int &kFlag, string &password)
{
	/* c keeps the return value of getopt
	 * enter records if the we have entered the getopt loop
	 * if enter equals to one then loop has been exeucted
	 * if enter equals to -1 then loop has not been executed
	 */
	int c;
	
	
	/* set the corresponding flags
	 * if there is no option after -u -g or -a
	 * then we report errors and exit immediately
	 * if the option after -a is more than one character,
	 * then we report an error as well
	 */
	while ( (c = getopt(argc,(char * const *)command,"a:k:l")) != -1){
		
		switch(c)
		{
			case'a':
				aFlag = 1 ;
				if(strlen(optarg)>1){
					fprintf(stderr,
				"permission check only need one character\n");
					exit(EXIT_FAILURE);

				}
				operation = optarg[0];
				if(!isdefaultPermission(operation)){
					fprintf(stderr,
					       "permission invalid\n");
					exit(EXIT_FAILURE);
				}

				break;
			case 'l':
				lFlag = 1;
				break;
				
			case 'k':
				kFlag = 1;
				password.append(optarg);
				break;
				
			case '?':
				if(optopt == 'a' || optopt == 'l'|| optopt == 'k')
					fprintf (stderr,
					"Option -%c requires an argument.\n"
						 , optopt);
				else if (isprint (optopt))
					fprintf (stderr,
					"Unknown option `-%c'.\n", optopt);
				
				else
					fprintf (stderr,
						 "werid character `\\x%x'.\n",
						 optopt);
				return 1;
				
			default:
				abort ();
		}
	}
	return 1;
}




int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}


int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}





void write_Into_file_cipher(string &relativePath, unsigned char *ciphertext, int cipher_length )
{
	int fd = open((const char *)relativePath.c_str(),O_WRONLY);
	if(write(fd,(void *)ciphertext,cipher_length) == -1){
		fprintf(stderr,"error happens\n");
		exit(1);
	}
	close(fd);
	return;
}


unsigned char * write_Into_file_and_out (string &relativePath,int & size_file)
{
	writeIntoFile(relativePath);

	size_file = size_of_file(relativePath);

	printf("the size of file is  %d \n" ,  size_file);

	void *buffer = (void *)malloc(sizeof(char)*size_file +1 );

	memset(buffer,0,sizeof(char)*size_file +1);

	int fd;
	fd = open((const char *)relativePath.c_str(),O_RDONLY);
	if(read(fd,buffer,size_file) == -1){
		fprintf(stderr,"error happens\n");
		exit(1);
	}
	close(fd);
	unsigned char * plaintext = (unsigned char *)buffer;
	printf("our buffer is %s \n", plaintext);
	return plaintext;
}


int size_of_file (string & relativePath)
{
	struct stat st;
	int size;
	stat(relativePath.c_str(), &st);
	size = (int)st.st_size;
	return size;
}
int writeIntoFile(string &relativePath)
{
	string bufferLine;
	ofstream newFile;
  	newFile.open(relativePath.c_str(),std::ofstream::out);	
	/* read file contents from the input file into the usr file
	 */
	cout<<"the relative path is    "<<relativePath<<endl;
	int flag = 0;
	while(getline(cin,bufferLine)){
		if(flag !=0){
		 	newFile<<"\n";
		}
		flag = 1;
		newFile<<bufferLine;
	}
	newFile.close();
	return 0;	
}


int  encrypt_random_key(unsigned char *iv, unsigned char *iv2,unsigned char *hashValue, unsigned char *random_key, unsigned char *encypted_key)
{
	memset(iv,0,AES_BLOCK_SIZE+1);
	rand_gen(iv);
	memcpy(iv2, iv, AES_BLOCK_SIZE);
	cout<<"------------------->   strlen of iv2 is "<< strlen((const char *)iv2) << endl;
	cout<<"------------------->   strlen of iv is "<< strlen((const char *)iv) << endl;
	int random_key_length = 16;
	// assume encypted_key has been malloced
	int cipher_text_length =  encrypt(random_key, random_key_length, hashValue,
 iv, encypted_key);
	// error checking needs to be done here
	return cipher_text_length;
}


void rand_gen (unsigned char *bytes)
{
	int i = 0;
	FILE* f;
	/*open file */
	if((f = fopen("/dev/urandom", "r")) == NULL) {
  	   exit(1);
	}	
	while(i < 16)  {
     		if((bytes[i]  = fgetc(f)) == EOF) {
       		   	 fprintf(stderr,
				"reading failure\n");
				exit(1);
     		}
      		i++;
	}
//	printf("the size of byte is %d\n", sizeof(bytes[0]));
	fclose(f);
}
