
// MFCApplication1Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include<opencv.hpp>
#include<fstream>
#include"CvvImage.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma warning(disable:4996)
#include<iostream>
#include <time.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include"CFaceID.h"
#include <mutex>

using namespace cv;
using namespace std;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 对话框



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, Information_Box);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BEGIN, &CMFCApplication1Dlg::OnBnClickedBegin)
	//ON_BN_CLICKED(IDC_SELECT, &CMFCApplication1Dlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDC_TRAIN, &CMFCApplication1Dlg::OnBnClickedTrain)
	ON_BN_CLICKED(IDC_RECOG, &CMFCApplication1Dlg::OnBnClickedRecog)
	//ON_BN_CLICKED(IDC_TAKEPHOTO, &CMFCApplication1Dlg::OnBnClickedTakephoto)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedAddFace)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序
Ptr<FaceRecognizer> modelPCA;
Ptr<FaceRecognizer> modelFisher;
Ptr<FaceRecognizer> modelLBP;
BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	modelPCA = createEigenFaceRecognizer();
	modelPCA->load("..\\xml\\PCA_Model.xml");
	modelFisher = createFisherFaceRecognizer();
	modelFisher->load("..\\xml\\Fisher_Model.xml");
	modelLBP = createLBPHFaceRecognizer();
	modelLBP->load("..\\xml\\LBP_Model.xml");


	char separator = ';';

	ifstream file("..\\FaceID\\faceinfo.txt", ifstream::in);    //以读入的方式打开文件
	string line,label,sfolder,facename;
	//int a = 0;
	while (getline(file, line))                       //从文本文件中读取一行字符，未指定限定符默认限定符为“/n”
	{
		stringstream lines(line);
		getline(lines, label, separator);               //根据指定分割符进行分割，分为“路径+标号”
		getline(lines, sfolder, separator);               //根据指定分割符进行分割，分为“路径+标号”
		getline(lines, facename);
		if (!facename.empty() && !label.empty()&& !sfolder.empty())           //如果读取成功，则将图片和对应标签压入对应容器中 
		{
			vectorFace vFace;			
			vFace.Label = label;
			vFace.sFolder = sfolder;
			vFace.Name = facename;
			FaceList.push_back(vFace);
		}
		//cout << a++;
	}
	file.close();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*void detectAndDisplay(Mat frame, CascadeClassifier face_cascade, CascadeClassifier eyes_cascade, String window_name)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces  
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, Size(70, 70), Size(100, 100));

	for (size_t i = 0; i < faces.size(); i++)
	{
		//Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);  
		//ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);  
		rectangle(frame, faces[i], Scalar(255, 0, 0), 2, 8, 0);

		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;

		//-- In each face, detect eyes  
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 1, CV_HAAR_DO_ROUGH_SEARCH, Size(3, 3));

		for (size_t j = 0; j < eyes.size(); j++)
		{
			Rect rect(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y, eyes[j].width, eyes[j].height);

			//Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);  
			//int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);  
			//circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);  
			rectangle(frame, rect, Scalar(0, 255, 0), 2, 8, 0);
		}
	}
	//-- Show what you got  
	namedWindow(window_name, 2);
	imshow(window_name, frame);
}*/
mutex g_io_mutex;
mutex g_io_mutex2;

