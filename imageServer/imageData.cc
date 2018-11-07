#include "imageData.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include "base/utility.h"

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
            channel_->setWriteResponse(bind(&imageData::handleWrite, this));
            //channel_->setErrorResponse(bind(&imageData::handleError, this));
        }
void imageData::stitch(){
    vector<Mat> imgs;
    for(int i = 0; i < imgs.size(); i++)
        imgs.push_back(imread(names_[i]));
    Mat pano;
    Stitcher stitcher = Stitcher::createDefault();
    Stitcher::Status status = stitcher.stitch(imgs, pano);

    if(status != Stitcher::OK)
        outBuffer_ = "can't stitch images ,error code = ERR_NEED_MORE_IMGS";
    else
        outBuffer_ = "stitch success!";

    handleWrite();
    

}

void imageData::handleRead()
{
    bool zero = false;
    int numOfRead = readn(connfd_, inBuffer_, zero);
    if(numOfRead < 0){
        handleError();
        error_ = true;
        return ;
    }else if(zero){
        //有请求但未读到数据
        error_ = true;
        handleError();
    }
    splitString(inBuffer_, " ", names_);
    stitch();
}

void imageData::handleWrite()
{
    //加入连接状态检测 connectionState
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
    loop_->addToPoller(channel_);
}