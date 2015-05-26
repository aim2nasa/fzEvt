#include <ace/Log_Msg.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_stdlib.h>
#include <ace/SString.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	ACE_TCHAR buffer[512];
	FILE   *pPipe;

	if ((pPipe = _popen("adb shell getevent -t -l ", "rt")) == NULL)
		ACE_ERROR((LM_ERROR, "(%t)\n", "_popen() error", -1));

	ACE_TCHAR* token = ACE_TEXT("[].: \r\n");

	ACE_TString tv_sec, tv_usec, device, type, code, value;
	timeval evt_time_temp;
	int x1=-1, x2=-1, y1=-1, y2=-1;

	while (fgets(buffer, sizeof(buffer), pPipe))
	{
		//printf(buffer);

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

		cout << tv_sec << "." << tv_usec << "," << device << "," << type << "," << code << "," << value << endl;
	}
	feof(pPipe);

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) end of main\n")));
	ACE_RETURN(0);
}