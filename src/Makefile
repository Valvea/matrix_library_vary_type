# ---- базовые флаги ----
CC      ?= cc
AR      ?= ar
CSTD    := -std=c11
OPT     := -O3 
ARCH    ?= -march=native
WARN    := -Wall -Wextra -Wshadow -Werror  -Wdouble-promotion
LTO     ?= -flto
CPPFLAGS:= -I.
DEBUG   := -O0 -g -fsanitize=address

CFLAGS  := $(CSTD) $(OPT) $(ARCH) $(WARN) $(LTO) $(CPPFLAGS)
PICFLAGS:= -fPIC                      # для shared
SHFLAGS := -shared                    # линковка .so

# ---- исходники ----
INT_SRC := matrix_int.c  cast_int_double_m.c
DBL_SRC := matrix_double.c  gauss_elim_part_piv.c
CORE_SRC := matrix_core.c


INT_OBJ     := $(INT_SRC:.c=.o)
DBL_OBJ     := $(DBL_SRC:.c=.o)
INT_OBJ_PIC := $(INT_SRC:.c=.pic.o)
DBL_OBJ_PIC := $(DBL_SRC:.c=.pic.o)
CORE_OBJ_PIC := $(CORE_SRC:.c=.pic.o)

# имена библиотек
INT_STATIC := libmatrix_int.a
DBL_STATIC := libmatrix_double.a
CORE_STATIC := libmatrix_core.a

INT_SHARED := libmatrix_int.so
DBL_SHARED := libmatrix_double.so
CORE_SHARED := libmatrix_core.so
.PHONY: all libs static shared demo clean

all: libs
libs: static shared

# ---- статические ----
static: $(INT_STATIC) $(DBL_STATIC) $(CORE_STATIC)

$(CORE_STATIC): $(CORE_OBJ)
	$(AR) rcs $@ $^

$(INT_STATIC): $(INT_OBJ)
	$(AR) rcs $@ $^

$(DBL_STATIC): $(DBL_OBJ)
	$(AR) rcs $@ $^


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---- динамические ----
shared: $(INT_SHARED) $(DBL_SHARED) $(CORE_SHARED)

$(CORE_SHARED): $(CORE_OBJ_PIC)
	$(CC) $(SHFLAGS) -o $@ $^
$(INT_SHARED): $(INT_OBJ_PIC)
	$(CC) $(SHFLAGS) -o $@ $^
$(DBL_SHARED): $(DBL_OBJ_PIC)
	$(CC) $(SHFLAGS) -o $@ $^

%.pic.o: %.c
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

# ---- демо (опционально) ----
demo: shared
	$(CC) $(CFLAGS) demo.c -L. -lmatrix_int -lmatrix_double -lmatrix_core -o demo.out
	# при запуске экспортируй путь к .so:  LD_LIBRARY_PATH=. ./demo.out

clean:
	 rm -f *.o *.pic.o *.a *.so demo.out

cppcheck:
	cppcheck *.c *.h --enable=all --suppress=missingIncludeSystem

format:
	clang-format  -style=file:../.clang-format -i *.c *.h