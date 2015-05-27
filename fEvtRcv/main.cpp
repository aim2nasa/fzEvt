#include <ace/Log_Msg.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_stdlib.h>
#include <ace/SString.h>

#define SWIPE_DISTANCE	50
#define	timersub(tvp, uvp, vvp)	\
	do {\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;\
		if ((vvp)->tv_usec < 0) {\
			(vvp)->tv_sec--;\
			(vvp)->tv_usec += 1000000;\
								}\
				} while (0)

void evAbs(ACE_TString& code, ACE_TString& value, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime);
void AbsMtTrackingId(ACE_TString& value, bool& is_swipe, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime);
void AbsMtPositionX(ACE_TString& value, bool& is_swipe, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime);
void AbsMtPositionY(ACE_TString& value, bool& is_swipe, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime);

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	ACE_TCHAR buffer[512];
	FILE   *pPipe;

	if ((pPipe = _popen("adb shell getevent -t -l ", "rt")) == NULL)
		ACE_ERROR((LM_ERROR, "(%t)\n", "_popen() error", -1));

	ACE_TCHAR* token = ACE_TEXT("[].: \r\n");

	ACE_TString tv_sec, tv_usec, device, type, code, value;
	timeval evt_time[6],evt_time_temp;
	int x1 = -1, x2 = -1, y1 = -1, y2 = -1;

	while (fgets(buffer, sizeof(buffer), pPipe))
	{
		char *p;
		p = ACE_OS::strtok(buffer, token); /* sec */ if (p == 0) continue;
		tv_sec = p;
		p = ACE_OS::strtok(0, token); /* usec */ if (p == 0) continue;
		tv_usec = p;
		p = ACE_OS::strtok(0, token); /* dev(skip) */ if (p == 0) continue;
		device = p;
		p = ACE_OS::strtok(0, token); /* type */ if (p == 0) continue;
		type = p;
		p = ACE_OS::strtok(0, token); /* code */ if (p == 0) continue;
		code = p;
		p = ACE_OS::strtok(0, token); /* value */ if (p == 0) continue;
		value = p;

		evt_time_temp.tv_sec = ACE_OS::atoi(tv_sec.c_str());
		evt_time_temp.tv_usec = ACE_OS::atoi(tv_usec.c_str());

		ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) %s.%s %s,%s,%s,%s\n"),
			tv_sec.c_str(), tv_usec.c_str(), device.c_str(), type.c_str(), code.c_str(), value.c_str()));

		if (type == "EV_ABS") evAbs(code,value,x1,x2,y1,y2,evt_time,&evt_time_temp);
	}
	_pclose(pPipe);

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) end of main\n")));
	ACE_RETURN(0);
}

void evAbs(ACE_TString& code, ACE_TString& value, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime)
{
	bool is_swipe = false;

	if (code == "ABS_MT_TRACKING_ID") AbsMtTrackingId(value, is_swipe, x1, x2, y1, y2, pTime, pTempTime);
	if (code == "ABS_MT_POSITION_X") AbsMtPositionX(value, is_swipe, x1, x2, y1, y2, pTime, pTempTime);
	if (code == "ABS_MT_POSITION_Y") AbsMtPositionY(value, is_swipe, x1, x2, y1, y2, pTime, pTempTime);
}

