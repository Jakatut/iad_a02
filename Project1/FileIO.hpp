#pragma once
#include <iostream>
#include <fstream>
#include <string>

class FileIO {
private:
	std::fstream file;
	unsigned char flags;

	std::string fileName;
	std::string textRead;

	void CloseFile();
	void OpenFile();

public:

	FileIO(std::string fileName, bool binary = false);

	bool Write(std::string text);
	bool Read();

	// getter method for the text
	std::string GetTextRead() { return this->textRead; }

	~FileIO();

};
