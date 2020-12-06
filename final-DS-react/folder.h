#pragma once
#include"Content.h"
#include"binarySearchTree.h"

class Folder {
private:
	string name;	// tag
	BinarySearchTree<string> contents;	// includes content name
	int count;
public:
	Folder() {
		name = "";
		count = 0;
	}
	~Folder() {}

	Folder(string name) { this->name = name; count = 0; }

	bool operator >(Folder& target) {
		if (this->name > target.getName())
			return true;
		return false;
	}
	bool operator ==(Folder& target) {
		if (this->name == target.getName())
			return true;
		return false;
	}
	bool operator <(Folder& target) {
		if (this->name < target.getName())
			return true;
		return false;
	}

	string getName() { return name; }
	int getCount() { return count; }
	BinarySearchTree<string> getContent() { return contents; }

	void Add(string name) { contents.Add(name); count++; }
	void Delete(string name) { contents.DeleteItem(name); count--; }
	

};