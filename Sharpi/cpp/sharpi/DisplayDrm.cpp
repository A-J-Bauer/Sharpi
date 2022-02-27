#include "DisplayDrm.h"


string DisplayDrm::description = 
"-----------------------------------------\n"
"Direct Rendering Manager DRM Display     \n"
"                                         \n"
"config:                                  \n"
" nothing to do                           \n"
"                                         \n"
"wiring:                                  \n"
" connect a monitor                       \n"
"                                         \n"
"not working if a window manager claimed  \n"
"the display already.                     \n"
"-----------------------------------------\n";

string DisplayDrm::GetDescription()
{	    
	return description;
};

const char* DisplayDrm::GetDescriptionC()
{
	return description.c_str();
};

string DisplayDrm::ListResources() // -> drm_mode.h
{    
    string resources = "";
    string path = "";
    string filename = "";

    filesystem::path devdri{ "/dev/dri" };

    for (auto const& dir_entry : filesystem::directory_iterator{ devdri })
    {
        drmModeResPtr res;

        if (dir_entry.is_character_file())
        {
            path = dir_entry.path();
            filename = dir_entry.path().filename();
            
            resources += "path: " + path;

            int fd = open(path.c_str(), O_RDWR | O_CLOEXEC);
            if (fd < 0)
            {
                resources += "\ncould not open " + path;
            }
            else            
            {
                res = drmModeGetResources(fd);
                if (res)
                {                    
                    for (int i = 0; i < res->count_connectors; i++)
                    {                       
                        resources += "\nconnector: " + to_string(res->connectors[i]);

                        drmModeConnectorPtr connector = drmModeGetConnector(fd, res->connectors[i]);
                        if (connector)
                        {
                            resources += " name: " + to_string(connector->connector_type) + "-" + to_string(connector->connector_type_id);
                            resources += " encoder: " + to_string(connector->encoder_id);

                            drmModeEncoderPtr encoder = drmModeGetEncoder(fd, connector->encoder_id);
                            if (encoder)
                            {
                                resources += " crtc: " + to_string(encoder->crtc_id);
                                drmModeFreeEncoder(encoder);
                            }

                            resources += "\nresolutions:";
                            drmModeModeInfoPtr mode = 0;
                            for (int i = 0; i < connector->count_modes; i++)
                            {
                                mode = &connector->modes[i];                    
                                double freq = mode->clock * 1000.0f / (mode->htotal * mode->vtotal);
                                freq = round(freq * 1000.0f) / 1000.0f;

                                if (mode->type & DRM_MODE_TYPE_PREFERRED)
                                {
                                    resources += "\n" + to_string(mode->hdisplay)
                                        + "x" + to_string(mode->vdisplay) + " "
                                        + to_string(freq) + " " + " (prefered)";
                                        
                                }
                                else
                                {
                                    resources += "\n" + to_string(mode->hdisplay) 
                                        + "x" + to_string(mode->vdisplay) + " " 
                                        + to_string(freq);
                                }                                   
                            }

                            drmModeFreeConnector(connector);
                        }
                    }

                    resources += "\nframebuffers:";
                    for (int i = 0; i < res->count_fbs; i++)
                    {
                        resources += " " + to_string(res->fbs[i]);
                    }
                    resources += "\n";

                    resources += "\nCRTCs:";
                    for (int i = 0; i < res->count_crtcs; i++)
                    {
                        resources += " " + to_string(res->crtcs[i]);
                    }
                    resources += "\n";

                    resources += "\nencoders:";
                    for (int i = 0; i < res->count_encoders; i++)
                    {
                        resources += " " + to_string(res->encoders[i]);
                    }                    

                    drmModeFreeResources(res);
                }
                else
                {
                    resources += "\ncould not get drm resources for " + path;
                }

                close(fd);

                resources += "\n\n";
            }
        }            
    }    

    return resources;
}

