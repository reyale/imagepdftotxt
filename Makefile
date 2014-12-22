GCC=g++
BOOST_LIBS=-lboost_program_options
LIBRARIES=-pthread -llept /usr/lib/libtesseract.a  -lz -L/usr/lib/x86_64-linux-gnu -lwebp -ltiff -ljpeg -lgif -lpng $(BOOST_LIBS)
BINARY=tesseract

default: tesseract

tesseract:
	$(GCC) main.cpp $(LIBRARIES) -o $(BINARY) 

clean:
	$(RM) *.o $(BINARY)
