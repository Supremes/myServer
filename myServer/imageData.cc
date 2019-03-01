#include "imageData.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include "base/utility.h"
#include <unistd.h>
#include <cstring>
using namespace std;
using namespace cv;

imageData::imageData(EventLoop *loop, int connfd):
        feature_("surf"),
        warp_("plane"),
        seam_("gc_color"),
        error_(false),
        connfd_(connfd),
        loop_(loop),
        channel_(new Channel(loop_, connfd_))
        {
            channel_->setReadResponse(bind(&imageData::handleRead, this));

        }
imageData::~imageData(){  
    cout << "connection:dtor at "  << this << "fd = " << connfd_ << endl;;
    //close(connfd_);
}

void imageData::stitch(){
    cout << "stitching.." << endl;
    vector<Mat> imgs;
    for(int i = 0; i < names_.size(); i++)
        imgs.push_back(imread(names_[i]));
    Mat pano;
    Stitcher stitcher = Stitcher::createDefault();
    Stitcher::Status status = stitcher.stitch(imgs, pano);
    
    if(status != Stitcher::OK)
        outBuffer_ = "can't stitch images!\n";
    else{
        imwrite("pano.jpg", pano);
        outBuffer_ = "stitch success!\n";
    }
}
//灰度化处理
void imageData::imageProcess(){
    cout << "imageProcess..." << endl;
    Mat img = imread(name_);
    Mat gray, flipImg;
    cvtColor(img, gray, COLOR_RGB2GRAY);
    imwrite("gray.jpg", gray);
    outBuffer_ = "grayScale success\t";
    transpose(img, flipImg);
    flip(flipImg, flipImg, 2);
    imwrite("flip.jpg", flipImg);
    outBuffer_ += "flip success\t";
}

// void imageData::handleRead()
// {
//     cout << "imageData::handleRead() connfd_ = "<< connfd_ << endl;
//     // int numOfRead = readn(connfd_, inBuffer_);
//     // if(numOfRead < 0){
//     //     handleError();
//     //     error_ = true;
//     //     return ;
//     // }
//     char in[2048];
//     int numOfRead = read(connfd_, in, 2048);
//     // if(numOfRead < 0){
//     //     handleError();
//     //     error_ = true;
//     //     return ;
//     // }
//     write(connfd_, in, strlen(in));

//     //splitString(inBuffer_, " ", names_);
//     //cout << inBuffer_ << endl;
//     // char send[1000] = "received";
//     // writen(connfd_, send, strlen(send));
//     //stitch();
// }
void imageData::handleRead()
{
    //cout << "imageData::handleRead() connfd_ = "<< connfd_ << endl;
    int numOfRead = readn(connfd_, inBuffer_);
    if(numOfRead < 0){
        handleError();
        error_ = true;
        return ;
    }else if(numOfRead > 0){
        //splitString(inBuffer_, " ", names_);
        name_ = inBuffer_;
        imageProcess();
        //stitch();
        handleWrite();
    }
}
void imageData::handleWrite()
{
    //加入连接状态检测 connectionState
    cout << "imageData writing.." << endl;
    if(!error_){
        if(writen(connfd_, outBuffer_) < 0){
            perror("writen");
            error_ = true;
        }
        // else{
        //     cout << outBuffer_ << endl;
        //     cout << "write success!\n";
        // }
    }
}

void imageData::handleError()
{
    string errorInfo = "handleError";
    if(writen(connfd_, errorInfo) < 0){
        perror("writenError");
        error_ = true;
    }
}

void imageData::handleClose()
{
    loop_->removeFromPoller(channel_);
}
void imageData::handleNewEvent()
{
    channel_->setEvents(EPOLLIN | EPOLLOUT);
    setState(kConnected);
    loop_->addToPoller(channel_);
}

void imageData::connectDestroyed()
{
    if(connState_ = kConnected){
        connState_ = kDisconnected;
        channel_->disableAll();
        cout << "disconnect connection fd = " << connfd_ << endl;
    }
    channel_->remove();
}