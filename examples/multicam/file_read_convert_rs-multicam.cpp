// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp>     // Include RealSense Cross Platform API
#include "example.hpp"              // Include short list of convenience functions for rendering
#include <fstream>              // File IO
#include <iostream>             // Terminal IO
#include <sstream>              // Stringstreams
#include <map>
#include <vector>
#include <cstdio>
#include <chrono>
// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void metadata_to_csv(const rs2::frame& frm, const std::string& filename)
{
    std::ofstream csv;

    csv.open(filename);

    //    std::cout << "Writing metadata to " << filename << endl;
    csv << "Stream," << rs2_stream_to_string(frm.get_profile().stream_type()) << "\nMetadata Attribute,Value\n";

    // Record all the available metadata attributes
    for (size_t i = 0; i < RS2_FRAME_METADATA_COUNT; i++)
    {
        if (frm.supports_frame_metadata((rs2_frame_metadata_value)i))
        {
            csv << rs2_frame_metadata_to_string((rs2_frame_metadata_value)i) << ","
                << frm.get_frame_metadata((rs2_frame_metadata_value)i) << "\n";
        }
    }

    csv.close();
}



int main(int argc, char * argv[]) try
{
    FILE* pFile;

    int num_desired_images = 5,ctr=0;
    std::stringstream readFile;
    readFile << "/home/vigir/Documents/CowData/100/camera1/rs-0-Color_raw_2020_10_19_01_52_35.txt";
    std::stringstream saveFile;
    saveFile << "/home/vigir/Documents/CowData/100/images/";
    std::cout<< "Reading file:\n\t" << readFile.str() << std::endl;
    std::stringstream fileName;
    
    // Read File
    pFile = fopen(readFile.str().c_str(),"rb");
    if(pFile == NULL){
	std::cout << "Could not open file" << readFile.str() << std::endl;
	fclose(pFile); 
	return EXIT_SUCCESS;
	}
    int width,height,bytesPerPixel,strideInBytes;
    size_t result;
    size_t size_int = sizeof(int);

    // read the width
    result = fread(&width, 1,sizeof(int),pFile);
    std::cout <<"\nWidth:"<< width << std::endl;
   // if(result != 1) { fputs ("\nReading error: Width",stderr); exit(3);}
 
    //  read the height
    result = fread(&height, 1,sizeof(int),pFile);
    std::cout <<"\nHeight:"<< height << std::endl;
   // if(result != 1) { fputs ("\nReading error: Height",stderr); exit(3);}
    
    // read the bytes per pixel
    result = fread(&bytesPerPixel, 1,sizeof(int),pFile);
    std::cout <<"\nBytes Per Pixel:"<< bytesPerPixel << std::endl;
   // if(result != 1) { fputs ("\nReading error: Bytes Per Pixel",stderr); exit(3);}
	
    // read the stride in bytes
    result = fread(&strideInBytes, 1,sizeof(int),pFile);
    std::cout <<"\nStride in Bytes:"<< strideInBytes << std::endl;
    //if(result != 1) { fputs ("\nReading error: Stride in Bytes",stderr); exit(3);}

    size_t dataSize = width * height * bytesPerPixel * strideInBytes;
    void* data = (void*) malloc(dataSize);
    
    // read the stride in bytes
    result = fread(data, 1,dataSize,pFile);
    //if(result != 1) { fputs ("\nReading error: Data",stderr); exit(3);}
    std::cout << "\nFile Read is Successful!" << std::endl;


    fileName << saveFile.str() << "/rs-Image.png";
    stbi_write_png(fileName.str().c_str(), width,height,bytesPerPixel,data, strideInBytes);

    std::cout<< "Writing file:\n\t" << fileName.str() << std::endl;
    fclose(pFile);    
    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception & e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}


