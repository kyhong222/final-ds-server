#include"application.h"

// cors �̽��� �ذ��ϱ� ���� header�� �߰�
void handle_options(http_request request) {
	http_response response(status_codes::OK);
	response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
	response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
	response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
	response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
	request.reply(response);
}

// application run function
void Application::Run() {
	http_listener listener(U("http://localhost:9090"));        //Server URL, Port ����.

	listener.support(methods::OPTIONS, handle_options);			// OPTIONS �޼ҵ带 ���� CORS �̽� ����
	listener.support(methods::POST, [&listener, this](http_request req) {
		/*
			1. Json�� ����
			2. Json.fnc ����� value�� ���� ������ �Լ� ����
			3. Json�� ������ �Լ��� ���� �Ѱ���
			4. �Լ����� �ʿ��� ������ ó���� ��, response�� Json�� �ٽ� �����
			5. response�� client�� ����
		*/

		// 1. Json�� ����
		web::json::value Json;
		req.extract_json()       //extracts the request content into a json
			.then([&Json, this](pplx::task<json::value> task)
			{
				Json = task.get();
			}).wait();	

		// 2. Json.fnc�� �����Ͽ� �˸��� �Լ� ����
		// 3, 4. Json�� reference�� �Ѱ��־���, �Լ� ���ο��� �˸��� response�� Json�� �����
		json::value fnc = Json.at(U("fnc"));
		wstring fncS = fnc.as_string();
		if (fncS == U("uploadContent")) 
			this->uploadContent(Json);
		else if (fncS == U("deleteContent"))
			this->deleteContent(Json);
		else if (fncS == U("showContentDetail"))
			this->showContentDetail(Json);
		else if (fncS == U("updateContent"))
			this->updateContent(Json);
		else if (fncS == U("showAllContent"))
			this->showAllContent(Json);
		else if (fncS == U("showAllFolder"))
			this->showAllFolder(Json);
		else if (fncS == U("showFolder"))
			this->showFolder(Json);
		else if (fncS == U("search"))
			this->search(Json);
		else {
			cout << "fail" << endl;
		}

		// 5. client�� ����
		http_response response(status_codes::OK);
		response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
		response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
		response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
		response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
		response.set_body(Json);

		// �ֿܼ� log ���
		cout << "response : ";
		wcout << Json << endl;
		req.reply(response);
	});


	// ���� open
	listener.open().then([&listener]() {
		cout << "Server started at 9090" << endl;
		}).wait();    //Server open
		while (true);
		listener.close();
}

void Application::uploadContent(web::json::value& Json) {
	// �ֿܼ� � Json�� �Դ��� �α� ���
	wcout << Json << endl;

	// Json���� �ʿ��� ��� ����
	string name = wsTos(Json.at(U("name")).as_string());
	string desc = wsTos(Json.at(U("description")).as_string());
	string tags = wsTos(Json.at(U("tags")).as_string());

	string uploaded = currentTime();
	string used = uploaded;

	// new Content ����
	Content newContent(name, uploaded, used, desc, tags);

	// masterList�� �߰�
	masterList.Add(newContent);

	// tag�� �°� ������ �߰�
	MySortedLinkedList<string> tagList = newContent.getTags();
	tagList.ResetList();

	// add on folder
	for (int i = 0; i < tagList.GetLength(); i++) {
		string tempTag;
		tagList.GetNextItem(tempTag);
		Folder tempFolder(tempTag);
		bool flg = false;
		FolderList.RetrieveItem(tempFolder, flg);
		tempFolder.Add(name);

		// if no folder, add on folderList
		if (!flg) {
			FolderList.Add(tempFolder);
		}
		// if folder exists, update it
		else {
			FolderList.UpdateItem(tempFolder);
		}
	}

	// create response json
	web::json::value newJson;
	
	string result = "OK";
	newJson[U("result")] = web::json::value::string(sTows(result));

	// response
	Json = newJson;
}

