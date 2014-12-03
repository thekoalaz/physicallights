//------------------------------------------------------------------------------
// <copyright file="InfraredBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstringt.h>
#include <atlstr.h>
#include <strsafe.h>

#include <opencv2/opencv.hpp>

#include "PhysicalLightKinect.h"
#include "resource.h"

/// <summary>
/// Constructor
/// </summary>
CInfraredBasics::CInfraredBasics() :
    m_pD2DFactory(NULL),
    m_hNextColorFrameEvent(INVALID_HANDLE_VALUE),
    m_pColorStreamHandle(INVALID_HANDLE_VALUE),
    m_pNuiSensor(NULL),
    m_hNextDepthFrameEvent(INVALID_HANDLE_VALUE),
    m_pDepthStreamHandle(INVALID_HANDLE_VALUE),
    m_pTempColorBuffer(NULL),
    m_bSaveScreenshot(false)
{
}

/// <summary>
/// Destructor
/// </summary>
CInfraredBasics::~CInfraredBasics()
{
    if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextColorFrameEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hNextColorFrameEvent);
    }

    if (m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hNextDepthFrameEvent);
    }


    delete [] m_pTempColorBuffer;
    m_pTempColorBuffer = NULL;

    // clean up Direct2D
    SafeRelease(m_pD2DFactory);
    SafeRelease(m_pNuiSensor);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CInfraredBasics::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"InfraredBasicsAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CInfraredBasics::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    const int eventCount = 1;
    HANDLE hEvents[eventCount];

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        hEvents[0] = m_hNextColorFrameEvent;

        // Check to see if we have either a message (by passing in QS_ALLINPUT)
        // Or a Kinect event (hEvents)
        // Update() will check for Kinect events individually, in case more than one are signalled
        MsgWaitForMultipleObjects(eventCount, hEvents, FALSE, INFINITE, QS_ALLINPUT);

        // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
        // can return for other reasons even though it is signaled.
        Update();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

/// <summary>
/// Main processing function
/// </summary>
void CInfraredBasics::Update()
{
    if (NULL == m_pNuiSensor)
    {
        return;
    }

    if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextColorFrameEvent, 0) )
    {
        ProcessColor();
    }
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CInfraredBasics::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CInfraredBasics* pThis = NULL;
    
    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CInfraredBasics*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CInfraredBasics*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CInfraredBasics::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

            // Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
            // We'll use this to draw the data we receive from the Kinect to the screen
            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D draw device.");
            }

            // Look for a connected Kinect, and create it if found
            CreateFirstConnected();
        }
        break;

        case WM_COMMAND:
        {
            switch (HIWORD(wParam))
            {
                case BN_CLICKED:
                {
                    switch (LOWORD(wParam))
                    {
                        case IDC_BUTTON_Screenshot:
                            m_bSaveScreenshot = true;
                            break;
                        case IDC_BUTTON_Calibrate:
                            Calibrate();
                            break;
                        case IDC_BUTTON_START:
                            Start();
                            break;
                        case IDC_BUTTON_STOP:
                            Stop();
                            break;
                    }
                }
                break;
            }
        }
        break;

        // If the titlebar X is clicked, destroy app
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            // Quit the main message pump
            PostQuitMessage(0);
            break;
    }

    return FALSE;
}

/// <summary>
/// Create the first connected Kinect found 
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CInfraredBasics::CreateFirstConnected()
{
    INuiSensor * pNuiSensor;
    HRESULT hr;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using color
        //hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR); 
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH|NUI_INITIALIZE_FLAG_USES_COLOR); 
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when color data is available
            m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            // Open a color image stream to receive color frames
            hr = m_pNuiSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_COLOR_INFRARED,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,
                m_hNextColorFrameEvent,
                &m_pColorStreamHandle);
            if (NULL == m_pNuiSensor || FAILED(hr))
            {
                SetStatusMessage(L"Could not open infrared stream");
                return E_FAIL;
            }

            m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            hr = m_pNuiSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,
                m_hNextDepthFrameEvent,
                &m_pDepthStreamHandle);
            if( NULL == m_pNuiSensor || FAILED( hr ) )
            {
                SetStatusMessage(L"Could not open depth stream");
                return E_FAIL;
            }
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
        SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