void CMFCApplication1Dlg::OnBnClickedBegin()
{
	// TODO: 在此添加控件通知处理程序代码
	//Recog=FALSE;

	if (!PathIsDirectory(_T("..\\facedata")))
	{
		CreateDirectory(_T("..\\facedata"), 0);
	}
	pFrame=NULL;
	pCapture = cvCreateCameraCapture(1);
	cvNamedWindow("video", 1);
	HWND hWnd = (HWND)cvGetWindowHandle("video");
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	int nFrmNum = 0;
	string face_cascade_name = "..\\haarcascades\\haarcascade_frontalface_default.xml";
	string eyes_cascade_name = "..\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;
	if (!face_cascade.load(face_cascade_name)|| !eyes_cascade.load(eyes_cascade_name))
	{
		AfxMessageBox(_T("级联分类器错误，可能未找到文件，拷贝该文件到工程目录下！"));
	}
	Cancel = FALSE;
	Enabletakephoto = FALSE;
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	//GetDlgItem(IDC_TAKEPHOTO)->EnableWindow(TRUE);
	while (pFrame = cvQueryFrame(pCapture))
	{

		//cvShowImage("video",pFrame);
		//char c = (char)waitKey(10);
		//if(c==27)
			//break;
		nFrmNum++;
		//int face_num = 0;
		vector<Rect> faces(100);
		IplImage* face_gray=NULL;
		face_gray = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
		cvCvtColor(pFrame, face_gray, COLOR_BGR2GRAY);  //rgb类型转换为灰度类型
		cvEqualizeHist(face_gray, face_gray);   //直方图均衡化
		face_cascade.detectMultiScale(face_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, Size(100, 100));
		//face_num = faces.size();
		char faceName[30];
		//fi++;
		sprintf(faceName, "..\\facedata\\s%d\\%d.jpg",sFolderOrder ,fi);
		for (int i = 0; i < faces.size(); i++)
		{
			//cvNamedWindow("face", 1);
			g_io_mutex2.lock();
			cvRectangle(pFrame, cvPoint(faces[i].x, faces[i].y), cvPoint(faces[i].x + faces[i].width, faces[i].y + faces[i].height), CV_RGB(255, 0, 0), 1,8,0);
			g_io_mutex2.unlock();
			cvSetImageROI(pFrame, cvRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
			//在目标图像中截出人脸区域
			IplImage* face1 = cvCreateImage(cvSize(faces[i].width, faces[i].height), pFrame->depth, pFrame->nChannels);
			IplImage* face2 = cvCreateImage(cvSize(92, 112), pFrame->depth, pFrame->nChannels);
			cvCopy(pFrame, face1);
			cvResetImageROI(pFrame);

			/*vector<Rect> eyes(1000);
			//-- In each face, detect eyes  
			eyes_cascade.detectMultiScale(face_gray, eyes, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH,Size(0,0) , Size(70, 70));
			for (size_t j = 0; j < eyes.size(); j++)
			{	
				g_io_mutex.lock();
				cvRectangle(pFrame, cvPoint(eyes[i].x, eyes[i].y), cvPoint(eyes[i].x + eyes[i].width, eyes[i].y + eyes[i].height), CV_RGB(0, 255, 0), 1, 8, 0);
				g_io_mutex.unlock();
				cvSetImageROI(pFrame, cvRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
				//在目标图像中截出人脸区域
				IplImage* face1 = cvCreateImage(cvSize(faces[i].width, faces[i].height), pFrame->depth, pFrame->nChannels);
				//IplImage* face2 = cvCreateImage(cvSize(92, 112), pFrame->depth, pFrame->nChannels);
				cvCopy(pFrame, face1);
				cvResetImageROI(pFrame);			
				if (eyes.size()>900)
				{
					//eyes.clear();
					eyes.swap(vector<Rect>(1000));
					AfxMessageBox(_T("清空内存"));
					//break;
				}
				
			}*/
			if (Enabletakephoto)
			{
				char c = (char)waitKey(15);
				//if (c == 'e') break;
				switch (c)
				{
				case 'p':
					//if (face1->width > 100)
					//{
					cvResize(face1, face2, CV_INTER_CUBIC);
					//}
					cvSaveImage(faceName, face2);
					cvShowImage("face", face2);
					waitKey(500);
					cvDestroyWindow("face");
					if(fi++==10)
						GetDlgItem(IDC_TRAIN)->EnableWindow(TRUE);
					break;
				default:
					break;
				}
			}
			if (nFrmNum%20==0&&!Enabletakephoto)
			{
				/*time_t curtime;
				time(&curtime);
				char *time1 = ctime(&curtime);*/
				
				time(&rawtime);
				info = localtime(&rawtime);
				strftime(buffer, 30, "%Y%m%d_%H%M%S", info);

				sprintf(tempjpg, "..\\people\\%s.jpg",buffer);
				cvResize(face1, face2, CV_INTER_CUBIC);
				if (!cvSaveImage(tempjpg, face2))
				{
					AfxMessageBox(_T("error！"));

				}
				memset(buffer, '\0', sizeof(buffer));
				OnBnClickedRecog();

			}
			//cvDestroyWindow("face");
			cvReleaseImage(&face1);
			cvReleaseImage(&face2);
			/*if (i >= 1000-1)
			{
				faces.clear();
				break;
			}*/
		}
		cvShowImage("video", pFrame);
		cvReleaseImage(&face_gray);
		if (waitKey(30) == 27 || Cancel)
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
			break;
		}

	}
	//cvWaitKey(300);	
	//cvReleaseImage(&pFrame);	
	cvDestroyWindow("video");	
	cvReleaseCapture(&pCapture);
	pCapture = NULL;

	//pFrame = NULL;
	

	/*String face_cascade_name = "..\\haarcascades\\haarcascade_frontalface_default.xml";
	String eyes_cascade_name = "..\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier face_cascade;   //定义人脸分类器  
	CascadeClassifier eyes_cascade;   //定义人眼分类器  
	String window_name = "Capture - Face detection";

	VideoCapture cap;	
	Mat frame;

	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return; };
	cap.open(1);
	if (!cap.isOpened()) { printf("--(!)Error opening video capture\n"); return; }
	
	while (cap.read(frame))
	{
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		detectAndDisplay(frame, face_cascade, eyes_cascade, window_name);
		int c = waitKey(0);
		if ((char)c == 27) { return; } // escape  

	}*/
}

