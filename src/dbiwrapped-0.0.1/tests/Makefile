INCLUDE =	-I/usr/devel/include -I..
CFLAGS =	-Wall -g -O0 $(INCLUDE) -MD
# CFLAGS =	-Wall -g -O2 $(INCLUDE) -MD
# namespace
#CFLAGS +=	-DMYSQLW_NAMESPACE=mysqlw
CPPFLAGS =	$(CFLAGS)

#LIBS =		-L/usr/local/lib/mysql -lmysqlclient -lz
LIBS =		-ldbi -lz -lmysqlclient

PROGS =		sample1	

all:		$(PROGS) 

sample1:	sample1.o
		g++ -o $@ $^ $(LIBS) ../libmysqlwrapped.a
		
clean:
		rm -f *~ *.o $(PROGS) *.d slask *.a