void DisplayDrm::Cleanup()
{	
	if (_eventContextPtr != NULL)
	{				
		free(_eventContextPtr);
	}

	if (_bufId[0] != 0)
	{
		drmModeRmFB(_fd, _bufId[0]);
		_bufId[0] = 0;
	}

	if (_bufId[1] != 0)
	{
		drmModeRmFB(_fd, _bufId[0]);
		_bufId[1] = 0;
	}
	
	for (int i = 0; i < 2; i++)
	{
		if (_mmapPtr[i])
		{
			munmap(_mmapPtr[i], _createDumbReq[i].size);
			_mmapPtr[i] = NULL;
		}

		if (_createDumbReq[i].handle)
		{
			struct drm_mode_destroy_dumb destroyDumbReq;
			memset(&destroyDumbReq, 0, sizeof(destroyDumbReq));

			destroyDumbReq.handle = _createDumbReq[i].handle;
			drmIoctl(_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroyDumbReq);

			_createDumbReq[i].handle = 0;
		}
	}	

	if (_connectorPtr)
	{
		drmModeFreeConnector(_connectorPtr);
	}

	if (_fd > -1)
	{
		close(_fd);
		_fd = -1;
	}
}

DisplayDrm::DisplayDrm()
{	
	drmModeResPtr resPtr = NULL;
	drmModeEncoderPtr encoderPtr = NULL;
	drmDevicePtr devices[MAX_DRM_DEVICES] = { NULL };
	int crtc_index; // no used	
	int num_devices = 0;

	memset(&_createDumbReq, 0, sizeof(_createDumbReq));
	
	if (_driDevice != "")
	{
		if ((_fd = open(_driDevice.c_str(), O_RDWR)) < 0)
		{
			error = -1; // could not open drm device
			goto cleanup;
		}

		if (!(resPtr = drmModeGetResources(_fd)))
		{
			error = -2; // no suitable device specified
			goto cleanup;
		}
	}
	else
	{
		if ((num_devices = drmGetDevices2(0, devices, MAX_DRM_DEVICES)) < 0)
		{
			error = -3; // drmGetDevices failed
			goto cleanup;
		}

		for (int i = 0; i < num_devices; i++)
		{
			drmDevicePtr device = devices[i];

			if (!(device->available_nodes & (1 << DRM_NODE_PRIMARY)))
			{
				continue;
			}

			// it's a primary device. If we can get the  drmModeResources, it means it's also a KMS-capable device.

			if ((_fd = open(device->nodes[DRM_NODE_PRIMARY], O_RDWR)) < 0)
			{
				continue;
			}

			if ((resPtr = drmModeGetResources(_fd)) != 0)
			{
				break;
			}
			else
			{
				close(_fd);
				_fd = -1;
				continue;
			}
		}

		if (!resPtr || _fd == -1)
		{
			error = -4; // no suitable device found
			goto cleanup;
		}
	}

	for (int i = 0; i < resPtr->count_connectors; i++) // find a connected connector
	{
		_connectorPtr = drmModeGetConnector(_fd, resPtr->connectors[i]);
		if (_connectorPtr->connection == DRM_MODE_CONNECTED)
		{
			break; // use the first connected for now
		}
		drmModeFreeConnector(_connectorPtr);
		_connectorPtr = NULL;
	}

	if (!_connectorPtr)
	{
		error = -5; // no connected connector
		goto cleanup;
	}

	// find requested mode:	
	if (_mode != "")
	{
		for (int i = 0; i < _connectorPtr->count_modes; i++)
		{
			drmModeModeInfoPtr current_mode = &_connectorPtr->modes[i];

			if (strcmp(current_mode->name, _mode.c_str()) == 0)
			{
				if (_frequency == 0 || current_mode->vrefresh == _frequency)
				{
					_modeInfoPtr = current_mode;
					break;
				}
			}
		}

		if (!_modeInfoPtr)
		{
			error = -6; // requested mode not found, set mode_str=NULL for default mode
			goto cleanup;
		}
	}

	if (!_modeInfoPtr)
	{
		// find preferred mode or the highest resolution mode
		for (int i = 0, area = 0; i < _connectorPtr->count_modes; i++)
		{
			drmModeModeInfoPtr current_mode = &_connectorPtr->modes[i];

			if (current_mode->type & DRM_MODE_TYPE_PREFERRED)
			{
				_modeInfoPtr = current_mode;
				break;
			}

			int current_area = current_mode->hdisplay * current_mode->vdisplay;
			if (current_area > area)
			{
				_modeInfoPtr = current_mode;
				area = current_area;
			}
		}
	}

	if (!_modeInfoPtr)
	{
		error = -7; // prefered mode not found
		goto cleanup;
	}

	// find encoder
	for (int i = 0; i < resPtr->count_encoders; i++)
	{
		encoderPtr = drmModeGetEncoder(_fd, resPtr->encoders[i]);
		if (encoderPtr->encoder_id == _connectorPtr->encoder_id)
			break;
		drmModeFreeEncoder(encoderPtr);
		encoderPtr = NULL;
	}

	_crtcId = 0;

	if (encoderPtr)
	{
		_crtcId = encoderPtr->crtc_id;
	}
	else
	{
		for (int i = 0; i < _connectorPtr->count_encoders; i++)
		{
			const uint32_t encoder_id = _connectorPtr->encoders[i];
			drmModeEncoderPtr encoder = drmModeGetEncoder(_fd, encoder_id);

			if (encoder)
			{
				for (i = 0; i < resPtr->count_crtcs; i++) // https://dvdhrm.wordpress.com/2012/09/13/linux-drm-mode-setting-api/
				{
					const uint32_t crtc_mask = 1 << i;
					if (encoder->possible_crtcs & crtc_mask)
					{
						_crtcId = resPtr->crtcs[i];
						break;
					}
				}

				drmModeFreeEncoder(encoder);
				encoder = NULL;
			}
		}
	}

	if (_crtcId == 0)
	{
		error = -8; // no crtc found
		goto cleanup;
	}

	for (int i = 0; i < resPtr->count_crtcs; i++)
	{
		if (resPtr->crtcs[i] == _crtcId)
		{
			crtc_index = i;
			break;
		}
	}

	width = _modeInfoPtr->hdisplay;
	height = _modeInfoPtr->vdisplay;

	struct drm_mode_map_dumb mapDumbReq[2];
	memset(&mapDumbReq[0], 0, sizeof(drm_mode_map_dumb));
	memset(&mapDumbReq[1], 0, sizeof(drm_mode_map_dumb));

	for (int i = 0; i < 2; i++)
	{
		_createDumbReq[i].width = width;
		_createDumbReq[i].height = height;
		_createDumbReq[i].bpp = 32;

		if (drmIoctl(_fd, DRM_IOCTL_MODE_CREATE_DUMB, &_createDumbReq[i]) < 0)
		{
			error = -9; // cannot create dumb buffer
			goto cleanup;
		}

		mapDumbReq[i].handle = _createDumbReq[i].handle;

		if (drmModeAddFB(_fd, _modeInfoPtr->hdisplay, _modeInfoPtr->vdisplay, 24, 32, _createDumbReq[i].pitch, _createDumbReq[i].handle, &_bufId[i]))
		{
			error = -10; // cannot create framebuffers
			goto cleanup;
		}

		if (drmIoctl(_fd, DRM_IOCTL_MODE_MAP_DUMB, &mapDumbReq[i]))
		{
			error = -11; // buffer preparation failed
			goto cleanup;
		}

		_mmapPtr[i] = mmap(0, _createDumbReq[i].size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, mapDumbReq[i].offset);

		if (_mmapPtr[i] == MAP_FAILED)
		{
			error = -12; // cannot mmap map dumb buffer
			_mmapPtr[i] = NULL;
			goto cleanup;
		}

	}	

	skImageInfo = SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType);
	skBitmap.allocPixels(skImageInfo);	

	_eventContextPtr = (drmEventContext *)malloc(sizeof(drmEventContext));
	memset(_eventContextPtr, 0, sizeof(drmEventContext));
	_eventContextPtr->version = 2; //  DRM_EVENT_CONTEXT_VERSION; // versioned 
	_eventContextPtr->vblank_handler = NULL;
	_eventContextPtr->page_flip_handler = DisplayDrm::PageFlipHandler;

	return;

