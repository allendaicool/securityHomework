//
//  functionCall.h
//  
//
//  Created by yihong dai on 9/20/14.
//
//

#ifndef ____functionCall__
#define ____functionCall__
#include<string>
#include <vector>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/md5.h>


using namespace std;
bool compare_key_fakeKey(unsigned char * , unsigned char *, int );
unsigned char * read_enc_key_iv_file (string, int & ) ;
unsigned char * MD5Sequence(string);
int getUser_Group(string &, vector<string> &);
int parseCommand(int, const char ** ,
		 int &,
		 int &, char &,  int &, string &);
int findPermission(string & ,char *,vector<string>&, char **);
int checkPermission(char, char *);
int addPathName(string &, char *, int , int, int);
int checkShellRedirect();
bool isdefaultPermission(char );
int checkMatch ( char *  , vector<string>&, char *, char *);
int checkifUserGroup (char *, char *,int);
int sanityCheck(char  *, int);
int checkIfContainPlus(char *);
int lengthCheck(const char *);


void rand_gen (unsigned char *);
int writeIntoFile(string &);
int size_of_file (string &);
unsigned char * write_Into_file_and_out (string &,int &);
void write_Into_file_cipher(string &, unsigned char *, int);
int  encrypt_random_key(unsigned char *,unsigned char *, unsigned char *, unsigned char *, unsigned char *);
void hex_print(const void* , size_t);
int encrypt(unsigned char *, int, unsigned char *,
  unsigned char *, unsigned char *);

int decrypt(unsigned char *, int , unsigned char *,
  unsigned char *, unsigned char *);


#endif /* defined(____functionCall__) */
