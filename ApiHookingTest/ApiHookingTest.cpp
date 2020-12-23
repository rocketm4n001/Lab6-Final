#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
    HANDLE hFile;
    char DataBuffer[] = "This is some test data to write to the file.";
    DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;
    BOOL bErrorFlag = FALSE;

    hFile = CreateFile(L"",                // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_NEW,             // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);

    OFSTRUCT buffer;
    HFILE hfile = OpenFile("D:\\test.txt", &buffer, OF_READ);
    char buffer_read[6];
    DWORD bytes_read = 0;
    system("PAUSE");
    return 0;
}
