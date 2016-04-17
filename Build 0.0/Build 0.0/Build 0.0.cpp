// Build 0.0.cpp : Defines the entry point for the application.
// OptiFit Main program. Coded by Chris Esterer
// Skeliton code generated by Microsoft Visual Studio

#include "stdafx.h"
#include "Build 0.0.h"
#include "OptiBody.h"	// Alaysis Class
#include "BodyBasics.h" // Kinect
#include "Wireless.h" // Wireless
#include <iostream> //beep

#define MAX_LOADSTRING 100
#define HIPSANGLE_PNUM 0
#define WRISTSACCELL_PNUM 1
#define WRISTSVELO_PNUM 2
#define OPTIFIT_PNUM 3
#define WIRELESS_PNUM 4
#define	EMG_PNUM 5
#define FORCE_PNUM 6
#define THREAD_MAX 9
//Process number linked to: window, thread, and GuiApp itterator ** added

// Global Variables:
HINSTANCE hInst;                                // current instance
HINSTANCE KinectInst;							// Kinect Instance
HINSTANCE WirelessInst;
int Show;										// Variable for the Show of windo (i.e. minimized, full screen, ect)
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
//ATOM				RegisterKinectClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CBodyBasics kinectApp; // App Variables and Arrays
Wireless WirelessApp;
GUIApp GuiApp[8];		// App Array
GUIApp* GuiAppPtr[8];
// Threading
std::vector<std::thread> WirelessThread;
std::vector<std::thread> KinectThread;
std::vector<std::thread> GUIThread;
static std::array<std::thread::id, THREAD_MAX> ToClose;



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here. //***


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BUILD00, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BUILD00));

    MSG msg;
	LPMSG lpMsg;
	// New Data message loop:


    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		if (msg.message == 0x8001) {
			
			Sleep(1);
		}
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);

            DispatchMessage(&msg);
        }

    }


    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;// points to window procedure
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUILD00)); //*** Commented out Build 0.0 Icon to try to fix error "CVTRES : fatal error CVT1100: duplicate resource.  type:ICON, name:1, language:0x0409"
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BUILD00);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));//***

    return RegisterClassExW(&wcex);// Register the window class.
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   Show = nCmdShow; // **
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

  // CBodyBasics Body1;//***
 //  Body1.Run(hInstance, nCmdShow);//***

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ChecktoCloseThreads();
	//CheckThreads();

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:

                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				WirelessThread.back().join(); WirelessThread.pop_back();
				KinectThread.back().join(); KinectThread.pop_back();
				GUIThread.back().join(); GUIThread.pop_back();
                DestroyWindow(hWnd);
                break;
			case ID_KINECT_RUN:
				KinectThread.push_back(std::thread(StartKinect,hWnd, message, wParam, lParam));
				WirelessThread.push_back(std::thread(StartWireless, hWnd, message, wParam, lParam));
				break;
			case ID_HIPS_ANGLE:
				GuiApp[OPTIFIT_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[HIPSANGLE_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
			//	GuiApp[HIPSANGLE_PNUM].setJointTypes(JointType_SpineMid, JointType_SpineMid); // Refer to CBodyBasics::SaveBody for Joint pairs
				GuiApp[HIPSANGLE_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineBase);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, wParam, lParam));
				break;
			case ID_WRISTS_ACCELLERATION:
				GuiApp[OPTIFIT_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[WRISTSACCELL_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
			//	GuiApp[WRISTSACCELL_PNUM].setJointTypes(JointType_WristRight, JointType_WristRight);
				GuiApp[WRISTSACCELL_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, wParam, lParam));
				break;
			case ID_WRISTS_VELOCITY:
				GuiApp[OPTIFIT_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[WRISTSVELO_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
			//	GuiApp[WRISTSVELO_PNUM].setJointTypes(JointType_WristRight, JointType_WristRight);
				GuiApp[WRISTSVELO_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, wParam, lParam));
				break;
			case ID_OPTIFIT_RUN: // Runs everything we need
				KinectThread.push_back(std::thread(StartKinect, hWnd, message, wParam, lParam));
				WirelessThread.push_back(std::thread(StartWireless, hWnd, message, wParam, lParam));
				//WirelessThread[WIRELESS_PNUM].

			
				
				GuiApp[EMG_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[EMG_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
				GuiApp[EMG_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, ID_EMG_VALUE, lParam));

				GuiApp[FORCE_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[FORCE_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
				GuiApp[FORCE_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, ID_FORCE_MOMENT, lParam));

				GuiApp[OPTIFIT_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[OPTIFIT_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
				GuiApp[OPTIFIT_PNUM].setEMGGUIClass(&GuiApp[EMG_PNUM]);
				GuiApp[OPTIFIT_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, wParam, lParam));
				break;
			case ID_FORCE_MOMENT:
				//WirelessThread[WIRELESS_PNUM].
				GuiApp[FORCE_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[FORCE_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
				GuiApp[FORCE_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, wParam, lParam));
				break;
			case ID_EMG_VALUE:
				//WirelessThread[WIRELESS_PNUM].
				GuiApp[EMG_PNUM].setWirelessClass((void*)&WirelessApp);
				GuiApp[EMG_PNUM].setOptiBodyClass(kinectApp.GetUserBody());
				GuiApp[EMG_PNUM].setJointTypes(JointType_SpineShoulder, JointType_SpineShoulder);
				GUIThread.push_back(std::thread(StartGUI, hWnd, message, wParam, lParam));
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		while (!KinectThread.empty()) { KinectThread.back().join(); KinectThread.pop_back(); }
		while (!GUIThread.empty()) { GUIThread.back().join(); GUIThread.pop_back(); }
		
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//GetDlgItem(hDlg, IDC_);
	
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
	
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
typedef struct KinectVars {
	HINSTANCE hInst;
	int nCmdShow = 48;

} Vars, *pVars;
void StartKinect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE NEWhInstance = (HINSTANCE)GetModuleHandle(NULL);
	KinectInst = NEWhInstance; // save incase we need it later
	//kinectApp.ParentWindow(hDlg);
	kinectApp.Run(hInst, Show, hDlg);

};
void StartGUI(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam); // get the message from the menu call so we know what kind of graph to make

	HINSTANCE NEWhInstance = (HINSTANCE)GetModuleHandle(NULL);
	GUIApp* NewGUI = new GUIApp;
	//GUIInst = NEWhInstance; // save incase we need it later
	//int wmId = LOWORD(wParam);
	// Parse the menu selections:
	switch (wmId)
	{
	case ID_HIPS_ANGLE:
		GuiApp[HIPSANGLE_PNUM].Datatype = 003; // For getData() call to OptiBody
		GuiApp[HIPSANGLE_PNUM].RunSpine(hInst, Show, hDlg, wmId);
		
		//GuiAppPtr[HIPSANGLE_PNUM] = NewGUI;

		ToClose[HIPSANGLE_PNUM] = std::this_thread::get_id();
		break;
	case ID_WRISTS_ACCELLERATION:
		GuiApp[WRISTSACCELL_PNUM].Datatype = 200; // =121
		GuiApp[WRISTSACCELL_PNUM].Run(hInst, Show, hDlg, wmId);
				
		ToClose[WRISTSACCELL_PNUM] = std::this_thread::get_id();
	case ID_WRISTS_VELOCITY:
		GuiApp[WRISTSVELO_PNUM].Datatype = 201; //=111
		GuiApp[WRISTSVELO_PNUM].Run(hInst, Show, hDlg, wmId);

		ToClose[WRISTSVELO_PNUM] = std::this_thread::get_id();
	
	case ID_OPTIFIT_RUN:
		GuiApp[OPTIFIT_PNUM].Datatype = 000;
		GuiApp[OPTIFIT_PNUM].RunSpine(hInst, Show, hDlg, wmId);
		ToClose[OPTIFIT_PNUM] = std::this_thread::get_id();
		break;

	case ID_FORCE_MOMENT:
		GuiApp[FORCE_PNUM].Datatype = 000;
		GuiApp[FORCE_PNUM].RunForce(hInst, Show, hDlg, wmId);
		ToClose[FORCE_PNUM] = std::this_thread::get_id();
		break;

	case ID_EMG_VALUE:
		GuiApp[EMG_PNUM].Datatype = 0000;
		GuiApp[EMG_PNUM].RunEMG(hInst, Show, hDlg, wmId);
		ToClose[EMG_PNUM] = std::this_thread::get_id();
		break;


}
		

	//GuiApp[0].Run(hInst, Show, hDlg, wmId);
	
	// this is where you save the ID to tell Build to remove the thread
	//ToClose[] = std::this_thread::get_id());
	
}
void StartWireless(HWND H, UINT I, WPARAM W, LPARAM L)
{
	HINSTANCE NEWhInstance = (HINSTANCE)GetModuleHandle(NULL);
	WirelessInst = NEWhInstance; // save incase we need it later
							   //kinectApp.ParentWindow(hDlg);
	WirelessApp.RunWireless();

	ToClose[WIRELESS_PNUM] = std::this_thread::get_id();
}
;
void CloseThread(int PNUM)
{
	// using std::find with vector and iterator:
	//std::vector<int> myvector(myints, myints + 4);
	//std::vector<std::thread::id>::iterator it;

	while (!GUIThread.empty() || !KinectThread.empty())
	{
		switch (PNUM) {
		case HIPSANGLE_PNUM:
			GUIThread[HIPSANGLE_PNUM].join();
			GUIThread.erase(GUIThread.begin() + HIPSANGLE_PNUM);
			break;
		case WRISTSACCELL_PNUM:
			GUIThread[WRISTSACCELL_PNUM].join();
			GUIThread.erase(GUIThread.begin() + WRISTSACCELL_PNUM);
			break;
		case WRISTSVELO_PNUM:
			GUIThread[WRISTSVELO_PNUM].join();
			GUIThread.erase(GUIThread.begin() + WRISTSVELO_PNUM);
			break;
		case OPTIFIT_PNUM:
			GUIThread[OPTIFIT_PNUM].join();
			GUIThread.erase(GUIThread.begin() + OPTIFIT_PNUM);
			break;
		case FORCE_PNUM:
			GUIThread[FORCE_PNUM].join();
			GUIThread.erase(GUIThread.begin() + FORCE_PNUM);
			break;
		case EMG_PNUM:
			GUIThread[EMG_PNUM].join();
			GUIThread.erase(GUIThread.begin() + EMG_PNUM);
			break;
		}
				//GUIThread[i].join();
				//ToClose.pop_back();
				//GUIThread.erase(GUIThread.begin() + i);
	}
}

;
void ChecktoCloseThreads(void)
{
	// using std::find with vector and iterator:
	//std::vector<int> myvector(myints, myints + 4);
	/*
	std::vector<std::thread::id>::iterator it;	
	while (!GUIThread.empty() || !KinectThread.empty())
	{
		for (int i = 0; i < GUIThread.size(); i++) 
		{
			for (int j = 0; i < ToClose.size(); i++)
			{
				if (GUIThread[i].get_id() == ToClose[j])

				{
					GUIThread[i].join();
					//ToClose.pop_back();
					GUIThread.erase(GUIThread.begin() + i);

				}
			}
		}
		for (int i = 0; i < KinectThread.size(); i++) 
		{
			if (KinectThread[i].get_id() == ToClose.back()) 
			{
				KinectThread[i].join();
				//ToClose.pop_back();
				KinectThread.erase(GUIThread.begin() + i);
			}
		}	
	}
	*/
}
;

//ATOM RegisterKinectClass(HINSTANCE hInstance)
//{
//	WNDCLASSEXW wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = WndProc;// points to window procedure
//	wcex.cbClsExtra = 0;
//	wcex.cbWndExtra = 0;
//	wcex.hInstance = hInstance;
//	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUILD00)); //*** Commented out Build 0.0 Icon to try to fix error "CVTRES : fatal error CVT1100: duplicate resource.  type:ICON, name:1, language:0x0409"
//	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BUILD00);
//	wcex.lpszClassName = szWindowClass;
//	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));//***
//
//	return RegisterClassExW(&wcex);// Register the window class.
//}