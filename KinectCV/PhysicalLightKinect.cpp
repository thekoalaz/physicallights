#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <strsafe.h>

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

#include "PhysicalLightKinect.h"

PhysicalLightKinect::PhysicalLightKinect() :
    m_hNextColorFrameEvent(INVALID_HANDLE_VALUE),
    m_pColorStreamHandle(INVALID_HANDLE_VALUE),
    m_pNuiSensor(NULL),
    m_hNextDepthFrameEvent(INVALID_HANDLE_VALUE),
    m_pDepthStreamHandle(INVALID_HANDLE_VALUE),
    m_pTempColorBuffer(NULL),
    m_bSaveInfrared(false),
    m_bSaveDepth(false),
    m_firstCalibration(true),
    m_reCalibrate(false),
    client(PhysicalLightClient::Instance())
{
}

PhysicalLightKinect::~PhysicalLightKinect()
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
}

int PhysicalLightKinect::Run()
{
    INuiSensor * pNuiSensor;
    HRESULT hr = S_OK;

    infrared = cvCreateImageHeader(cvSize(ImageWidth, ImageHeight),IPL_DEPTH_8U,1);
    depth = cvCreateImageHeader(cvSize(ImageWidth, ImageHeight),IPL_DEPTH_8U,1);

    //hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR);
    //if (hr != S_OK)
    //{
    //    std::cout << "NuiInitialize failed" << std::endl;
    //    return hr;
    //}

    //m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    //hr = NuiImageStreamOpen(
    //    NUI_IMAGE_TYPE_COLOR_INFRARED,
    //    NUI_IMAGE_RESOLUTION_640x480,
    //    0,
    //    2,
    //    m_hNextColorFrameEvent,
    //    &m_pColorStreamHandle);
    //if (FAILED(hr))
    //{
    //    std::cout << "Could not open infrared stream video" << std::endl;
    //    return hr;
    ////}

    ////m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    ////hr = NuiImageStreamOpen(
    ////    NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
    ////    NUI_IMAGE_RESOLUTION_640x480,
    ////    0,
    ////    2,
    ////    m_hNextDepthFrameEvent,
    ////    &m_pDepthStreamHandle);
    ////if (FAILED(hr))
    ////{
    ////    std::cout << "Could not open depth stream video" << std::endl;
    //    return hr;
    //}

    cv::namedWindow("Command Window", CV_WINDOW_AUTOSIZE);
    bool calibrate = false;
    while (1)
    {
        if (calibrate)
        {
            Update();
            Sleep(8000);
        }

        int c = cvWaitKey(1);
        if (c == 'n')
        {
            m_reCalibrate = true;
        }
        if (c == 's')
        {
            m_bSaveInfrared = true;
            m_bSaveDepth = true;
        }
        if (c == 'C')
        {
            if (calibrate) calibrate = false;
            else calibrate = true;
        }
        if (c == 'c')
        {
            Update();
        }
        //exit
        if (c == 27 || c == 'q' || c == 'Q')
            break;
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
        std::cout << "No ready Kinect found!" << std::endl;
        return E_FAIL;
    }

    cvReleaseImageHeader(&infrared);
    cvReleaseImageHeader(&depth);
    NuiShutdown();
    return 0;
}

void PhysicalLightKinect::Update()
{
    Calibrate();
    Transform(light1Calibration);
    Transform(light2Calibration);
    client->translate(PhysicalLightClient::KEY_LIGHT, light1Calibration[0], light1Calibration[1], light1Calibration[2]);
    client->translate(PhysicalLightClient::FILL_LIGHT, light2Calibration[0], light2Calibration[1], light2Calibration[2]);
    if (m_firstCalibration || m_reCalibrate)
    {
        client->set_center(
            (light1Calibration[0] + light2Calibration[0]) / 2,
            (light1Calibration[1] + light2Calibration[1]) / 2,
            (light1Calibration[2] + light2Calibration[2]) / 2);
        client->translate(PhysicalLightClient::CHAR,
            (light1Calibration[0] + light2Calibration[0]) / 2,
            (light1Calibration[1] + light2Calibration[1]) / 2 + 600,
            (light1Calibration[2] + light2Calibration[2]) / 2);
        client->translate(PhysicalLightClient::GROUND,
            (light1Calibration[0] + light2Calibration[0]) / 2,
            (light1Calibration[1] + light2Calibration[1]) / 2-170,
            (light1Calibration[2] + light2Calibration[2]) / 2);
        client->translate(PhysicalLightClient::CAMERA,
            (light1Calibration[0] + light2Calibration[0]) / 2-250,
            (light1Calibration[1] + light2Calibration[1]) / 2-30,
            (light1Calibration[2] + light2Calibration[2]) / 2+100);

        m_firstCalibration = false;
        m_reCalibrate = false;
    }
    ////std::cout<<"Wait for Infrared"<<std::endl;
    //if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextColorFrameEvent, 0))
    //{
    ////    std::cout << "Draw Infrared" << std::endl;
    //    drawInfrared();
    //}
    ////std::cout<<"Wait for Depth"<<std::endl;
    //if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextDepthFrameEvent, 0))
    //{
    ////    std::cout << "Draw Depth" << std::endl;
    //    drawDepth();
    //}
}
int PhysicalLightKinect::drawInfrared()
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;
    HRESULT hr = NuiImageStreamGetNextFrame( m_pColorStreamHandle, 0, &pImageFrame );
    if( FAILED( hr ) )
    {
        std::cout<<"Get Infrared Frame Failed"<<std::endl;
        return -1;
    }

    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;
        cvSetData(infrared,pBuffer,LockedRect.Pitch);
    }
    cvShowImage("Infrared", infrared);
    if (m_bSaveInfrared)
    {
        cvSaveImage("infrared.bmp", infrared);
        m_bSaveInfrared = false;
    }
    NuiImageStreamReleaseFrame( m_pColorStreamHandle, pImageFrame );
    return 0;
}