void getAllFiles(int num, string path, vector<string>& files)
{
	//文件句柄
	long hFile = 0;
	string str_num;
	//char num = '0';//标签
	//string str_num(2, num);
	//文件信息
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类 很有意思的一个赋值函数:assign()，有很多重载版本
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) 
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{  //比较文件类型是否是文件夹
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					getAllFiles(num,p.assign(path).append("\\").append(fileinfo.name), files);
					num++;
				}
			}
			else
			{
				str_num = to_string(num);
				files.push_back(p.assign(path).append("\\").append(fileinfo.name).append(";").append(str_num));
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
}
void csv()
{
	// TODO: 在此添加控件通知处理程序代码
	char* filePath = "..\\facedata";
	vector<string> files;
	char* distAll = "..\\csv\\AllFiles.txt";
	int num = 0;//标签
	getAllFiles(num, filePath, files);
	ofstream ofn(distAll);
	int size = files.size();
	//ofn << size << endl;
	for (int i = 0; i < size; i++)
	{
		ofn << files[i] << endl;

	}
	ofn.close();
}
/*void CMFCApplication1Dlg::TakePhoto()
{
		// TODO: 在此添加控件通知处理程序代码
		//Recog=FALSE;

		if (!PathIsDirectory(_T("..\\facedata")))
		{
			CreateDirectory(_T("..\\facedata"), 0);
		}
		IplImage* pFrame = NULL;
		pCapture = cvCreateCameraCapture(1);
		cvNamedWindow("video", 1);
		HWND hWnd = (HWND)cvGetWindowHandle("video");
		::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		int nFrmNum = 0;
		string face_cascade_name = "..\\haarcascades\\haarcascade_frontalface_default.xml";
		string eyes_cascade_name = "..\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
		CascadeClassifier face_cascade;
		CascadeClassifier eyes_cascade;
		if (!face_cascade.load(face_cascade_name) || !eyes_cascade.load(eyes_cascade_name))
		{
			AfxMessageBox(_T("级联分类器错误，可能未找到文件，拷贝该文件到工程目录下！"));
		}

		//GetDlgItem(IDC_TAKEPHOTO)->EnableWindow(TRUE);
		while (pFrame = cvQueryFrame(pCapture))
		{

			//cvShowImage("video",pFrame);
			//char c = (char)waitKey(10);
			//if(c==27)
				//break;
			nFrmNum++;
			//int face_num = 0;
			vector<Rect> faces(10000);
			IplImage* face_gray = NULL;
			face_gray = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			cvCvtColor(pFrame, face_gray, COLOR_BGR2GRAY);  //rgb类型转换为灰度类型
			cvEqualizeHist(face_gray, face_gray);   //直方图均衡化
			face_cascade.detectMultiScale(face_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, Size(100, 100));
			//face_num = faces.size();
			char faceName[30];
			//fi++;
			sprintf(faceName, "..\\facedata\\s41\\%d.jpg", fi);
			for (int i = 0; i < faces.size(); i++)
			{
				//cvNamedWindow("face", 1);
				cvRectangle(pFrame, cvPoint(faces[i].x, faces[i].y), cvPoint(faces[i].x + faces[i].width, faces[i].y + faces[i].height), CV_RGB(255, 0, 0), 1, 8, 0);
				cvSetImageROI(pFrame, cvRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
				//在目标图像中截出人脸区域
				IplImage* face1 = cvCreateImage(cvSize(faces[i].width, faces[i].height), pFrame->depth, pFrame->nChannels);
				IplImage* face2 = cvCreateImage(cvSize(92, 112), pFrame->depth, pFrame->nChannels);
				cvCopy(pFrame, face1);
				cvResetImageROI(pFrame);

				vector<Rect> eyes(10000);
				//-- In each face, detect eyes  
				eyes_cascade.detectMultiScale(face_gray, eyes, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, Size(0, 0), Size(70, 70));
				for (size_t j = 0; j < eyes.size(); j++)
				{
					cvRectangle(pFrame, cvPoint(eyes[i].x, eyes[i].y), cvPoint(eyes[i].x + eyes[i].width, eyes[i].y + eyes[i].height), CV_RGB(0, 255, 0), 1, 8, 0);
					cvSetImageROI(pFrame, cvRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
					//在目标图像中截出人脸区域
					IplImage* face1 = cvCreateImage(cvSize(faces[i].width, faces[i].height), pFrame->depth, pFrame->nChannels);
					//IplImage* face2 = cvCreateImage(cvSize(92, 112), pFrame->depth, pFrame->nChannels);
					cvCopy(pFrame, face1);
					cvResetImageROI(pFrame);
					if ((eyes.capacity() - eyes.size()) < 10)
					{
						eyes.clear();
						eyes.swap(vector<Rect>(1000));
						AfxMessageBox(_T("清空内存"));
						break;
					}

				}
				char c = (char)waitKey(15);
				switch (c)
				{
				case 'p':
					cvResize(face1, face2, CV_INTER_CUBIC);
					cvSaveImage(faceName, face2);
					cvShowImage("face", face2);
					waitKey(500);
					cvDestroyWindow("face");
					fi++;
					break;
				default:
					break;
				}
				cvReleaseImage(&face1);
				cvReleaseImage(&face2);
			}
			cvShowImage("video", pFrame);
			cvReleaseImage(&face_gray);
			if (waitKey(30) == 27 )
				break;

		}
		cvDestroyWindow("video");
		cvReleaseCapture(&pCapture);
		pCapture = NULL;

}*/
void CMFCApplication1Dlg::OnBnClickedAddFace()
{	
	fi = 1;
	CFaceID IDdlg;
	//string str;
	namestr.clear();
	if (IDOK == IDdlg.DoModal())
	{
		namestr = IDdlg.FIDstr;
	}

	string tempstring = FaceList.back().sFolder;
	sFolderOrder=atoi(tempstring.erase(0, 1).c_str());
	sFolderOrder++;	
	dir = "..\\facedata\\s" + to_string(sFolderOrder);
	if (!PathIsDirectory(dir.c_str()))
	{
		CreateDirectory(dir.c_str(), 0);
	}
	Enabletakephoto = TRUE;
	AfxMessageBox(_T("请拍摄至少10张被检测到的人脸图片,按p拍照"));

	/*str.insert(0, ";");
	str.insert(0, to_string(sFolderOrder));
	str.insert(0, "s");
	str.insert(0, to_string(nameOrder));
	ofstream ofn("..\\FaceID\\faceinfo.txt", ios::app);
	ofn << str << endl;
	ofn.close();
	nameOrder++;
*/
}
//void CMFCApplication1Dlg::OnBnClickedSelect()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CString filter;
//	filter = "文件类型(*.txt)|*.txt||";
//	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, filter);
//	if (dlg.DoModal() == IDOK)
//	{	// csvPath1=dlg.GetPathName();
//		csvPath1 = dlg.GetPathName();
//		GetDlgItem(IDC_TRAIN)->EnableWindow(TRUE);
//	}
//}