cleanup:	
	if (encoderPtr)
	{
		drmModeFreeEncoder(encoderPtr);
	}

	if (resPtr) 
	{ 
		drmModeFreeResources(resPtr); 
	}
	
	if (num_devices) drmFreeDevices(devices, num_devices);

	Cleanup();
}

DisplayDrm::DisplayDrm(string driDevice, string mode, unsigned int frequency) : DisplayDrm()
{
	_driDevice = driDevice;	
	_mode = mode;
	_frequency = frequency;
}

DisplayDrm::~DisplayDrm()
{
	PowerOff();	

	Cleanup();
}

void DisplayDrm::PowerOn()
{
	if (!_isOn)
	{
		int ret;

		if (_modeInfoPtr)
		{
			if (_mmapPtr[0]) memset(_mmapPtr[0], 0, _createDumbReq[0].size);
			if (_mmapPtr[1]) memset(_mmapPtr[1], 0, _createDumbReq[1].size);
			
			_modeCrtcOrigPtr = drmModeGetCrtc(_fd, _crtcId);

			ret = drmSetMaster(_fd);
			
			if (ret == 0)
			{
				drmModeSetCrtc(_fd, _crtcId, _bufId[0], 0, 0, &_connectorPtr->connector_id, 1, _modeInfoPtr);
			}
			else
			{
				if (_modeCrtcOrigPtr)
				{
					drmModeFreeCrtc(_modeCrtcOrigPtr);
					_modeCrtcOrigPtr = NULL;
				}
			}
		}

		_isOn = (ret == 0);
		
	}
}

