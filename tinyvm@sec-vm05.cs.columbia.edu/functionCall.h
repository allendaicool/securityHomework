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
using namespace std;
int getUser_Group(string &, vector<string> &);
int parseCommand(int, const char ** ,
		 int &,
		 int &, char &);
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
#endif /* defined(____functionCall__) */
