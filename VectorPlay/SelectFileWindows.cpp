
/*
Copyright 2017 Kjetil Hvalstrand (LiveForIt-Software.net)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <iostream>
#include <windows.h>



// basicly we get a filename if was success, or else NULL
// error code can be read, but wont be correct on different OS's for now.


int wstrlen( TCHAR * wstr)
{
	int l_idx = 0;
	while (((char*)wstr)[l_idx] != 0) l_idx += 2;
	return l_idx;
}

char *wstrdup(TCHAR *wSrc)
{
	int l_idx = 0;
	int l_len = wstrlen(wSrc);
	char *l_nstr = (char *)malloc(l_len);
	if (l_nstr) {
		do {
			l_nstr[l_idx] = (char)wSrc[l_idx];
			l_idx++;
		} while ((char)wSrc[l_idx] != 0);
	}
	l_nstr[l_idx] = 0;
	return l_nstr;
}


char *BasicFileSave(int *errorcode)
{
	TCHAR filename[MAX_PATH] = TEXT("\0");
	OPENFILENAME   ofn;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Save xml (*.xml)\0*.xml\0");
	ofn.lpstrTitle = TEXT("Save animation xml As");
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("txt");

	if (GetSaveFileNameW(&ofn))
	{
		return wstrdup( filename);
	}
	
	return NULL;
}


char *BasicFileOpen( int *errorcode )
{
	char filename[MAX_PATH];

	OPENFILENAMEA ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter =  "anim xml\0*.xml\0Any File\0*.*\0";
	ofn.lpstrFile =  filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select animation xml file!";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA( &ofn))
	{
//		std::cout << "You chose the file \"" << filename << "\"\n";
		*errorcode = 0;
		return strdup(filename);
	}
	else
	{
		// we don't care about OS, or what error code is for now, this just quick implmentation.
		*errorcode = CommDlgExtendedError();

		/*
		// All this stuff below is to tell you exactly how you messed up above. 
		// Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
		case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
		case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
		case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
		case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
		case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
		case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
		case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
		case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
		case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
		case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
		case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
		case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
		case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
		case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
		default: std::cout << "You cancelled.\n";
		}
		*/
	}
	return NULL;
}
