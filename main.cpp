#include <iostream>
#include <fstream>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void set_tess_variables(tesseract::TessBaseAPI* api, const std::vector<std::string> & variables) {
  for(size_t i = 0; i < variables.size(); ++i) {
    std::string kv = variables[i];
    size_t pos = kv.find(":");
    if(pos == std::string::npos)
      throw std::runtime_error("Invalid variable, please delineate by a semi-colon: " + kv);
    std::string key = kv.substr(0, pos);
    std::string value = kv.substr(pos + 1, kv.size() - pos);
    if(!api->SetVariable(key.c_str(), value.c_str()))
      throw std::runtime_error("Failed to set: " + key + " " + value);
  }
}

int main(int argc, char** argv)
{
   po::options_description desc("Allowed options");
   desc.add_options()
    ("help", "produce help message")
    ("input", po::value<std::string>(), "required - input image file")
    ("language", po::value<std::string>(), "default is NULL")
    ("output", po::value<std::string>(), "default is std out")
    ("tessopt", po::value<std::vector<std::string> >()->multitoken(), "tesseract options <key>:<value>")
    ("tesssegmode", po::value<int>()->default_value(tesseract::PSM_SINGLE_BLOCK), "tesseract page segmentation mode");
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
 
   if(vm.count("tessopt"))
     set_tess_variables(api, vm["tessopt"].as<std::vector< std::string> >());

   if(vm.count("tesssegmode")) {
     tesseract::PageSegMode mode = (tesseract::PageSegMode)(vm["tesssegmode"].as<int>());
     if(mode < 0 || mode > tesseract::PSM_COUNT)
       throw std::runtime_error("Invalid tess segmentation mode");
     api->SetPageSegMode(mode);
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
