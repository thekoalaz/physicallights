//------------------------------------------------------------------------------
// <copyright file="InfraredBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include "NuiApi.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>

#include "PhysicalLightClient.h"

class PhysicalLightKinect
{
    static const int        ImageWidth  = 640;
    static const int        ImageHeight = 480;

    static const int        X_FOV = 57;
    static const int        Y_FOV = 43;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    PhysicalLightKinect();

    /// <summary>
    /// Destructor
    /// </summary>
    ~PhysicalLightKinect();

    int Run();


private:
    HWND                    m_hWnd;

    bool                    m_bSaveInfrared;
    bool                    m_bSaveDepth;

    bool                    m_firstCalibration;
    bool                    m_reCalibrate;

    // Current Kinect
    INuiSensor*             m_pNuiSensor;
    
    HANDLE                  m_pColorStreamHandle;
    HANDLE                  m_hNextColorFrameEvent;

    HANDLE                  m_pDepthStreamHandle;
    HANDLE                  m_hNextDepthFrameEvent;

    RGBQUAD*                m_pTempColorBuffer;

    IplImage* infrared;
    IplImage* depth;

    PhysicalLightClient*    client;

    std::vector<double>     light1Calibration;
    std::vector<double>     light2Calibration;

    void                    Update();
    void                    Calibrate();
    void                    Parse_Calibrate();
    void                    Parse_Line(std::string line, std::vector<double> & calibrate);

    void                    Transform(std::vector<double> & coords);

    void                    Start();
    void                    Stop();

    int                     drawInfrared();
    int                     drawDepth();
};
