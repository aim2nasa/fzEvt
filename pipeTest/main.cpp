// crt_popen.c
/* This program uses _popen and _pclose to receive a
* stream of text from a system process.
*/

#include <stdio.h>
#include <stdlib.h>

int main(void)
{

	char   psBuffer[128];
	FILE   *pPipe;

	/* Run DIR so that it writes its output to a pipe. Open this
	* pipe with read text attribute so that we can read it
	* like a text file.
	*/

	if ((pPipe = _popen("dir c:\\*.* /on /p", "rt")) == NULL)
		exit(1);

	/* Read pipe until end of file, or an error occurs. */

	while (fgets(psBuffer, 128, pPipe))
	{
		printf(psBuffer);
	}


	/* Close pipe and print return value of pPipe. */
	if (feof(pPipe))
	{
		printf("\nProcess returned %d\n", _pclose(pPipe));
	}
	else
	{
		printf("Error: Failed to read the pipe to the end.\n");
	}
}