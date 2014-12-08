#include <iostream>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

int main(int argc, char** argv)
{
    if (argc < 2) {
	std::cout << "please provide an input file" << std::endl;
        return -1;
    }

    char *outText;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) {
        std::cout << "could not initialize tesseract" << std::endl; 
        return -1; 
    }

    Pix *image = pixRead(argv[1]);
    api->SetImage(image);
    
    outText = api->GetUTF8Text();
    printf("%s", outText);

    api->End();
    delete [] outText;
    pixDestroy(&image);

    return 0;
}