void DisplayDrm::PowerOff()
{
	if (_isOn)
	{
		if (_modeCrtcOrigPtr)
		{
			int ret = 0;
			time_t t1, t2;
			double diff;

			time(&t1);
			time(&t2);
			while (!flipdone && (diff = difftime(t2, t1)) < 1 && !ret)
			{
				ret = drmHandleEvent(_fd, _eventContextPtr);
				time(&t2);
			}

			drmModeSetCrtc(_fd, _modeCrtcOrigPtr->crtc_id, _modeCrtcOrigPtr->buffer_id, 
				_modeCrtcOrigPtr->x, _modeCrtcOrigPtr->y, 
				&_connectorPtr->connector_id, 1, &_modeCrtcOrigPtr->mode);

			drmModeFreeCrtc(_modeCrtcOrigPtr);
		}
		
		if (_mmapPtr[0]) memset(_mmapPtr[0], 0, _createDumbReq[0].size);
		if (_mmapPtr[1]) memset(_mmapPtr[1], 0, _createDumbReq[1].size);

		if (_fd > -1) drmDropMaster(_fd);

		_isOn = false;
	}
}

bool DisplayDrm::IsOn()
{
	return _isOn;
}

void DisplayDrm::PageFlipHandler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void* data)
{
	((DisplayDrm *)data)->flipdone = true;
}

void DisplayDrm::Update()
{
	if (!_isOn || !_modeCrtcOrigPtr)
	{
		return;
	}
	
	size_t s = width * height * 4;
	
	bidx = bidx ? 0 : 1;

	if (_mmapPtr[bidx])
	{
		memcpy(_mmapPtr[bidx], skBitmap.getPixels(), s);
	}	

	if (_fd)
	{
		int ret = 0;
		time_t t1, t2;
		double diff;

		time(&t1);
		time(&t2);
		while (!flipdone && (diff = difftime(t2, t1)) < 1 && !ret)
		{
			ret = drmHandleEvent(_fd, _eventContextPtr);
			time(&t2);
		}
		
		if (diff < 2)
		{
			flipdone = false;
			ret = drmModePageFlip(_fd, _crtcId, _bufId[bidx], DRM_MODE_PAGE_FLIP_EVENT, this);
		}
		else
		{
			flipdone = true;
		}
		 
	}
	
}



















