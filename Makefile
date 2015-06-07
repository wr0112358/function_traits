CFLAGS=-Wmissing-prototypes -Wbad-function-cast -Wstrict-prototypes -Wnested-externs -Wold-style-definition
CXXFLAGS=-pedantic -Wall -Wpointer-arith -Wcast-qual \
    -Wno-missing-braces -Wextra -Wno-missing-field-initializers -Wformat=2 \
    -Wswitch-enum -Wcast-align -Wpointer-arith \
    -Wstrict-overflow=5 -Winline \
    -Wundef -Wcast-qual -Wunreachable-code \
    -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls \
    -Werror -Wfloat-equal\
    -ggdb3 \
    -O0 \
    -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing \
    -std=c++1z

test: test.o
	g++ $^ -o $@

clean:
	rm -f test test.o
