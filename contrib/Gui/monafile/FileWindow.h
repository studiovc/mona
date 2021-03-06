#ifndef __MONAFILE_FILEWINDOW_H__
#define __MONAFILE_FILEWINDOW_H__

#include "FileBrowser.h"

class FileWindow : public System::Mona::Forms::Form
{
private:
	_P<FileBrowser> browser;
	
public:
	FileWindow();
	virtual ~FileWindow() {}
	
	virtual void Create();
	
	void set_Directory(System::String path);
};

#endif  // __MONAFILE_FILEWINDOW_H__
