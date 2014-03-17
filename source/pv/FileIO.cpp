#include "pv/FileIO.h"
#include <iostream>

namespace PV
{
	File::File(const char* fileName, bool loadData)
	{
		this->dataLoaded = false;
		this->data = NULL;
		this->fileLength = 0;

		this->file = fopen(fileName, "rb");

		if (this->file)
		{
			if (loadData)
			{
				this->LoadData();
			}
		}
	}

	void File::LoadData()
	{
		this->dataLoaded = false;
		fseek(this->file, 0, SEEK_END);
		this->fileLength = ftell(this->file);
		fseek(this->file, 0, SEEK_SET);

		if (this->data != NULL)
		{
			free(this->data);
			this->data = NULL;
		}
		if (this->fileLength > 0)
		{
			this->data = (char*)malloc(this->fileLength * sizeof(char));

			if (this->data != NULL)
			{
				fread(this->data, sizeof(char), this->fileLength, this->file);
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
		if (this->data != NULL)
		{
			free(this->data);
			this->data = NULL;
		}

		this->fileLength = 0;
		this->dataLoaded = false;
	}

	void File::Close()
	{
		if (this->file)
		{
			fclose(this->file);
		}
	}

	File::~File()
	{
		this->FreeData();
		this->Close();
	}
};
