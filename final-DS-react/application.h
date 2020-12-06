#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <cpprest/http_listener.h>
#include<cpprest/json.h>
#include<future>
#include<Windows.h>


#include"binarySearchTree.h"
#include"sortedLinkedList.h"
#include"content.h"
#include"folder.h"

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace utility;

class Application {
private:
	BinarySearchTree<Content> masterList;	// Content master list
	BinarySearchTree<Folder> FolderList;	// Folder master list

public:
	// constructor & destructor
	Application() {}
	~Application() {}

	// application run function
	void Run();

	// application main functions, response json
	void uploadContent(web::json::value &Json);		// upload content
	void deleteContent(web::json::value& Json);		// if target content is exists, delete it
	void showContentDetail(web::json::value& Json);	// show content's details
	void updateContent(web::json::value& Json);		// update target content as Json

	void showAllContent(web::json::value& Json);	// send all contents to client
	void showAllFolder(web::json::value& Json);		// send all folders to client
	void showFolder(web::json::value& Json);		// show folder's detail
	void search(web::json::value& Json);			// search with searchWord on Json

	// convert wstring to string
	string wsTos(wstring target) {
		string temp;
		temp.assign(target.begin(), target.end());

		return temp;
	}

	// convert string to wstring
	wstring sTows(string target) {
		wstring temp;
		temp.assign(target.begin(), target.end());

		return temp;
	}

	// get current time string, used to set last use time
	string currentTime() {
		time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링

		return buf;
	}
};