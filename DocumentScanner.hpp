//
//  DocumentScanner.hpp
//  brewCV
//
//  Created by xFranv8
//

#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

class DocumentScanner {

public:
    DocumentScanner();
    ~DocumentScanner() {} ;

    void load_image(std::string path_to_file);
    
    void scan_image(void);
    
    void display_image(void);
    
protected:
    cv::Mat src;

private:
    
    cv::Mat dest;
        
};

