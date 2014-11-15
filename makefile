CC=g++
CFLAGS=-g -Wall

build: objget   objput  objlist objsetacl objgetacl objtestacl

objget :  objget.o  functionCall.o
	$(CC) $(CFLAGS) -o objget    objget.cpp functionCall.cpp -lssl -lcrypto

	

objput	: help.o functionCall.o
	$(CC) $(CFLAGS) -o objput     help.cpp functionCall.cpp -lssl -lcrypto

	

objlist : objlist.o functionCall.o
	$(CC) $(CFLAGS)  -o objlist    objlist.cpp functionCall.cpp -lssl -lcrypto

	
objsetacl : objsetacl.o functionCall.o
	$(CC) $(CFLAGS)  -o objsetacl    objsetacl.cpp functionCall.cpp -lssl -lcrypto



objgetacl : objgetacl.o functionCall.o
	$(CC) $(CFLAGS)  -o objgetacl   objgetacl.cpp functionCall.cpp -lssl -lcrypto



objtestacl : objtestacl.o functionCall.o
	$(CC) $(CFLAGS)  -o objtestacl  objtestacl.cpp functionCall.cpp -lssl -lcrypto



	
userfile=userfile.txt

exec:	build
	 cat $(userfile) > user+group
	-bash  my_script
	-useradd god
	
	-chown god ./objput
	-chmod 711 ./objput
	-chmod u+s  ./objput
	-chown god ./objget
	-chmod 711 ./objget
	-chmod u+s  ./objget
	-chown god ./objlist
	-chmod 711 ./objlist
	-chmod u+s  ./objlist
	-chown god ./objgetacl
	-chmod 711 ./objgetacl
	-chmod u+s  ./objgetacl
	-chown god ./objsetacl
	-chmod 711 ./objsetacl
	-chmod u+s  ./objsetacl
	-chown god ./objtestacl
	-chmod 711 ./objtestacl
	-chmod u+s  ./objtestacl	
	-mkdir filesystem
	-chown god filesystem
	-chmod 700 filesystem	



test: 	build exec
	-su u1 -c "./objput  doc < haha.txt"
	-su u1 -c "./objget  doc"
	-su u1 -c "./objlist"
	-su u1 -c "./objgetacl doc"
	-su u1 -c "./objsetacl  doc < newacl"
	-su u1 -c "./objtestacl  -a r doc"
	-su u1 -c "./objtestacl  -a x doc"
	-su u1 -c "./objput  doc < haha.txt"
	-su u2 -c "./objput  doc < haha2.txt"
	-su u1 -c "./objget  u2+doc"
	-su u1 -c "./objgetacl u2+doc"
	-su u1 -c "./objsetacl u2+doc < newacl"
	-su u2 -c "./objsetacl u2+doc < setu2ACL" 
	-su u1 -c "./objgetacl u2+doc"
	-su u1 -c "./objget u2+doc"
	-su u1 -c "./objsetacl u2+doc < setu2ACL"
	-su u1 -c "cat filesystem/u1+doc"
	-su u1 -c  "cat filesystem/u1+doc+ACL"
	-su u1 -c "cd filesystem"
	-su u1 -c "ls -l filesystem/u1+doc"
clean:
	-rm  -f  *.o   objtestacl objgetacl objsetacl objlist objput objget
	-rm -f *.core
	-rm -rf filesystem/
	