void AbsMtTrackingId(ACE_TString& value, bool& is_swipe, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime)
{
	timeval* evt_time = pTime;
	timeval& evt_time_temp = *pTempTime;

	if (value == "ffffffff" && x1 != -1 && y1 != -1)
	{
		char file_path[512];
		char save_name[512];
		SYSTEMTIME st;
		GetSystemTime(&st);
		sprintf(save_name, "%s_%04d%02d%02d_%02d%02d%02d_%03d.txt", "NA",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		sprintf_s(file_path, "%s",save_name);
		FILE* write_fp = fopen(file_path, "w");
		if (write_fp)
		{
			//fprintf(write_fp, "time: %04d%02d%02d,%02d%02d%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			/* 이벤트 발생시 단말의 시간, 이벤트 발생 시간 보내줘야 함 */
			ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) EVENT [x1:%d x2:%d (%d) y1:%d y2:%d (%d)]\n"),x1, x2, abs(x2 - x1), y1, y2, abs(y2 - y1)));

			if ((x2 != -1 && y2 != -1) && (abs(x2 - x1) >= SWIPE_DISTANCE || abs(y2 - y1) >= SWIPE_DISTANCE)){
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ** Swipe Event **\n")));
				is_swipe = true;
			}else
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) ** Tap Event **\n")));

			if (is_swipe == false) {
				fprintf(write_fp, "eventname : TAP\n");
				fprintf(write_fp, "eventcount : 4\n");
			}else{
				fprintf(write_fp, "eventname : SWIPE\n");
				fprintf(write_fp, "eventcount : 6\n");
			}

			evt_time[3] = evt_time_temp;
			evt_time_temp = evt_time[0];
			timersub(&evt_time[0], &evt_time_temp, &evt_time[0]);
			timersub(&evt_time[1], &evt_time_temp, &evt_time[1]);
			timersub(&evt_time[2], &evt_time_temp, &evt_time[2]);
			timersub(&evt_time[3], &evt_time_temp, &evt_time[3]);
			timersub(&evt_time[4], &evt_time_temp, &evt_time[4]);
			timersub(&evt_time[5], &evt_time_temp, &evt_time[5]);

			if (is_swipe == false) {
				fprintf(write_fp, "eventtime : %d, %d, %d, %d\n",
					(evt_time[0].tv_sec * 1000) + (evt_time[0].tv_usec / 1000) > 0 ? (evt_time[0].tv_sec * 1000) + (evt_time[0].tv_usec / 1000) : 0,
					(evt_time[1].tv_sec * 1000) + (evt_time[1].tv_usec / 1000) > 0 ? (evt_time[1].tv_sec * 1000) + (evt_time[1].tv_usec / 1000) : 0,
					(evt_time[2].tv_sec * 1000) + (evt_time[2].tv_usec / 1000) > 0 ? (evt_time[2].tv_sec * 1000) + (evt_time[2].tv_usec / 1000) : 0,
					(evt_time[3].tv_sec * 1000) + (evt_time[3].tv_usec / 1000) > 0 ? (evt_time[3].tv_sec * 1000) + (evt_time[3].tv_usec / 1000) : 0);
				fprintf(write_fp, "eventid : 0, 0, 0, 0\n");
				fprintf(write_fp, "eventnamebyid : %s, %s, %s, %s\n", "DOWN", "X", "Y", "UP");
				fprintf(write_fp, "eventvaluebyid : %d, %d, %d, %d", 0, x1, y1, 0);
			}else{
				fprintf(write_fp, "eventtime : %d, %d, %d, %d, %d, %d\n",
					(evt_time[0].tv_sec * 1000) + (evt_time[0].tv_usec / 1000) > 0 ? (evt_time[0].tv_sec * 1000) + (evt_time[0].tv_usec / 1000) : 0,
					(evt_time[1].tv_sec * 1000) + (evt_time[1].tv_usec / 1000) > 0 ? (evt_time[1].tv_sec * 1000) + (evt_time[1].tv_usec / 1000) : 0,
					(evt_time[2].tv_sec * 1000) + (evt_time[2].tv_usec / 1000) > 0 ? (evt_time[2].tv_sec * 1000) + (evt_time[2].tv_usec / 1000) : 0,
					(evt_time[4].tv_sec * 1000) + (evt_time[4].tv_usec / 1000) > 0 ? (evt_time[4].tv_sec * 1000) + (evt_time[4].tv_usec / 1000) : 0,
					(evt_time[5].tv_sec * 1000) + (evt_time[5].tv_usec / 1000) > 0 ? (evt_time[5].tv_sec * 1000) + (evt_time[5].tv_usec / 1000) : 0,
					(evt_time[3].tv_sec * 1000) + (evt_time[3].tv_usec / 1000) > 0 ? (evt_time[3].tv_sec * 1000) + (evt_time[3].tv_usec / 1000) : 0);
				fprintf(write_fp, "eventid : 0, 0, 0, 0, 0, 0\n");
				fprintf(write_fp, "eventnamebyid : %s, %s, %s, %s, %s, %s\n", "DOWN", "X", "Y", "X", "Y", "UP");
				fprintf(write_fp, "eventvaluebyid : %d, %d, %d, %d, %d, %d", 0, x1, y1, x2, y2, 0);
			}
			fclose(write_fp);

			if (is_swipe == false) {
				//write_event_file(x1, y1);
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) writeEvent(x1:%d,y1:%d) **\n"), x1, y1));
			}else{
				//write_event_file(x1, y1, x2, y2);
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) writeEvent(x1:%d,x2:%d,y1:%d,y2:%d) **\n"), x1, x2, y1, y2));
			}
			x1 = y1 = x2 = y2 = -1;
			memset(evt_time, 0, sizeof(evt_time));
		}
	}
	else
		evt_time[0] = evt_time_temp;
}

void AbsMtPositionX(ACE_TString& value, bool& is_swipe, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime)
{
	if (x1 == -1){
		x1 = strtoul(value.c_str(), 0, 16);
		pTime[1] = *pTempTime;
	}else {
		x2 = strtoul(value.c_str(), 0, 16);
		pTime[4] = *pTempTime;
	}
}

void AbsMtPositionY(ACE_TString& value, bool& is_swipe, int& x1, int& x2, int& y1, int& y2, timeval* pTime, timeval* pTempTime)
{
	if (y1 == -1) {
		y1 = strtoul(value.c_str(), 0, 16);
		pTime[2] = *pTempTime;
	}else{
		y2 = strtoul(value.c_str(), 0, 16);
		pTime[5] = *pTempTime;
	}
}