void Application::deleteContent(web::json::value& Json) {
	// �ֿܼ� JSON ���
	wcout << Json << endl;

	// json���κ��� name ����
	string name = wsTos(Json.at(U("name")).as_string());

	// ���� �غ�
	bool found = false;
	Content target(name);

	masterList.RetrieveItem(target, found);

	if (found) {
		// target.tag ��� folder�� ã��, 
		MySortedLinkedList<string> tagList = target.getTags();
		tagList.ResetList();
		for (int i = 0; i < tagList.GetLength(); i++) {
			string tag;
			tagList.GetNextItem(tag);
			bool folderFound = false;

			Folder tempFolder(tag);
			FolderList.RetrieveItem(tempFolder, folderFound);

			// folder���� target.name�� ����
			tempFolder.Delete(name);
			
			// �������� �ݿ�
			FolderList.UpdateItem(tempFolder);
		}

		// masterList���� ��������
		masterList.DeleteItem(target);
	}

	// create response json
	web::json::value newJson;

	string result = "OK";
	newJson[U("result")] = web::json::value::string(sTows(result));

	Json = newJson;

}
void Application::showContentDetail(web::json::value& Json) {
	// json���κ��� name ����
	wcout << Json << endl;
	string name = wsTos(Json.at(U("name")).as_string());
	
	// masterList���� ã��
	Content target(name);
	bool found = false;
	masterList.RetrieveItem(target, found);

	// response
	web::json::value newJson;
	if (found) {
		// ã�Ҵٸ� -> usedAt ���� �� content�� ���� json���� �� response

		target.setUsedAtAsCurrent();

		newJson[U("name")] = web::json::value::string(sTows(target.getName()));
		newJson[U("uploaded")] = web::json::value::string(sTows(target.getUploadedAt()));
		newJson[U("used")] = web::json::value::string(sTows(target.getUsedAt()));
		newJson[U("description")] = web::json::value::string(sTows(target.getDescription()));
		newJson[U("tags")] = web::json::value::string(sTows(target.getTagsAsString()));

		Json = newJson;
	}
	else {
		// ��ã�Ҵٸ� -> ��ã������ response
		newJson[U("result")] = web::json::value::string(L"failed");
	}
}
void Application::updateContent(web::json::value& Json) {
	// json���κ��� element ����
	wcout << Json << endl;
	string name = wsTos(Json.at(U("name")).as_string());
	string desc = wsTos(Json.at(U("description")).as_string());
	string tags = wsTos(Json.at(U("tags")).as_string());

	// masterList���� ã��
	Content target(name);
	bool found = false;
	masterList.RetrieveItem(target, found);

	// response
	web::json::value newJson;

	if (found) {
		// ã�Ҵٸ� -> usedAt ���� �� content ������Ʈ �� ok response

		target.setUsedAtAsCurrent();

		target.setdescription(desc);

		// tag�� ���� folder ������Ʈ
		// ���� ���� tag folder���� ����������
		MySortedLinkedList<string> tagList = target.getTags();
		tagList.ResetList();
		for (int i = 0; i < tagList.GetLength(); i++) {
			string tag;
			tagList.GetNextItem(tag);
			bool folderFound = false;

			Folder tempFolder(tag);
			FolderList.RetrieveItem(tempFolder, folderFound);

			// folder���� target.name�� ����
			tempFolder.Delete(name);
			FolderList.UpdateItem(tempFolder);
		}

		// �� tags������� �ٽ� folder�� ����
		target.setTags(tags);
		tagList = target.getTags();
		tagList.ResetList();
		// add on folder
		for (int i = 0; i < tagList.GetLength(); i++) {
			string tempTag;
			tagList.GetNextItem(tempTag);
			Folder tempFolder(tempTag);
			bool flg = false;
			FolderList.RetrieveItem(tempFolder, flg);
			tempFolder.Add(name);

			// if no folder, add on folderList
			if (!flg) {
				FolderList.Add(tempFolder);
			}
			else {
				FolderList.UpdateItem(tempFolder);
			}
		}

		masterList.UpdateItem(target);

		newJson[U("result")] = web::json::value::string(L"OK");
	}
	else {
		// ��ã�Ҵٸ� -> failed response
		newJson[U("result")] = web::json::value::string(L"failed");
	}
}

