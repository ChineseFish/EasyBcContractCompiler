FLEX=flex
FLEXFLAGS=-o
BISON=bison
BISONFLAGS=--verbose --graph -d -o
CC=gcc
CFLAGS=-g -Wall

#
SUB_DIRS=parser optimize ast byte

#记住当前工程的根目录路径
ROOT_DIR=$(shell pwd)

#目标文件所在的目录
OBJS_DIR=$(ROOT_DIR)/obj

#获取c
CUR_SOURCE=${wildcard *.c}

#
CUR_OBJS=${patsubst %.c, $(OBJS_DIR)/%.o, $(notdir $(CUR_SOURCE))}

#
export OBJS_DIR CC CFLAGS

all:$(SUB_DIRS) $(CUR_OBJS) DEBUG 

#递归执行子目录下的makefile文件
$(SUB_DIRS):ECHO
	make -C $@

DEBUG:ECHO
	make -C debug

ECHO:
	@echo $(SUBDIRS)

$(OBJS_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

pre:
	$(BISON) $(BISONFLAGS) parser/rule.c parser/spl.y
	$(FLEX)  $(FLEXFLAGS) parser/lex.yy.c parser/spl.l

clean:
	rm -f debug/splc
	rm -f $(OBJS_DIR)/*.o
	rm -f parser/rule.dot parser/rule.output

cleanall:clean
	rm -f parser/rule.h parser/rule.c parser/lex.yy.c