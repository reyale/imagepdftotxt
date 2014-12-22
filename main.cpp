#include <iostream>
#include <fstream>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
   po::options_description desc("Allowed options");
   desc.add_options()
    ("help", "produce help message")
    ("input", po::value<std::string>(), "required - input image file")
    ("language", po::value<std::string>(), "default is NULL")
    ("output", po::value<std::string>(), "default is std out")
   ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);   

   if(vm.count("input") <= 0) {
     std::cout << "Please provide input file" << std::endl;
     std::cout << desc << std::endl;
     return -1;
   }

   tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

   int init_result = false;
   if(vm.count("language") > 0) {
     init_result = api->Init(NULL, vm["language"].as<std::string>().c_str()); 
   } else {
     init_result = api->Init(NULL, NULL);
   }
   if(init_result != 0) {
     std::cout << "Failed to initialize tesseract" << std::endl;
     return -1;
   }

   Pix* image = pixRead(vm["input"].as<std::string>().c_str());
   if(!image) {
     std::cout << "Failed to load image" << std::endl;
     return -1;
   }
   api->SetImage(image);

   std::ostream* output;
   if(!vm.count("output"))
     output = &std::cout;
   else
     output = new std::ofstream(vm["output"].as<std::string>().c_str());
    
   char* out_text = api->GetUTF8Text();
   *output << out_text;
   output->flush();

   api->End();
   delete [] out_text;
   pixDestroy(&image);

   return 0;
}
