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
   
    std::stringstream fileExtension(argv[1]); // Sent from Master Publisher->Subscriber->calls rs-multicam with timestamp for file extension
    int num_desired_images = 5,ctr=0;
    std::stringstream savepath("/media/DataDrive/");
    std::cout<<savepath.str();
    std::string cam_id;
    // Create a simple OpenGL window for rendering:
 //   window app(1280, 960, "CPP Multi-Camera Example");


    rs2::context                          ctx;        // Create librealsense context for managing devices
	
    std::map<std::string, rs2::colorizer> colorizers; // Declare map from device serial number to colorizer (utility class to convert depth data RGB colorspace)

    std::vector<rs2::pipeline>            pipelines;
	
    // Capture serial numbers before opening streaming
    std::vector<std::string>              serials;
   

    for (auto&& dev : ctx.query_devices()){
	
        serials.push_back(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
	}
     int dev_count =0;
    // Start a streaming pipe per each connected device
    for (auto&& serial : serials)
    {
	
        rs2::pipeline pipe(ctx);
        rs2::config cfg;
        cfg.enable_device(serial);
	//cfg.enable_stream(RS2_STREAM_COLOR,640,480,RS2_FORMAT_ANY,30); 
        pipe.start(cfg);
        pipelines.emplace_back(pipe);
        // Map from each device's serial number to a different colorizer
        colorizers[serial] = rs2::colorizer();
	//std::cout << "Dev ["<<dev_count << "] | " << serial << std::endl; 
	dev_count++;
    }
	//std::cout << "Size of pipelines:" << pipelines.size() << std::endl;

    // We'll keep track of the last frame of each stream available to make the presentation persistent
    std::map<int, rs2::frame> render_frames;
    // Main app loop
    //while (app)
    //{
        // Collect the new frames from all the connected devices
        std::vector<rs2::frame> new_frames;
	
        for (auto &&pipe : pipelines)
        {
	    int n=0;
	    std::cout<< "Current Camera:"<<n<<std::endl;
            rs2::frameset fs;
            if (pipe.poll_for_frames(&fs))
            {
		int frame_ctr = 1;
                for (const rs2::frame& f : fs){
			std::cout << "Current frame is:" << frame_ctr << std::endl;
			frame_ctr++;
			new_frames.emplace_back(f); 
			  // save raw RGB, save raw depth
	 // We can only save video frames as pngs, so we skip the rest
		if (auto vf = f.as<rs2::video_frame>())
		{
		    auto stream = f.get_profile().stream_type();
		    // Use the colorizer to get an rgb image for the depth stream
		    auto serial = rs2::sensor_from_frame(f)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		    if (vf.is<rs2::depth_frame>()) vf = colorizers[serial].process(f);
		    
		    // Write images to disk
		    std::stringstream file_name;
		   // std::cout << vf.get_height() << 
		   if(serial == serials[0]){
			cam_id = "camera1";
		    }		   
		  else if(serial == serials[1]){
			cam_id = "camera2";
		    }
		
		   
		   file_name << savepath.str() << cam_id << "/rs-" << ctr << "-"  << 	vf.get_profile().stream_name() << "_raw_" << fileExtension.str() << ".txt";
		   // stbi_write_png(png_file.str().c_str(), vf.get_width(), vf.get_height(),
		                  // vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());*/
		    

		    FILE* fptr = fopen(file_name.str().c_str(),"wb");
		    if(fptr == NULL){
		    	std::cout << "Could not open file" << file_name.str() << std::endl;
			fclose(fptr); 
			return EXIT_SUCCESS;

		    }
		    int w,h,bpp,sib;
		    w = vf.get_width();
		    h = vf.get_height();
		    bpp = vf.get_bytes_per_pixel();
		    sib = vf.get_stride_in_bytes();
		    fwrite(&w,sizeof(w),1,fptr);
		    fwrite(&h,sizeof(h),1,fptr);
		    fwrite(&bpp,sizeof(bpp),1,fptr);
		    fwrite(&sib,sizeof(sib),1,fptr);
		    size_t size = w*h*bpp*sib;   
		    fwrite(vf.get_data(),size,1,fptr);
		    fclose(fptr);
			
		    std::cout << "w: " << vf.get_width() << "| h: " << vf.get_height() << std::endl;
		    std::cout << "\tSaved " << file_name.str() << std::endl;
	
		
		    // Record per-frame metadata for UVC streams
		    //std::stringstream csv_file;
		    /*csv_file << savepath.str() << cam_id[n] <<  "/rs-" << ctr << "-"  << vf.get_profile().stream_name()
		             << "-metadata.csv";*/
		    //metadata_to_csv(vf, csv_file.str());
		   
		}
		
	      }
		 n++;
            }
		//n++;

        }

        // Convert the newly-arrived frames to render-friendly format
       // for (const auto& frame : new_frames)
       // {
	   // int n=0;
            // Get the serial number of the current frame's device
           // auto serial = rs2::sensor_from_frame(frame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            // Apply the colorizer of the matching device and store the colorized frame
            //render_frames[frame.get_profile().unique_id()] = colorizers[serial].process(frame);
		// save rendered RGB, depth
	    /*if (auto vf = frame.as<rs2::video_frame>())
		{
		    auto stream = frame.get_profile().stream_type();
		    // Use the colorizer to get an rgb image for the depth stream
		    auto serial = rs2::sensor_from_frame(frame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		    if (vf.is<rs2::depth_frame>()) vf = colorizers[serial].process(frame);
		    
		    // Write images to disk
		    std::stringstream png_file;
		    png_file << savepath.str() << cam_id[n] << "/rs-rendered-" << ctr << "-" << vf.get_profile().stream_name() << ".png";
		    stbi_write_png(png_file.str().c_str(), vf.get_width(), vf.get_height(),
		                   vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());
		    std::cout << "\tSaved " << png_file.str() << std::endl;

		    // Record per-frame metadata for UVC streams
		    std::stringstream csv_file;
		    csv_file << savepath.str() << cam_id[n] << "/rs-rendered-" << ctr << "-" << vf.get_profile().stream_name()
		             << "-metadata.csv";
		    metadata_to_csv(vf, csv_file.str());
		}
		n++;*/
	

        //}
	ctr++;
        // Present all the collected frames with openGl mosaic
       // app.show(render_frames);
	if(ctr >= num_desired_images)
		return EXIT_SUCCESS; 
   // }
    
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


