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
    std::string cam_id;
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
    for (auto&& serial : serials)
    {
	
        rs2::pipeline pipe(ctx);
        rs2::config cfg;
        cfg.enable_device(serial);
	cfg.enable_stream(RS2_STREAM_COLOR,1280,720,RS2_FORMAT_ANY,30);
	cfg.enable_stream(RS2_STREAM_DEPTH,1280,720,RS2_FORMAT_ANY,30); 
        pipe.start(cfg);
        pipelines.emplace_back(pipe);
        // Map from each device's serial number to a different colorizer
        colorizers[serial] = rs2::colorizer();
	std::cout << "rs-camera["<<dev_count << "] :: serial_no: " << serial << std::endl; 
	dev_count++;
    }
	

    // We'll keep track of the last frame of each stream available to make the presentation persistent
    std::map<int, rs2::frame> render_frames;
    // Main app loop
    //while (app)
    //{
        // Collect the new frames from all the connected devices
        std::vector<rs2::frame> new_frames;
	int n=0;
	 
      for (auto &&pipe : pipelines)
        {
	    std::stringstream file_name1;
	    std::stringstream file_name2;
	    std::stringstream file_name3;
	    //std::cout<< "Current Camera:"<<n<<std::endl;
	    n++;
            rs2::frameset fs;
	    
	    //if( pipe.poll_for_frames(&fs) ){ // if this runs then get frames!
	     fs = pipe.wait_for_frames();
	     
			   rs2::depth_frame depth = fs.get_depth_frame();
			  auto serial = rs2::sensor_from_frame(depth)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
//               auto serial = 1;
			   rs2::video_frame depth_colored = colorizers[serial].process(depth);
               		   //std::cout << "Serial Number:" << serial << std::endl;
			   //std::cout << "Frameset_size:" << fs.size() << std::endl; 
			  
		 	   file_name1 << savepath.str() << "rs-depth_" << serial << "_" << fileExtension.str() << ".txt";
			    FILE* fptr = fopen(file_name1.str().c_str(),"wb");
			    if(fptr == NULL){
				 std::cout << "Could not open file" << file_name1.str() << std::endl;
			     fclose(fptr);
			     return EXIT_SUCCESS;
			    }
			     int w,h,bpp,sib;
			     w = depth.get_width();
			     h = depth.get_height();
			     std::cout << "Saving Depth image with :[w="<< w << ",h=" << h << "] | Device:" << serial << std::endl; 
			     bpp = depth.get_bytes_per_pixel();
			     sib = depth.get_stride_in_bytes();
			     fwrite(&w,sizeof(w),1,fptr);
			     fwrite(&h,sizeof(h),1,fptr);
			     fwrite(&bpp,sizeof(bpp),1,fptr);
			     fwrite(&sib,sizeof(sib),1,fptr);
			     size_t size = w*h*bpp*sib;
			     fwrite(depth.get_data(),size,1,fptr);
			     fclose(fptr);
			   file_name3 << savepath.str() << "rs-depth_" << serial << "_" << fileExtension.str() << ".png";
			   //write_raw_files(file_name1, depth.get_width(), depth.get_height(), depth.get_bytes_per_pixel(),depth.get_data(), depth.get_stride_in_bytes() );
			  stbi_write_png(file_name3.str().c_str(), depth_colored.get_width(), depth_colored.get_height(), depth_colored.get_bytes_per_pixel(),depth_colored.get_data(), depth_colored.get_stride_in_bytes());
			    
			   rs2::video_frame color = fs.get_color_frame();
			   file_name2 << savepath.str() << "rs-color_" << serial << "_" << fileExtension.str() << ".txt";
			  // write_raw_files(file_name2, color.get_width(), color.get_height(), color.get_bytes_per_pixel(),color.get_data(), color.get_stride_in_bytes() );
			  // stbi_write_png(file_name2.str().c_str(), color.get_width(), color.get_height(), color.get_bytes_per_pixel(),color.get_data(), color.get_stride_in_bytes());
			    fptr = fopen(file_name2.str().c_str(),"wb");
			    if(fptr == NULL){
				 std::cout << "Could not open file" << file_name2.str() << std::endl;
			     fclose(fptr);
			     return EXIT_SUCCESS;
			    }
			     
			     w = color.get_width();
			     h = color.get_height();
			     std::cout << "Saving Color image with :[w="<< w << ",h=" << h << "] | Device:" << serial << std::endl; 
			     bpp = color.get_bytes_per_pixel();
			     sib = color.get_stride_in_bytes();
			     fwrite(&w,sizeof(w),1,fptr);
			     fwrite(&h,sizeof(h),1,fptr);
			     fwrite(&bpp,sizeof(bpp),1,fptr);
			     fwrite(&sib,sizeof(sib),1,fptr);
			     size = w*h*bpp*sib;
			     fwrite(color.get_data(),size,1,fptr);
			     fclose(fptr);
		   //}end-if poll for frames
		/*if( pipe.poll_for_frames(&fs) ){ // if this runs then get frames!
				if(auto&& frame = fs[iterator]);
					iterator++;
				else return EXIT_SUCCESS;

				if(auto vf = frame.as<rs2::video_frame())
				{
					auto serial = rs2::sensor_from_frame(frame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
					std::cout << "Serial Number:" << serial << std::endl;
					if(vf.is<rs2::depth_frame>()) vf = colorizers[serial].process(frame);
				}			
		
		
	
			for( const rs2::frame& frame : fs){	
		   		if( frame.is<rs2::depth_frame>() ){
					//do depth stuff!
				   auto serial = rs2::sensor_from_frame(frame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
				   std::cout << "Serial Number:" << serial << std::endl;
				   frame = colorizers[serial].process(frame);
			 	   file_name1 << savepath.str() << "rs-depth_" << serial << "_" << fileExtension.str() << ".png";
				   stbi_write_png(file_name1.str().c_str(), frame.get_width(), frame.get_height(), frame.get_bytes_per_pixel(),frame.get_data(), frame.get_stride_in_bytes());
			    
				}
				else {
				   auto serial = rs2::sensor_from_frame(frame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
				   std::cout << "Serial Number:" << serial << std::endl;
				   file_name1 << savepath.str() << "rs-color_" << serial << "_" << fileExtension.str() << ".png";
				   stbi_write_png(file_name1.str().c_str(), frame.get_width(), frame.get_height(), frame.get_bytes_per_pixel(),frame.get_data(), frame.get_stride_in_bytes());
			    
				}		
			   } // end for		   
		    
		} // end if*/
		//else
		//{ std::cout << "No frames to process!" << std::endl;}
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


