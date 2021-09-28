
// PointCloudViewerDlg.cpp: 구현 파일
//
#pragma once

#include "pch.h"
#include "framework.h"
#include "PointCloudViewer.h"
#include "PointCloudViewerDlg.h"
#include "afxdialogex.h"

#include "../Common/CommonFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CPointCloudViewerDlg 대화 상자



CPointCloudViewerDlg::CPointCloudViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POINTCLOUDVIEWER_DIALOG, pParent)
	, m_pDC(nullptr)
	, fov(0)
	, m_isMouseButtonDown(false)
{
	m_projectionMatrix = glm::mat4(NULL);
	m_viewMatrix = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in  World Space, 월드 좌표계에서 카메라 위치
		glm::vec3(0, 0, 0), // and looks at the origin, 카메라가 바라보는 점(초점의 위치)
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down), 카메라 기울임
	);
	m_modelMatrix = glm::mat4(NULL);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPointCloudViewerDlg::~CPointCloudViewerDlg()
{
	//delete m_pointcloud;
	if (m_openGL != nullptr)
		delete m_openGL;
}

void CPointCloudViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEW_GL, m_view);
}

BEGIN_MESSAGE_MAP(CPointCloudViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CPointCloudViewerDlg::OnBnClickedButtonLoad)
	ON_WM_DESTROY()
	ON_WM_TIMER()
//	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPointCloudViewerDlg 메시지 처리기

BOOL CPointCloudViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_openGL = new GL::OpenGL(m_shaderProgram, vao, GetDlgItem(IDC_VIEW_GL));
	if (!m_openGL->GetRenderingContext())
	{
		AfxMessageBox(CString("OpenGL 초기화중 에러가 발생하여 프로그램을 실행할 수 없습니다."));
		return -1;
	}
	m_MVP = m_openGL->GetObjectMVP();
	//OpenGL context 생성
	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	//if (!GetRenderingContext())
	//{
	//	AfxMessageBox(CString("OpenGL 초기화중 에러가 발생하여 프로그램을 실행할 수 없습니다."));
	//	return -1;
	//}


	//GLuint shaderProgram;
	//DefineVAO(vao, shaderProgram);

	//glUseProgram(shaderProgram);
	//glBindVertexArray(vao);

	SetTimer(1000, 30, NULL);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CPointCloudViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPointCloudViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPointCloudViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPointCloudViewerDlg::OnBnClickedButtonLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString str = _T("All files(*.*)|*.*|"); // 모든 파일 표시
	// _T("Excel 파일 (*.xls, *.xlsx) |*.xls; *.xlsx|"); 와 같이 확장자를 제한하여 표시할 수 있음
	CFileDialog dlg(TRUE, _T("*.dat"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, this);
	CString strPathName;

	if (dlg.DoModal() == IDOK)
	{
		strPathName = dlg.GetPathName();
		// 파일 경로를 가져와 사용할 경우, Edit Control에 값 저장
		//SetDlgItemText(IDC_EDIT1, strPathName);
	}

	uint32_t size = strPathName.GetLength();
	char* filePath = new char[size + 1];
	strcpy_s(filePath, size + 1, CT2A(strPathName));

	PC::PointCloud pointcloud;
	PC::DATA::PointClouds clouds;
	//std::vector<PC::DATA::PointCloudData> clouds;
	if (!pointcloud.LoadPCD(filePath, clouds))
	{
		AfxMessageBox(_T("Fail to load file..."), MB_ICONSTOP);
	}
	 
	m_clouds = clouds;

	//GLuint shaderProgram;

	m_openGL->PointcloudeVAO(clouds, m_cloud_glmVec, m_MVP);
	//DefineVAO(vao, m_shaderProgram);

	m_openGL->GLUseProgram();
	//m_openGL->GLBindVertexArray(vao);
	//glUseProgram(m_shaderProgram);
	//glBindVertexArray(vao);

	delete[] filePath; 
}


void CPointCloudViewerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//glDeleteVertexArrays(1, &vao);

	//if (FALSE == ::wglDeleteContext(m_hRC))
	//{
	//	AfxMessageBox(CString("wglDeleteContext failed"));
	//}
}

// 화면 업데이트용 타이머
void CPointCloudViewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);

	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto& mvp = m_openGL->GetObjectMVP_Ptr();
	m_MVP = mvp;

	m_openGL->GLUseProgram();
	m_openGL->GLUniformMatrix4fv(m_matrixId, 1, GL_FALSE, &m_MVP[0][0]);

	// GL_P[OINTS로 하면 큐브 생성 없이 포인트로 출력됨
	m_openGL->GLDrawArrays(GL_POINTS, 0, m_cloud_glmVec.size());
	//m_openGL->GLDrawElements(GL_POINTS, m_cloud_glmVec.size() * sizeof(glm::vec3), GL_FLOAT, (void*)0);

	//화면 업데이트
	//SwapBuffers(m_pDC->GetSafeHdc());
	SwapBuffers(m_openGL->GetWindowDC()->GetSafeHdc());
}

BOOL CPointCloudViewerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_openGL->MouseWheelEvent(zDelta);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CPointCloudViewerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (!m_isMouseButtonDown) return;

	//float angle_degree = GL::CommFunc::GetDegreeAngle(m_firstClickPoint.x, m_firstClickPoint.y, point.x, point.y);

	CString str("Mouse move\n");
	TRACE(str);
	////str.Format(_T("angle: %f"), angle_degree);
	////TRACE(str);
	str.Format(_T("point x: %d\n"), point.x);
	TRACE(str);
	str.Format(_T("point y: %d\n"), point.y);
	TRACE(str);

	m_openGL->RotateObject(point.x, point.y);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CPointCloudViewerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

#ifdef _DEBUG
	CString str("Mouse button\n");
	TRACE(str);
	str.Format(_T("point x: %d\n"), point.x);
	TRACE(str);
	str.Format(_T("point y: %d\n"), point.y);
	TRACE(str);
#endif
	m_isMouseButtonDown = true;
	m_firstClickPoint = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPointCloudViewerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_isMouseButtonDown = false;

	CDialogEx::OnLButtonUp(nFlags, point);
}
