#pragma once
#pragma warning(disable: 4996)  

#include<string>
#include<sstream>
#include<time.h>

#include"binarySearchTree.h"
#include"unsortedList.h"
#include"sortedLinkedList.h"

using namespace std;

class Content {
public:
	Content();
	Content(string name);
	Content(string name, string uploaded, string used, string description, string tags);
	~Content() {}

	string getName() {
		return name;
	}
	string getUploadedAt() {
		return uploadedAt;
	}
	string getUsedAt() {
		return usedAt;
	}
	string getDescription() {
		return description;
	}
	MySortedLinkedList<string> getTags() {
		return tags;
	}
	string getTagsAsString() {
		string tagString;
		tags.ResetList();
		for (int i = 0; i < tags.GetLength(); i++) {
			string temp;
			tags.GetNextItem(temp);
			
			tagString += temp;
			tagString += " ";
		}

		return tagString;
	}

	void setName(string name) {
		this->name = name;
	}
	void setUploadedAt(string time) {
		this->uploadedAt = time;
	}
	void setUsedAt(string time) {
		this->usedAt = time;
	}
	void setdescription(string desc) {
		this->description = desc;
	}
	void setTags(string tags) {
		this->tags.MakeEmpty();

		stringstream ss(tags);
		string temp;

		while (getline(ss, temp, ' ')) {
			this->tags.Add(temp);
		}
	}
	void setUsedAtAsCurrent() {
		usedAt = currentTime();
	}

	bool operator <(Content& target) {
		if (this->name < target.name)
			return true;
		return false;
	}
	bool operator ==(Content& target) {
		if (this->name == target.name)
			return true;
		return false;
	}
	bool operator >(Content& target) {
		if (this->name > target.name)
			return true;
		return false;
	}

	string currentTime();

private:
	string name;
	string uploadedAt;
	string usedAt;
	string description;
	MySortedLinkedList<string> tags;
};