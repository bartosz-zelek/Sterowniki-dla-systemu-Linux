#
# Inputs
#
EXTRA_CFLAGS = 
OBJS =    demo-one.o big-chars.o
PROGRAM = demo-one

#
# Allow makefiles to override optimization level
# -- Lower optimization level to make debug nicer
# 
ifeq ($(OPT_LEVEL),)
	OPT_LEVEL=-O0
endif

#
# Flags to compile. Customize with the EXTRA variables
#
LIBS = -lpthread $(EXTRA_LIBS) 
C_FLAGS = -g $(OPT_LEVEL) -Wall -Werror $(EXTRA_C_FLAGS) #-m64

#
# By depending on LIB, we can also build .a files 
# automatically!
#  
all: $(PROGRAM) $(LIB)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(LIBS) $(C_FLAGS)  $(OBJS) 

$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $(OBJS)	

%.o: %.c
	$(CC) -c $? -o $@ $(C_FLAGS) 

%.o: %.cc
	$(CC) -c $? -o $@ $(C_FLAGS) 

clean: 
	rm -f *.elf *.o *.a