void CMFCApplication1Dlg::OnBnClickedTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	//string csvPath = CW2A(csvPath1.GetString());

	//string csvPath((LPCTSTR)csvPath1);
	//CString tmp; 
	//tmp=csvPath.c_str(); 
	//AfxMessageBox(tmp);
	Cancel = TRUE;
	csv();
	std::vector<Mat> images;
	std::vector<int> labels;
	char separator = ';';

	ifstream file("..\\csv\\AllFiles.txt", ifstream::in);    //以读入的方式打开文件
	String line, path, label;
	int a = 0;
	while (getline(file, line))                       //从文本文件中读取一行字符，未指定限定符默认限定符为“/n”
	{
		stringstream lines(line);
		getline(lines, path, separator);               //根据指定分割符进行分割，分为“路径+标号”
		getline(lines, label);
		if (!path.empty() && !label.empty())           //如果读取成功，则将图片和对应标签压入对应容器中 
		{
			images.push_back(imread(path, 0));        //读取训练样本
			labels.push_back(atoi(label.c_str()));   //读取训练样本标号
		}
		String temppath = dir + "\\1.jpg";
		if (path == temppath)
			nameOrder = labels.back();
		cout << a++;
	}
	vectorFace tempvector;
	tempvector.Label = to_string(nameOrder);
	tempvector.Name = namestr;
	tempvector.sFolder = to_string(sFolderOrder);

	namestr.insert(0, ";");
	namestr.insert(0, to_string(sFolderOrder));
	namestr.insert(0, "s");
	namestr.insert(0, ";");
	namestr.insert(0, to_string(nameOrder));
	ofstream ofn("..\\FaceID\\faceinfo.txt", ios::app);
	ofn << namestr << endl;
	ofn.close();

	Ptr<FaceRecognizer> modelPCA1 = createEigenFaceRecognizer();
	modelPCA1->train(images, labels);
	modelPCA1->save("..\\xml\\PCA_Model.xml");



	Ptr<FaceRecognizer> modelFisher1 = createFisherFaceRecognizer();
	modelFisher1->train(images, labels);
	modelFisher1->save("..\\xml\\Fisher_Model.xml");

	Ptr<FaceRecognizer> modelLBP1 = createLBPHFaceRecognizer();
	modelLBP1->train(images, labels);
	modelLBP1->save("..\\xml\\LBP_Model.xml");
	AfxMessageBox(_T("训练成功！"));
	GetDlgItem(IDC_TRAIN)->EnableWindow(FALSE);

}

