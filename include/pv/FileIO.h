#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include <iostream>
#include <fstream>

class File
{
public:
	File(std::string fileName, bool loadData);
	void LoadData();
	const bool DataLoaded() const;
	const char* Data() const;
	const unsigned long Size() const;
	void FreeData();
	~File();
protected:
	std::ifstream fileStream;
	bool dataLoaded;
	char* data;
	unsigned long fileLength;
	void Close();
};

#endif

