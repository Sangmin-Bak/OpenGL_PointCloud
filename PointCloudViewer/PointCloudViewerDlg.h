
// PointCloudViewerDlg.h: 헤더 파일
//

#pragma once

#include <cstdio>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "../PointCloud/PointCloud.h"
#include "../Graphics/OpenGL.h"


// CPointCloudViewerDlg 대화 상자
class CPointCloudViewerDlg : public CDialogEx
{
// 생성입니다.
public:
	CPointCloudViewerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	virtual ~CPointCloudViewerDlg();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POINTCLOUDVIEWER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	HGLRC m_hRC;
	CDC* m_pDC;

	GLuint vao;

	GLuint m_shaderProgram;
	GLuint m_matrixId;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_modelMatrix;
	glm::mat4 m_MVP;


	PC::DATA::PointClouds m_clouds;
	std::vector<glm::vec3> m_cloud_glmVec;
	GL::OpenGL* m_openGL;
	
	CPoint m_firstClickPoint;
	bool m_isMouseButtonDown;
	short fov;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad();
	CStatic m_view;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};