/// <summary>
/// Get the name of the file where screenshot will be stored.
/// </summary>
/// <param name="screenshotName">
/// [out] String buffer that will receive screenshot file name.
/// </param>
/// <param name="screenshotNameSize">
/// [in] Number of characters in screenshotName string buffer.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT GetScreenshotFileName(wchar_t *screenshotName, wchar_t *streamName, UINT screenshotNameSize)
{
    wchar_t *knownPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &knownPath);

    if (SUCCEEDED(hr))
    {
        // File name will be CalibrateInfrared.bmp
        StringCchPrintfW(screenshotName, screenshotNameSize, L".\\Calibrate%s.bmp", streamName);
    }

    CoTaskMemFree(knownPath);
    return hr;
}

/// <summary>
/// Save passed in image data to disk as a bitmap
/// </summary>
/// <param name="pBitmapBits">image data to save</param>
/// <param name="lWidth">width (in pixels) of input image data</param>
/// <param name="lHeight">height (in pixels) of input image data</param>
/// <param name="wBitsPerPixel">bits per pixel of image data</param>
/// <param name="lpszFilePath">full file path to output bitmap to</param>
/// <returns>indicates success or failure</returns>
HRESULT CInfraredBasics::SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCWSTR lpszFilePath)
{
    DWORD dwByteCount = lWidth * lHeight * (wBitsPerPixel / 8);

    BITMAPINFOHEADER bmpInfoHeader = {0};

    bmpInfoHeader.biSize        = sizeof(BITMAPINFOHEADER);  // Size of the header
    bmpInfoHeader.biBitCount    = wBitsPerPixel;             // Bit count
    bmpInfoHeader.biCompression = BI_RGB;                    // Standard RGB, no compression
    bmpInfoHeader.biWidth       = lWidth;                    // Width in pixels
    bmpInfoHeader.biHeight      = -lHeight;                  // Height in pixels, negative indicates it's stored right-side-up
    bmpInfoHeader.biPlanes      = 1;                         // Default
    bmpInfoHeader.biSizeImage   = dwByteCount;               // Image size in bytes

    BITMAPFILEHEADER bfh = {0};

    bfh.bfType    = 0x4D42;                                           // 'M''B', indicates bitmap
    bfh.bfOffBits = bmpInfoHeader.biSize + sizeof(BITMAPFILEHEADER);  // Offset to the start of pixel data
    bfh.bfSize    = bfh.bfOffBits + bmpInfoHeader.biSizeImage;        // Size of image + headers

    // Create the file on disk to write to
    HANDLE hFile = CreateFileW(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Return if error opening file
    if (NULL == hFile) 
    {
        return E_ACCESSDENIED;
    }

    DWORD dwBytesWritten = 0;
    
    // Write the bitmap file header
    if ( !WriteFile(hFile, &bfh, sizeof(bfh), &dwBytesWritten, NULL) )
    {
        CloseHandle(hFile);
        return E_FAIL;
    }
    
    // Write the bitmap info header
    if ( !WriteFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwBytesWritten, NULL) )
    {
        CloseHandle(hFile);
        return E_FAIL;
    }
    
    // Write the RGB Data
    if ( !WriteFile(hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwBytesWritten, NULL) )
    {
        CloseHandle(hFile);
        return E_FAIL;
    }    

    // Close the file
    CloseHandle(hFile);
    return S_OK;
}

