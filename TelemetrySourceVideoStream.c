/**
 * this is the command to find out if a camera is connected
 * /opt/vc/bin/vcgencmd get_camera
 * supported=0 detected=0
 */
static BOOL TelemetrySourceVideoStream()
{
	if(0==system("/opt/vc/bin/vcgencmd get_camera | grep supported=1")
		if(0==system("/opt/vc/bin/vcgencmd get_camera | grep detected=1")
			return TRUE;
	
	return FALSE;
}