void CMFCApplication1Dlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	/*cvDestroyAllWindows();
	cvReleaseCapture(&pCapture);
	pCapture = NULL;*/
	Cancel = TRUE;
	CDialogEx::OnCancel();
}


void CMFCApplication1Dlg::OnBnClickedRecog()
{
	// TODO: 在此添加控件通知处理程序代码
	IplImage *image = NULL;			
	IplImage *temp = NULL;

	/*if (b_Flag)
	{
		image = cvLoadImage(FilePathName3, 0);
	}
	if (b_Flag1)
	{
	}*/
	//temp = cvQueryFrame(pCapture);
	/*cvSaveImage("..\\people\\temp.jpg", pFrame);
	//cvDestroyAllWindows();
	//cvReleaseCapture(&pCapture);
	//pCapture = NULL;
	cvDestroyWindow("video");
	cvReleaseCapture(&pCapture);
	pCapture = NULL;*/

	image = cvLoadImage(tempjpg, 0);
	//memset(tempjpg, '\0', sizeof(tempjpg));

	CDC *pDC = GetDlgItem(IDC_PICTURE)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(IDC_PICTURE)->GetClientRect(&rect);
	CvvImage cimg;

	cimg.CopyOf(image);
	cimg.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);
	//UpdateData();

	if (!PathFileExists(_T("..\\xml\\PCA_Model.xml")) || !PathFileExists(_T("..\\xml\\Fisher_Model.xml")) || !PathFileExists(_T("..\\xml\\LBP_Model.xml")))
		AfxMessageBox(_T("请确保训练模型在xml文件夹中！"));
	if (!image)
		AfxMessageBox(_T("请选择待识别图片！"));

	if (image)
	{
		// TODO: 在此添加控件通知处理程序代码
		/*Ptr<FaceRecognizer> modelPCA = createEigenFaceRecognizer();
		modelPCA->load("..\\xml\\PCA_Model.xml");*/
		/*Ptr<FaceRecognizer> modelFisher = createFisherFaceRecognizer();
		modelFisher->load("..\\xml\\Fisher_Model.xml");
		Ptr<FaceRecognizer> modelLBP = createLBPHFaceRecognizer();
		modelLBP->load("..\\xml\\LBP_Model.xml");*/


		Mat testImage(image, 0);
		//Mat testImage(image);
		Mat MyFace;
		if (testImage.cols != 92 || testImage.rows != 112)
		{
			resize(testImage, MyFace, Size(92, 112));
			int predictPCA = modelPCA->predict(MyFace);
			int predictLBP = modelLBP->predict(MyFace);
			int predictFisher = modelFisher->predict(MyFace);

			if (predictPCA==predictLBP || predictLBP == predictFisher || predictFisher == predictPCA)
			{
				BOOL finded = FALSE;
				for (auto n:FaceList)
				{
					if (predictPCA == atoi(n.Label.c_str()))
					{
						char new_tempjpg[50] = "";
						Information_Box.AddString(n.Name.c_str());
						finded = TRUE;
						sprintf(new_tempjpg, "%s%s.jpg", tempjpg, n.Name.c_str());
						rename(tempjpg, new_tempjpg);
						Invalidate();
						break;

					}

				}
				/*if (predictPCA == 35 || predictLBP == 35 || predictFisher == 35)
				{
					Information_Box.AddString(_T("李泽坤"));
					//AfxMessageBox(_T("识别成功"), MB_ICONINFORMATION);
				}*/
				if(!finded)
				{
					char new_tempjpg[50] = "";
					Information_Box.AddString(_T("查无此人"));
					sprintf(new_tempjpg, "%s查无此人.jpg",tempjpg);
					rename(tempjpg, new_tempjpg);
					//AfxMessageBox(_T("查无此人"), MB_ICONERROR);
					Invalidate();
				}
			}
			else
			{
				char new_tempjpg[50] = "";
				Information_Box.AddString(_T("人像判断不准确"));
				sprintf(new_tempjpg, "%s人像判断不准确.jpg", tempjpg);
				rename(tempjpg, new_tempjpg);
				//AfxMessageBox(_T("查无此人"), MB_ICONERROR);
				//AfxMessageBox(_T("查无此人"), MB_ICONERROR);
				Invalidate();
			}

		}
		else {

			int predictPCA = modelPCA->predict(testImage);
			int predictLBP = modelLBP->predict(testImage);
			int predictFisher = modelFisher->predict(testImage);

			//if (predictPCA)// == predictLBP || predictLBP == predictFisher || predictFisher == predictPCA)
			if (predictPCA == predictLBP || predictLBP == predictFisher || predictFisher == predictPCA)
			{
				BOOL finded = FALSE;
				for (auto n : FaceList)
				{
			
					if (predictPCA == atoi(n.Label.c_str()))
					{
						char new_tempjpg[50] = "";
						Information_Box.AddString(n.Name.c_str());
						sprintf(new_tempjpg, "%s%s.jpg", tempjpg, n.Name.c_str());
						rename(tempjpg, new_tempjpg);
						finded = TRUE;
						//Invalidate();
						break;
					}
				}
				//Invalidate();

				/*if (predictPCA == 35 || predictLBP == 35 || predictFisher == 35)
				{
					Information_Box.AddString(_T("李泽坤"));
					//AfxMessageBox(_T("识别成功"), MB_ICONINFORMATION);
					finded = TRUE;

				}*/
				//Invalidate();
				if(!finded)
				{
					char new_tempjpg[50] = "";
					Information_Box.AddString(_T("查无此人"));
					sprintf(new_tempjpg, "%s查无此人.jpg", tempjpg);
					rename(tempjpg, new_tempjpg);
					//AfxMessageBox(_T("查无此人"), MB_ICONERROR);
					Invalidate();
				}
				
			}
			else
			{
				char new_tempjpg[50] = "";
				Information_Box.AddString(_T("人像判断不准确"));
				sprintf(new_tempjpg, "%s人像判断不准确.jpg", tempjpg);
				rename(tempjpg, new_tempjpg);
				//AfxMessageBox(_T("查无此人"), MB_ICONERROR);
				Invalidate();
			}
			
		}
	}

}





