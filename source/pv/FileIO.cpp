#include "pv/FileIO.h"

File::File(std::string fileName, bool loadData)
{
	this->dataLoaded = false;
	this->data = NULL;
	this->fileLength = 0;

	this->fileStream.open(fileName.c_str(), std::ios::in);

	if(this->fileStream)
	{
		if(this->fileStream.good())
		{
			if(loadData)
			{
				this->LoadData();
			}
		}
	}
}

void File::LoadData()
{
	this->dataLoaded = false;
	this->fileStream.seekg(0, std::ios::end);
	this->fileLength = this->fileStream.tellg();
	this->fileStream.seekg(0, std::ios::beg);

	if(this->data != NULL)
	{
		free(this->data);
		this->data = NULL;
	}
	if(this->fileLength > 0)
	{
		this->data = (char*)malloc(this->fileLength * sizeof(char));

		if(this->data != NULL)
		{
			this->fileStream.read(this->data, this->fileLength);
			this->dataLoaded = true;
		}
	}
}

const bool File::DataLoaded() const
{
	return this->dataLoaded;
}
const char* File::Data() const
{
	return this->data;
}
const unsigned long File::Size() const
{
	return this->fileLength;
}

void File::FreeData()
{
	if(this->data != NULL)
	{
		free(this->data);
		this->data = NULL;
	}

	this->fileLength = 0;
	this->dataLoaded = false;
}

void File::Close()
{
	if(this->fileStream)
	{
		if(this->fileStream.good())
		{
			this->fileStream.close();
		}
	}
}

File::~File()
{
	this->FreeData();
	this->Close();
}