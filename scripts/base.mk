#COMPILER 			= /usr/local/bin/g++49 -R/usr/local/lib/gcc49
COMPILER		= clang++
COMPILER_OPTIONS 	= -std=c++0x -Wall -Wextra -pedantic -pipe -O2 -finline-functions -fPIC -g -DDEBUG

YAMI_INCLUDE 		= -I/usr/local/include/yami4
YAMI_LIBS 			= -L/usr/local/lib/yami4  -lyamicored -lyamicppd -pthread

SOCI_INCLUDE		= -I/usr/local/include/soci \
					-I/usr/local/include/postgresql \
					-I/usr/local/include/mysql

SOCI_LIBS			= -L/usr/local/lib/soci -lsoci_mysql -lsoci_postgresql -lsoci_core