/// <summary>
/// Handle new color data
/// </summary>
/// <returns>indicates success or failure</returns>
void CInfraredBasics::ProcessColor()
{
    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the infrared frame
    hr = m_pNuiSensor->NuiImageStreamGetNextFrame(m_pColorStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return;
    }

    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we've received valid data
    if (LockedRect.Pitch != 0)
    {
        if (m_pTempColorBuffer == NULL)
        {
            m_pTempColorBuffer = new RGBQUAD[cColorWidth * cColorHeight];
        }

        for (int i = 0; i < cColorWidth * cColorHeight; ++i)
        {
            BYTE intensity = reinterpret_cast<USHORT*>(LockedRect.pBits)[i] >> 8;

            RGBQUAD *pQuad = &m_pTempColorBuffer[i];
            pQuad->rgbBlue = intensity;
            pQuad->rgbGreen = intensity;
            pQuad->rgbRed = intensity;
            pQuad->rgbReserved = 255;
        }

        // Draw the data with Direct2D

        // If the user pressed the screenshot button, save a screenshot
        if (m_bSaveScreenshot)
        {
            WCHAR statusMessage[cStatusMessageMaxLen];

            // Retrieve the path to My Photos
            WCHAR screenshotPath[MAX_PATH];
            GetScreenshotFileName(screenshotPath, L"Infrared", _countof(screenshotPath));

            std::vector<int> compression_params;
            compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(9);

            const NUI_IMAGE_FRAME * pImageFrame = NULL;
            //HRESULT hr = NuiImageStreamGetNextFrame( m_hNextDepthFrameEvent, 0, &pImageFrame );
            //INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
            //NUI_LOCKED_RECT LockedRect;
            //pTexture->LockRect( 0, &LockedRect, NULL, 0 );

            cv::Mat img;
            img.create(cColorHeight, cColorWidth, CV_8UC3);
            //cvSetData(&img, static_cast<BYTE *>(LockedRect.pBits), cColorWidth);
            for (int i=0; i<img.rows; i++)
            {  
                uchar *ptr = img.ptr<uchar>(i);
                  
                //?uchar  
                uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
                for (int j=0; j<img.cols; j++)
                {
                    ptr[3*j] = pBuffer[4*j];
                    ptr[3*j+1] = pBuffer[4*j+1];
                    ptr[3*j+2] = pBuffer[4*j+2];
                }
            }
            img = cv::imread("C:\Users\Kevin\OneDrive\Pictures\Private\Cocktail_Party_Resize.png");
            //cvSetData(img, static_cast<BYTE *>(LockedRect.pBits), img->widthStep);
            cv::imwrite("Calibrate_Infrared.bmp", img);
            //// Write out the bitmap to disk
            //hr = SaveBitmapToFile(static_cast<BYTE *>(LockedRect.pBits), cColorWidth, cColorHeight, 32, screenshotPath);

            //if (SUCCEEDED(hr))
            //{
            //    // Set the status bar to show where the screenshot was saved
            //    StringCchPrintf( statusMessage, cStatusMessageMaxLen, L"Screenshot saved to %s", screenshotPath);
            //}
            //else
            //{
            //    StringCchPrintf( statusMessage, cStatusMessageMaxLen, L"Failed to write screenshot to %s", screenshotPath);
            //}

            //SetStatusMessage(statusMessage);

            // toggle off so we don't save a screenshot again next frame
            m_bSaveScreenshot = false;
        }
    }

    // We're done with the texture so unlock it
    pTexture->UnlockRect(0);

    // Release the frame
    m_pNuiSensor->NuiImageStreamReleaseFrame(m_pColorStreamHandle, &imageFrame);
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void CInfraredBasics::SetStatusMessage(const WCHAR * szMessage)
{
    SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
}

void CInfraredBasics::Calibrate()
{
    SetStatusMessage(L"Calibrate");
    system("matlab -r calibrate('', '') -logfile calibrate.log -nosplash -nodesktop -minimize");
    Sleep(2000);
    Parse_Calibrate();

    /* Test Result */
    std::ostringstream result;
    result << "(";
    for (auto value : light1Calibration)
    {
        result << " " << value;
    }
    result << ") ";
    
    result << "( ";
    for (auto value : light2Calibration)
    {
        result << " " << value;
    }
    result << ") ";

    std::string result_str = result.str();
    std::wstring w_result = std::wstring(result_str.begin(), result_str.end()).c_str();
    SetStatusMessage(w_result.c_str());

}

void CInfraredBasics::Parse_Calibrate()
{
    std::string fileName = "calibrate.txt";
    std::ifstream fin(fileName);
    if( !fin  ) {
        std::cerr << "Can't open file " << fileName << std::endl;
        std::exit( -1 );
    }


    // initialize the vector from the values in the file:
    std::vector<double> lines{ std::istream_iterator<double>(fin),
                               std::istream_iterator<double>() };

    int divide = 3;
    int index = 0;
    for (auto &line : lines)
    {
        if (index < divide)
        {
            light1Calibration.push_back(line);
            index++;
        }
        else
        {
            light2Calibration.push_back(line);
        }
    }

    fin.close();
}


void CInfraredBasics::Start()
{
    SetStatusMessage(L"Start");
}

void CInfraredBasics::Stop()
{
    SetStatusMessage(L"Stop");
}
