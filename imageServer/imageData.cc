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
            //channel_->setWriteResponse(bind(&imageData::handleWrite, this));
            //channel_->setErrorResponse(bind(&imageData::handleError, this));
            setSocketKeepAlive(connfd_, true);
            // channel_->setEvents(EPOLLIN | EPOLLOUT);
            // loop_->addToPoller(channel_);   
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
        outBuffer_ = "can't stitch images ,error code = ERR_NEED_MORE_IMGS";
    else{
        imwrite("result.jpg", pano);
        outBuffer_ = "stitch success!";
    }
    cout << outBuffer_.c_str() << endl;
    //handleWrite();
}

void imageData::handleRead()
{
    cout << "imageData::handleRead() connfd_ = "<< connfd_ << endl;
    // int numOfRead = readn(connfd_, inBuffer_);
    // if(numOfRead < 0){
    //     handleError();
    //     error_ = true;
    //     return ;
    // }
    char in[2048];
    int numOfRead = read(connfd_, in, 2048);
    // if(numOfRead < 0){
    //     handleError();
    //     error_ = true;
    //     return ;
    // }
    inBuffer_ = in;
    splitString(inBuffer_, " ", names_);
    cout << inBuffer_ << endl;
    char send[1000] = "received\n";
    write(connfd_, send, strlen(send));
    //stitch();
}

void imageData::handleWrite()
{
    //加入连接状态检测 connectionState
    cout << "imageData writing.." << endl;
    //outBuffer_ = "imageData writing..";
    if(!error_){
        if(writen(connfd_, outBuffer_) < 0){
            perror("writen");
            error_ = true;
        }
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
        cout << "disconnect connection fd = " << fd << endl;
    }
    channel_->remove();
}