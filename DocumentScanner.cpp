//
//  DocumentScanner.cpp
//  brewCV
//
//  Created by xFranv8.
//

#include "DocumentScanner.hpp"


DocumentScanner::DocumentScanner() {
    
}

void DocumentScanner::load_image(std::string path_to_file) {

    src = cv::imread(path_to_file);
    
    dest.create(src.size(), src.type());
}


void DocumentScanner::scan_image() {

    cv::Mat src_grayscale;
    cv::cvtColor(src, src_grayscale, cv::COLOR_BGR2GRAY);
    
    cv::Mat detected_edges;
    cv::blur(src_grayscale, detected_edges, cv::Size(3,3));
    cv::Canny(detected_edges, detected_edges, 100, 200, 3, false);
    
    cv::imshow("edges", detected_edges);
    
    std::vector<cv::Mat> contours;
    
    cv::findContours(detected_edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    uint last_index = (uint) contours.size() - 1;
    
    cv::Mat aux;
    src.copyTo(aux);

    cv::drawContours(aux, contours, last_index, cv::Scalar(0,255,0), 2);
    cv::imshow("Detected Contour", aux);
//    std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2){
//        return cv::contourArea(c1) > cv::contourArea(c2);
//    });
//
//    cv::Mat aprox;
//    int index = 0;
//    for (int i = 0; i < contours.size(); i++){
//        std::cout<<cv::contourArea(contours[i])<<std::endl;
//        double peri = cv::arcLength(contours[i], true);
//        cv::approxPolyDP(contours[i], aprox, 0.02 * peri, true);
//        if (aprox.cols == 4){
//            index = i;
//            break;
//        }
//    }
    
    cv::Mat biggest_contour = contours[last_index];
    //cv::Mat biggest_contour = contours[0];
    
    double ep = cv::arcLength(biggest_contour, true);
    
    cv::Mat r;
    cv::approxPolyDP(biggest_contour, r, 0.02 * ep, true);

    r.reshape(4,2);
    cv::drawContours(src, r, -1, cv::Scalar(0,255,0), 2);
    cv::imshow("Four Point Detection", src);
    
    cv::MatIterator_<cv::Vec2i> iter, end = r.end<cv::Vec2i>();
    cv::MatIterator_<cv::Vec2i> j;
    std::vector<cv::Vec2i> final_corners;
    
    for (iter = r.begin<cv::Vec2i>(); iter != end; iter++){
        std::cout << "point: " << *iter << std::endl;
        if (r.rows > 4){
            for (j = iter + 1; j != end; j++){
                int first_subtraction = abs((*iter)[0] - (*j)[0]);
                int second_subtraction = abs((*iter)[1] - (*j)[1]);
                
                if ((first_subtraction < 8) and (second_subtraction < 8)){
                    final_corners.push_back(*iter);
                    break;
                }
            }
        }else{
            final_corners = r;
            break;
        }
    }
    
    std::cout<<std::endl;
    std::vector<cv::Point> not_a_rect_shape;
    cv::Point first_coordinate;
    cv::Point second_coordinate;
    cv::Point third_coordinate;
    cv::Point fourth_coordinate;
    
    std::sort(final_corners.begin(), final_corners.end(), [](const cv::Vec2i p1, const cv::Vec2i p2){
            return p1[0] < p2[0];
        });
    
    for (int i = 0; i < final_corners.size(); i++){
        std::cout << "final point: " << final_corners[i] << std::endl;
    }
    
    
    if (final_corners[0][1] < final_corners[1][1]){
        first_coordinate = cv::Point(final_corners[0][0], final_corners[0][1]);
        third_coordinate = cv::Point(final_corners[1][0], final_corners[1][1]);
    }else{
        third_coordinate = cv::Point(final_corners[0][0], final_corners[0][1]);
        first_coordinate = cv::Point(final_corners[1][0], final_corners[1][1]);
    }
    
    if (final_corners[2][1] < final_corners[3][1]){
        second_coordinate = cv::Point(final_corners[2][0], final_corners[2][1]);
        fourth_coordinate = cv::Point(final_corners[3][0], final_corners[3][1]);
    }else{
        fourth_coordinate = cv::Point(final_corners[2][0], final_corners[2][1]);
        second_coordinate = cv::Point(final_corners[3][0], final_corners[3][1]);
    }
    
    for (int i = 0; i < not_a_rect_shape.size(); i++){
        std::cout << "ordered corners: " << not_a_rect_shape[i] << std::endl;
    }

    
    
    cv::RotatedRect box = cv::minAreaRect(cv::Mat(final_corners));
    std::cout << "Rotated box set to (" << box.boundingRect().x << "," << box.boundingRect().y << ") " <<box.size.width << "x" << box.size.height << std::endl;
    

    
    
    cv::Point2f src_vertices[4];
    src_vertices[0] = first_coordinate;
    src_vertices[1] = second_coordinate;
    src_vertices[2] = third_coordinate;
    src_vertices[3] = fourth_coordinate;
    
    cv::Point2f dst_vertices[4];
    dst_vertices[0] = cv::Point(0, 0);
    dst_vertices[1] = cv::Point(box.boundingRect().width-1, 0); // Bug was: had mistakenly switched these 2 parameters
    dst_vertices[2] = cv::Point(0, box.boundingRect().height-1);
    dst_vertices[3] = cv::Point(box.boundingRect().width-1, box.boundingRect().height-1);
    
    
    cv::Mat warpMatrix = cv::getPerspectiveTransform(src_vertices, dst_vertices);

    cv::Mat rotated;
    cv::Size size(box.boundingRect().width, box.boundingRect().height);
    cv::warpPerspective(src, rotated, warpMatrix, size, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
    
//    cv::Mat warpAffineMatrix = cv::getAffineTransform(src_vertices, dst_vertices);
//    cv::Mat rotated;
//    cv::Size size(box.boundingRect().width, box.boundingRect().height);
//    cv::warpAffine(src, rotated, warpAffineMatrix, size, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
    
    cv::cvtColor(rotated, rotated, cv::COLOR_BGR2GRAY);
    cv::imshow("Pre Final Result", rotated);
    cv::adaptiveThreshold(rotated, rotated, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 10);
    cv::imshow("Final Result", rotated);
    

}


void DocumentScanner::display_image() {
    cv::imshow("InputImage", src);
    cv::waitKey(0);
}