int PhysicalLightKinect::drawDepth()
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;
    HRESULT hr = NuiImageStreamGetNextFrame( m_pDepthStreamHandle, 0, &pImageFrame );
    if( FAILED( hr ) )
    {
        std::cout<<"Get Depth Frame Failed"<<std::endl;
        return -1;
    }

    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE buf[ImageWidth*ImageHeight*3];
        USHORT * pBuff = (USHORT*) LockedRect.pBits;
        const int CHANNEL = 3;
        for(int i=0;i<ImageWidth*ImageHeight;i++)
        {
            BYTE index = pBuff[i]&0x07;
            USHORT realDepth = (pBuff[i]&0xFFF8)>>3;
            BYTE scale = 255 - (BYTE)(256*realDepth/0x0fff);
            buf[CHANNEL*i] = buf[CHANNEL*i+1] = buf[CHANNEL*i+2] = 0;
            switch( index )
            {
            case 0:
                buf[CHANNEL*i]=scale/2;
                buf[CHANNEL*i+1]=scale/2;
                buf[CHANNEL*i+2]=scale/2;
                break;
            case 1:
                buf[CHANNEL*i]=scale;
                break;
            case 2:
                buf[CHANNEL*i+1]=scale;
                break;
            case 3:
                buf[CHANNEL*i+2]=scale;
                break;
            case 4:
                buf[CHANNEL*i]=scale;
                buf[CHANNEL*i+1]=scale;
                break;
            case 5:
                buf[CHANNEL*i]=scale;
                buf[CHANNEL*i+2]=scale;
                break;
            case 6:
                buf[CHANNEL*i+1]=scale;
                buf[CHANNEL*i+2]=scale;
                break;
            case 7:
                buf[CHANNEL*i]=255-scale/2;
                buf[CHANNEL*i+1]=255-scale/2;
                buf[CHANNEL*i+2]=255-scale/2;
                break;
            }
        }
        cvSetData(depth,buf,ImageWidth*3);
    }
    cvShowImage("Depth", depth);
    if (m_bSaveDepth)
    {
        cvSaveImage("depth.bmp", depth);
        m_bSaveDepth = true;
    }
    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
    return 0;
}

void PhysicalLightKinect::Calibrate()
{
    std::cout << "Calibrate" << std::endl;
    if (m_firstCalibration || m_reCalibrate)
    {
        system("START /WAIT matlab -r calibrate() -logfile calibrate.log -nosplash -nodesktop");
        Sleep(20000);
    }
    else
    {
        system("START /WAIT matlab -r updateCoordinates() -logfile calibrate.log -nosplash -nodesktop -minimize");
    }
    Sleep(5000);
    Parse_Calibrate();

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

    std::cout << result.str() << std::endl;
}

void PhysicalLightKinect::Parse_Calibrate()
{
    std::string fileName = ".\\data\\calibrate.txt";
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
            if (m_firstCalibration)
                light1Calibration.push_back(line);
            else
                light1Calibration[index] = line;
            index++;
        }
        else
        {
            if (m_firstCalibration)
                light2Calibration.push_back(line);
            else
                light2Calibration[index-divide] = line;
            index++;
        }
    }

    fin.close();
}

void PhysicalLightKinect::Transform(std::vector<double> & coords)
{
    double focal_x = (double)ImageWidth / (2 * tan(X_FOV * M_PI / 180.0));
    double focal_y = (double)ImageHeight / (2 * tan(Y_FOV * M_PI / 180.0));

    coords[0] = coords[2] * coords[0] / focal_x;
    coords[1] = coords[2] * coords[1] / focal_y;
}

void PhysicalLightKinect::Start()
{
    std::cout << "Start";
}

void PhysicalLightKinect::Stop()
{
    std::cout << "Stop";
}
