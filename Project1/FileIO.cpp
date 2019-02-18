#include "FileIO.hpp"

FileIO::FileIO(std::string fileName, bool binary, bool append) {

	this->flags = std::ios::out | std::ios::in;
	this->flags |= (binary) ? std::ios::binary : 0;
	this->flags |= (append) ? std::ios::app : 0;
	this->textRead = "";
	this->fileName = fileName;

}

void FileIO::OpenFile() {

	// attempt to open the file
	this->file.open(this->fileName, this->flags);
	
	if (!this->file) {

		// create the file
		this->file.open(this->fileName, std::ios::app);
		CloseFile();

		// attempt to open the file again
		this->file.open(this->fileName, this->flags);

	}

}


bool FileIO::Write(std::string text) {

	OpenFile();

	// make sure the file is open
	if (!this->file.is_open() || !this->file) { return false; }

	// write to the file
	this->file << text.c_str() + '\0';

	CloseFile();

	return true;

}

bool FileIO::Read() {

	OpenFile();

	std::string line = "";
	this->textRead = "";

	// make sure the file is open
	if (!this->file.is_open() || !this->file) { return false; }

	while (this->file.good()) {

		std::getline(this->file, line);
		this->textRead += line + "\n";

	}

	CloseFile();

	return true;

}

void FileIO::CloseFile() {

	if (this->file.is_open()) {

		this->file.close();

	}

}

FileIO::~FileIO() {

	this->CloseFile();

}