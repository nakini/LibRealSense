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
#include <string>
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


void write_raw_files(std::stringstream fileName, int w,int h, int bpp, const void* data, int sib){
 	    FILE* fptr = fopen(fileName.str().c_str(),"wb");
            if(fptr == NULL){
                 std::cout << "Could not open file" << fileName.str() << std::endl;
             fclose(fptr);
             exit(1);
            }
             
             fwrite(&w,sizeof(w),1,fptr);
             fwrite(&h,sizeof(h),1,fptr);
             fwrite(&bpp,sizeof(bpp),1,fptr);
             fwrite(&sib,sizeof(sib),1,fptr);
             size_t size = w*h*bpp*sib;
             fwrite(data,size,1,fptr);
             fclose(fptr);
}

int main(int argc, char * argv[]) try
{
   if(argc < 2){
       std::cout << "Usage : ./rs-multicam [time_stamp, ex : 2020_10_24_00_01_05]" << std::endl;
       return EXIT_SUCCESS;
   }
    std::stringstream fileExtension(argv[1]); // Sent from Master Publisher->Subscriber->calls rs-multicam with timestamp for file extension
    int num_desired_images = 1,ctr=0;
    std::stringstream savepath("/home/vigir/images/");
    std::cout<<"\nSaving Images at : " << savepath.str() << std::endl;
    std::stringstream fname;
    // Create a simple OpenGL window for rendering:
 //   window app(1280, 960, "CPP Multi-Camera Example");


    rs2::context                          ctx;        // Create librealsense context for managing devices
	
    std::map<std::string, rs2::colorizer> colorizers; // Declare map from device serial number to colorizer (utility class to convert depth data RGB colorspace)

    std::vector<rs2::pipeline>            pipelines;
	
    // Capture serial numbers before opening streaming
    std::vector<std::string>              serials;
   

    int itr = 0;
    for (auto&& dev : ctx.query_devices()){
	
        serials.push_back(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
	std::cout << "device[" << itr <<"] : " << serials[itr] << std::endl;
	itr++;
	}
	std::cout << "Number of devices (rs-cameras):" << serials.size() << std::endl;
     int dev_count =0;
    // Start a streaming pipe per each connected device
	int n = 30;
    for (auto&& serial : serials)
    {
	
        rs2::pipeline pipe(ctx);
        rs2::config cfg;
        cfg.enable_device(serial);
	cfg.enable_stream(RS2_STREAM_COLOR,1280,720,RS2_FORMAT_ANY,30);
	cfg.enable_stream(RS2_STREAM_DEPTH,1280,720,RS2_FORMAT_ANY,30);
	cfg.enable_record_to_file( savepath.str() + "data_" + std::to_string(n) +"_" + serial + "_" + fileExtension.str() + ".bag"); 
        pipe.start(cfg);
	
	for(int i=0; i<n ;i++){
		auto  frame = pipe.wait_for_frames();	
	}
        pipe.stop();
    }
	

    
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


