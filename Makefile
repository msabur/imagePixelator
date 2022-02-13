CXXFLAGS=-Wall -Wno-return-type
LDLIBS=-lm
EXE=pixelator

all: $(EXE)

$(EXE): pixelator.cpp libs/ppm_io.cpp libs/argparse.c

clean:
	$(RM) $(EXE)
