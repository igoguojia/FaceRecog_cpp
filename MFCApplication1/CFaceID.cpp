// CFaceID.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CFaceID.h"
#include "afxdialogex.h"


// CFaceID 对话框

IMPLEMENT_DYNAMIC(CFaceID, CDialogEx)

CFaceID::CFaceID(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FaceID, pParent)
{

}

CFaceID::~CFaceID()
{
}

void CFaceID::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, FIDName);
}


BEGIN_MESSAGE_MAP(CFaceID, CDialogEx)
END_MESSAGE_MAP()


// CFaceID 消息处理程序


void CFaceID::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	FIDName.GetWindowText(FIDstr);
	CDialogEx::OnOK();
}
