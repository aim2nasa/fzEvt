#include <ace/Log_Msg.h>
#include <stdio.h>

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	char   psBuffer[128];
	FILE   *pPipe;

	if ((pPipe = _popen("adb shell getevent -t -l ", "rt")) == NULL)
		ACE_ERROR((LM_ERROR, "(%t)\n", "_popen() error", -1));

	while (fgets(psBuffer, 128, pPipe))
	{
		printf(psBuffer);
	}
	feof(pPipe);

	ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) end of main\n")));
	ACE_RETURN(0);
}