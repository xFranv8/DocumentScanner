//
//  main.cpp
//  brewCV
//
//  Created by xFranv8.


#include <iostream>
#include "DocumentScanner.hpp"



int main(int argc, const char * argv[]) {

    DocumentScanner *scanner = new DocumentScanner();

    //scanner->load_image("/Users/xfranv8/Desktop/image.png");
    //scanner->load_image("/Users/xfranv8/Desktop/IMG_2882small.png");
    scanner->load_image("/Users/xfranv8/Desktop/image3.png");
    scanner->scan_image();
    scanner->display_image();

    delete scanner;

    return 0;
}


