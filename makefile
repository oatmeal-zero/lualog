install:
#ifdef USING_ZLOG then
	gcc -g -DUSING_ZLOG -Wall -fPIC --shared -o lualog.so log.c -I$(HOME)/include -I$(HOME)/include/lua -L$(HOME)/lib -llua5.2 -lzlog
#else	
#	gcc -g -Wall -fPIC --shared -o lualog.so log.c -I$(HOME)/include -I$(HOME)/include/lua -L$(HOME)/lib -llua5.2 -llogpub
#endif	
	mv lualog.so $(HOME)/lib/