void Application::showAllContent(web::json::value& Json) {
	// �ֿܼ� JSON ���
	wcout << Json << endl;

	//masterList�� sortedLinkedList�� ��ȯ
	MySortedLinkedList<Content> itemList = masterList.GetItemList();
	itemList.ResetList();

	// add on json
	json::value arr;
	for (int i = 0; i < itemList.GetLength(); i++) {
		Content target;
		itemList.GetNextItem(target);
		
		web::json::value contentJson;

		contentJson[U("name")] = web::json::value::string(sTows(target.getName()));
		contentJson[U("uploaded")] = web::json::value::string(sTows(target.getUploadedAt()));
		contentJson[U("used")] = web::json::value::string(sTows(target.getUsedAt()));
		contentJson[U("description")] = web::json::value::string(sTows(target.getDescription()));
		contentJson[U("tags")] = web::json::value::string(sTows(target.getTagsAsString()));

		arr[i] = contentJson;
	}

	Json = arr;

}
void Application::showAllFolder(web::json::value& Json) {
	// json�� �ֿܼ� ���
	wcout << Json << endl;

	// folderList�� sortedList�� ��ȯ
	MySortedLinkedList<Folder> folderList = FolderList.GetItemList();
	folderList.ResetList();

	// Json�� ���
	json::value arr;
	for (int i = 0; i < folderList.GetLength(); i++) {
		Folder target;
		folderList.GetNextItem(target);

		// folder�� count�� 0�̸� ������������.
		if (target.getCount())
		{
			web::json::value folderJson;

			folderJson[U("name")] = web::json::value::string(sTows(target.getName()));
			folderJson[U("count")] = web::json::value::number(target.getCount());

			arr[i] = folderJson;
		}
		
	}

	Json = arr;
}
void Application::showFolder(web::json::value& Json) {
	// �ֿܼ� json ���
	wcout << Json << endl;

	// ��� ����
	string name = wsTos(Json.at(U("name")).as_string());

	// FolderList���� ã��
	bool found = false;
	Folder target(name);
	FolderList.RetrieveItem(target, found);

	// json�� �ֱ�
	if (found) {
		// ã�Ҵٸ� -> ������ ���빰 response
		MySortedLinkedList<string> contentList = target.getContent().GetItemList();
		contentList.ResetList();

		// content�� ���� json-array
		web::json::value arr;

		// folder�� content<string> ������� masterList���� ã�� arr�� �ֱ�
		for (int i = 0; i < contentList.GetLength(); i++) {
			string tempName;
			contentList.GetNextItem(tempName);
			Content tempContent(tempName);
			bool contentFound = false;
			masterList.RetrieveItem(tempContent, contentFound);

			// json���� ����� arr�� ����
			json::value contentJson;
			contentJson[U("name")] = web::json::value::string(sTows(tempContent.getName()));
			contentJson[U("uploaded")] = web::json::value::string(sTows(tempContent.getUploadedAt()));
			contentJson[U("used")] = web::json::value::string(sTows(tempContent.getUsedAt()));
			contentJson[U("description")] = web::json::value::string(sTows(tempContent.getDescription()));
			contentJson[U("tags")] = web::json::value::string(sTows(tempContent.getTagsAsString()));

			arr[i] = contentJson;

		}
		Json = arr;
	}
	else {
		// ��ã�Ҵٸ� -> failed response
		web::json::value newJson;
		newJson[U("result")] = web::json::value::string(L"failed");
		Json = newJson;
	}
	
}
void Application::search(web::json::value& Json) {
	// tag�̸��� ��ġ�°ͳ��� ������ �����
	// Json ���
	wcout << Json << endl;

	// json���κ��� element ����
	string searchWord = wsTos(Json.at(U("searchWord")).as_string());

	// searchWord �Ľ� �� folder list ����
	MySortedLinkedList<Folder> fList;

	stringstream ss(searchWord);
	string temp;

	while (getline(ss, temp, ' ')) {
		Folder f(temp);
		bool found = false;
		FolderList.RetrieveItem(f, found);
		fList.Add(f);
	}

	// fList�� ������ ���ϱ�
	MySortedLinkedList<string> result;

	fList.ResetList();
	for (int i = 0; i < fList.GetLength(); i++) {
		Folder tempF;
		fList.GetNextItem(tempF);

		MySortedLinkedList<string> tempSList = tempF.getContent().GetItemList();
		tempSList.ResetList();
		for (int j = 0; j < tempSList.GetLength(); j++) {
			string tempS;
			tempSList.GetNextItem(tempS);

			result.Add(tempS);
		}
	}

	// �����չ迭�� content�� json���� response
	json::value arr;
	result.ResetList();
	for (int i = 0; i < result.GetLength(); i++) {
		string tempS;
		result.GetNextItem(tempS);

		Content target(tempS);
		bool found = false;
		masterList.RetrieveItem(target, found);

		web::json::value contentJson;

		contentJson[U("name")] = web::json::value::string(sTows(target.getName()));
		contentJson[U("uploaded")] = web::json::value::string(sTows(target.getUploadedAt()));
		contentJson[U("used")] = web::json::value::string(sTows(target.getUsedAt()));
		contentJson[U("description")] = web::json::value::string(sTows(target.getDescription()));
		contentJson[U("tags")] = web::json::value::string(sTows(target.getTagsAsString()));

		arr[i] = contentJson;
	}

	Json = arr;

}