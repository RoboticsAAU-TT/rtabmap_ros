/*
Copyright (c) 2010-2014, Mathieu Labbe - IntRoLab - Universite de Sherbrooke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Universite de Sherbrooke nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "rtabmap/core/RtabmapExp.h" // DLL export/import defines

#include <opencv2/highgui/highgui.hpp>
#include "rtabmap/core/Camera.h"
#include "rtabmap/utilite/UTimer.h"
#include <set>
#include <stack>
#include <list>
#include <vector>

class UDirectory;

namespace rtabmap
{

/////////////////////////
// CameraImages
/////////////////////////
class RTABMAP_EXP CameraImages :
	public Camera
{
public:
	CameraImages();
	CameraImages(
			const std::string & path,
			float imageRate = 0,
			const Transform & localTransform = Transform::getIdentity());
	virtual ~CameraImages();

	virtual bool init(const std::string & calibrationFolder = ".", const std::string & cameraName = "");
	virtual bool isCalibrated() const;
	virtual std::string getSerial() const;
	std::string getPath() const {return _path;}
	unsigned int imagesCount() const;
	std::vector<std::string> filenames() const;
	bool isImagesRectified() const {return _rectifyImages;}
	int getBayerMode() const {return _bayerMode;}
	const CameraModel & cameraModel() const {return _model;}

	void setPath(const std::string & dir) {_path=dir;}
	void setStartIndex(int index) {_startAt = index;} // negative means last
	void setDirRefreshed(bool enabled) {_refreshDir = enabled;}
	void setImagesRectified(bool enabled) {_rectifyImages = enabled;}
	void setBayerMode(int mode) {_bayerMode = mode;} // -1=disabled (default) 0=BayerBG, 1=BayerGB, 2=BayerRG, 3=BayerGR

	void setTimestamps(bool fileNamesAreStamps, const std::string & filePath = "", bool syncImageRateWithStamps=true)
	{
		_filenamesAreTimestamps = fileNamesAreStamps;
		timestampsPath_=filePath;
		syncImageRateWithStamps_ = syncImageRateWithStamps;
	}

	void setScanPath(
			const std::string & dir,
			int maxScanPts = 0,
			int downsampleStep = 1,
			float voxelSize = 0.0f,
			int normalsK = 0, // compute normals if > 0
			const Transform & localTransform=Transform::getIdentity())
	{
		_scanPath = dir;
		_scanLocalTransform = localTransform;
		_scanMaxPts = maxScanPts;
		_scanDownsampleStep = downsampleStep;
		_scanNormalsK = normalsK;
		_scanVoxelSize = voxelSize;
		if(_scanDownsampleStep>1)
		{
			_scanMaxPts /= _scanDownsampleStep;
		}
	}

	void setDepthFromScan(bool enabled, bool fillHolesVertical = true, bool fillHolesFromBorder = false)
	{
		_depthFromScan = enabled;
		_depthFromScanFillHolesVertical = fillHolesVertical;
		_depthFromScanFillHolesFromBorder = fillHolesFromBorder;
	}

	void setGroundTruthPath(const std::string & filePath, int format = 0)
	{
		groundTruthPath_ = filePath;
		_groundTruthFormat = format;
	}

	void setDepth(bool isDepth, float depthScaleFactor = 1.0f)
	{
		_isDepth = isDepth;
		_depthScaleFactor=depthScaleFactor;
	}

protected:
	virtual SensorData captureImage();

private:
	std::string _path;
	int _startAt;
	// If the list of files in the directory is refreshed
	// on each call of takeImage()
	bool _refreshDir;
	bool _rectifyImages;
	int _bayerMode;
	bool _isDepth;
	float _depthScaleFactor;
	int _count;
	UDirectory * _dir;
	std::string _lastFileName;

	int _countScan;
	UDirectory * _scanDir;
	std::string _lastScanFileName;
	std::string _scanPath;
	Transform _scanLocalTransform;
	int _scanMaxPts;
	int _scanDownsampleStep;
	float _scanVoxelSize;
	int _scanNormalsK;

	bool _depthFromScan;
	bool _depthFromScanFillHolesVertical;
	bool _depthFromScanFillHolesFromBorder;

	bool _filenamesAreTimestamps;
	std::string timestampsPath_;
	bool syncImageRateWithStamps_;

	std::string groundTruthPath_;
	int _groundTruthFormat;

	std::list<double> stamps_;
	std::list<Transform> groundTruth_;
	CameraModel _model;

	UTimer _captureTimer;
	double _captureDelay;
};




/////////////////////////
// CameraVideo
/////////////////////////
class RTABMAP_EXP CameraVideo :
	public Camera
{
public:
	enum Source{kVideoFile, kUsbDevice};

public:
	CameraVideo(int usbDevice = 0,
			float imageRate = 0,
			const Transform & localTransform = Transform::getIdentity());
	CameraVideo(const std::string & filePath,
			bool rectifyImages = false,
			float imageRate = 0,
			const Transform & localTransform = Transform::getIdentity());
	virtual ~CameraVideo();

	virtual bool init(const std::string & calibrationFolder = ".", const std::string & cameraName = "");
	virtual bool isCalibrated() const;
	virtual std::string getSerial() const;
	int getUsbDevice() const {return _usbDevice;}
	const std::string & getFilePath() const {return _filePath;}

protected:
	virtual SensorData captureImage();

private:
	// File type
	std::string _filePath;
	bool _rectifyImages;

	cv::VideoCapture _capture;
	Source _src;

	// Usb camera
	int _usbDevice;
	std::string _guid;

	CameraModel _model;
};


} // namespace rtabmap