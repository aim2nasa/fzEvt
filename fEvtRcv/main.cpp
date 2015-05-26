#include <ace/Log_Msg.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	char   buffer[512];
	FILE   *pPipe;

	if ((pPipe = _popen("adb shell getevent -t -l ", "rt")) == NULL)
		ACE_ERROR((LM_ERROR, "(%t)\n", "_popen() error", -1));

	char* token = "[].: \r\n";

	std::string tv_sec, tv_usec;
	std::string device,type;
	std::string code;
	std::string value;

	timeval evt_time[6];
	timeval evt_time_temp;
	std::string evt_code[2];
	int x1 = -1; int y1 = -1;
	int x2 = -1; int y2 = -1;

	while (fgets(buffer, sizeof(buffer), pPipe))
	{
		//printf(buffer);

		char *p;
		p = strtok(buffer, token); /* sec */ if (p == 0) continue;
		tv_sec = p;
		p = strtok(0, token); /* usec */ if (p == 0) continue;
		tv_usec = p;
		p = strtok(0, token); /* dev(skip) */ if (p == 0) continue;
		device = p;
		p = strtok(0, token); /* type */ if (p == 0) continue;
		type = p;
		p = strtok(0, token); /* code */ if (p == 0) continue;
		code = p;
		p = strtok(0, token); /* value */ if (p == 0) continue;
		value = p;
		evt_time_temp.tv_sec = atoi(tv_sec.c_str());
		evt_time_temp.tv_usec = atoi(tv_usec.c_str());

		cout << tv_sec << "." << tv_usec << "," << device << "," << type << "," << code << "," << value << endl;
	}
	feof(pPipe);

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) end of main\n")));
	ACE_RETURN(0);
}