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

#include <vector>

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

int main(int argc, const char * argv[])
{
	FILE * v2_file;
	v2_file = fopen("./u1+doc1+v1","rb");
	if (v2_file==NULL) {fputs ("File error",stderr); exit (1);}
	unsigned char *v2_enc_buffer = (unsigned char *) malloc (17);
	memset(v2_enc_buffer,0,17);

	int byte_read = fread (v2_enc_buffer,1,17,v2_file);

	if (16 != byte_read) {fputs ("Reading error",stderr); exit (3);}

//	cout << "read in size is----------------------------> "<<	byte_read << endl;
	hex_print(v2_enc_buffer, 16);
	fclose(v2_file);	
	return 0;


}
