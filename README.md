# FaceRecog_cpp
FaceRecog
opencv给出的教程里面的第一个数据集：  
ORL Database of Faces（https://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html）。又称ORL人脸数据库  
级联分类器  
haarcascade_eye_tree_eyeglasses.xml  
haarcascade_frontalface_default.xml  

## 一、	实验目的：
通过建立自己的人脸检测识别系统，深入理解人机交互设计。
## 二、	实验要求
能够调用摄像头实时抓取图像，如果图像中出现人脸，能够检测定位人脸，同时保存人脸，建立自己的人脸样本集；训练人脸识别模型；测试人脸识别模型。
## 三、	实验结果分析
### 一键录入人脸数据，自动打标签，并且训练模型。
### 能够有效检测人脸数据
### 对于陌生人进行预警
### 记录并保存检测者照片
## 四、实验问题与初步解决
###  1.	 Opencv2.4与vs2017不适配，会导致人脸检测时，不自动释放vector空间，造成栈溢出。*未完全解决  
  解决：加了线程锁，确保进行数据写入时线程不被打断
  mutex g_io_mutex;  
  g_io_mutex.lock();  
  cvRectangle(pFrame, cvPoint(faces[i].x, faces[i].y), cvPoint(faces[i].x + faces[i].width, faces[i].y + faces[i].height), CV_RGB(255, 0, 0), 1,8,0);  
  g_io_mutex.unlock();  
###  2.   Opencv2.4.9缺少两个dll  
    向OpenCV2.4.9库路径(\opencv2\opencv\build\x86\vc12\bin)添加两个文件，分别为msvcr120d.dll（32位）以及msvcp120d.dll（32位）。  
    这两个文件需要从网上下载。  
###  3.	如何用opencv把图片显示到MFC的picture控件上  
  void CTestDlg::DrawPicToHDC(IplImage *img, UINT ID)  
  {  
  CDC *pDC = GetDlgItem(ID)->GetDC();  
  HDC hDC = pDC->GetSafeHdc();  
  CRect rect;  
  GetDlgItem(ID)->GetClientRect(&rect);  
  CvvImage cimg;  
  cimg.CopyOf(img);  
  cimg.DrawToHDC(hDC, &rect);  

  ReleaseDC(pDC);  
  }  
  //需要已经移除的头文件  
  Opencv 2.2以上的版本不再包含CvvImage类，可有时我们在MFC中显示图片仍然需要CvvImage类，特别进行图像的拷贝、显示等操作的时候。  
  早期版本的CvvImage.h添加进工程也是可行的。  

![iamge](https://github.com/igoguojia/FaceRecog_cpp/blob/main/ScreenShot/1.png)
