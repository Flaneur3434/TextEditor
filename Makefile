PROGRAM = marisa
FILES.c = buffer.c editorOperations.c file_IO.c frame.c init.c input.c keymap.c main.c terminal.c utils.c
FILES.h = marisa.h buffer.h frame.h utils.h
FILES.o = ${FILES.c:.c=.o}

CC      = gcc

LD      = ld

SFLAGS  = -std=c11

GFLAGS  = -g

OFLAGS  = -O0

WFLAG1  = -Wall
WFLAG2  = -Wextra
WFLAG3  = -Werror
WFLAG4  = -Wstrict-prototypes
WFLAG5  = -Wmissing-prototypes
WFLAG6  = -Wshadow
WFLAG7  = -Wnull-dereference
WFLAG8  = -Wduplicated-cond
WFLAG9  = -Wunused-macros
WFLAG10 = -Wduplicated-branches
WFLAG11 = -Wvector-operation-performance
WFLAG12 = -Wredundant-decls
WFLAG13 = -Wmissing-field-initializers
WFLAG14 = -Wpointer-arith

LIB1    = -lncursesw

WFLAGS  = ${WFLAG1} ${WFLAG2} ${WFLAG3} ${WFLAG4} ${WFLAG5} ${WFLAG6} ${WFLAG7} ${WFLAG8} ${WFLAG9} ${WFLAG10} ${WFLAG11} ${WFLAG12} ${WFLAG13} ${WFLAG14}

UFLAGS  = # Set on command line only

CFLAGS  = ${SFLAGS} ${GFLAGS} ${OFLAGS} ${WFLAGS} ${UFLAGS} $(shell pkg-config --cflags glib-2.0)

LDFLAGS =

LDLIBS  = ${LIB1}

LFLAGS = $(shell pkg-config --libs glib-2.0)

CTAGS   = ctags -f TAGS -e -R --languages=C

all: ${PROGRAM} tags

${PROGRAM}: ${FILES.o}
	${CC} -o $@ ${CFLAGS} ${FILES.o} ${LFLAGS} ${LDFLAGS} ${LDLIBS}

${FILES.o}: ${FILES.h}

# If it exists, prog1.dSYM is a directory on macOS
DEBRIS = a.out core *~ *.dSYM *.su TAGS
RM_FR  = rm -fr

tags: ${FILES.c}
	@-${RM_FR} TAGS
	@-${CTAGS} ${FILES.c}

clean:
	@-${RM_FR} ${FILES.o} ${PROGRAM} ${DEBRIS}
