#include"application.h"

// cors 이슈를 해결하기 위해 header에 추가
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
	http_listener listener(U("http://localhost:9090"));        //Server URL, Port 지정.

	listener.support(methods::OPTIONS, handle_options);			// OPTIONS 메소드를 통해 CORS 이슈 방지
	listener.support(methods::POST, [&listener, this](http_request req) {
		/*
			1. Json을 추출
			2. Json.fnc 요소의 value를 통해 실행할 함수 결정
			3. Json을 실행할 함수에 같이 넘겨줌
			4. 함수에서 필요한 연산을 처리한 후, response를 Json에 다시 담아줌
			5. response를 client에 전송
		*/

		// 1. Json을 추출
		web::json::value Json;
		req.extract_json()       //extracts the request content into a json
			.then([&Json, this](pplx::task<json::value> task)
			{
				Json = task.get();
			}).wait();	

		// 2. Json.fnc를 추출하여 알맞은 함수 실행
		// 3, 4. Json을 reference로 넘겨주었고, 함수 내부에서 알맞은 response를 Json에 담아줌
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

		// 5. client로 전송
		http_response response(status_codes::OK);
		response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
		response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
		response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
		response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
		response.set_body(Json);

		// 콘솔에 log 출력
		cout << "response : ";
		wcout << Json << endl;
		req.reply(response);
	});


	// 서버 open
	listener.open().then([&listener]() {
		cout << "Server started at 9090" << endl;
		}).wait();    //Server open
		while (true);
		listener.close();
}

void Application::uploadContent(web::json::value& Json) {
	// 콘솔에 어떤 Json이 왔는지 로그 출력
	wcout << Json << endl;

	// Json에서 필요한 요소 추출
	string name = wsTos(Json.at(U("name")).as_string());
	string desc = wsTos(Json.at(U("description")).as_string());
	string tags = wsTos(Json.at(U("tags")).as_string());

	string uploaded = currentTime();
	string used = uploaded;

	// new Content 생성
	Content newContent(name, uploaded, used, desc, tags);

	// masterList에 추가
	masterList.Add(newContent);

	// tag에 맞게 폴더에 추가
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
	// 콘솔에 JSON 출력
	wcout << Json << endl;

	// json으로부터 name 추출
	string name = wsTos(Json.at(U("name")).as_string());

	// 삭제 준비
	bool found = false;
	Content target(name);

	masterList.RetrieveItem(target, found);

	if (found) {
		// target.tag 기반 folder를 찾고, 
		MySortedLinkedList<string> tagList = target.getTags();
		tagList.ResetList();
		for (int i = 0; i < tagList.GetLength(); i++) {
			string tag;
			tagList.GetNextItem(tag);
			bool folderFound = false;

			Folder tempFolder(tag);
			FolderList.RetrieveItem(tempFolder, folderFound);

			// folder에서 target.name을 지움
			tempFolder.Delete(name);
			
			// 수정사항 반영
			FolderList.UpdateItem(tempFolder);
		}

		// masterList에서 지워버림
		masterList.DeleteItem(target);
	}

	// create response json
	web::json::value newJson;

	string result = "OK";
	newJson[U("result")] = web::json::value::string(sTows(result));

	Json = newJson;

}
void Application::showContentDetail(web::json::value& Json) {
	// json으로부터 name 추출
	wcout << Json << endl;
	string name = wsTos(Json.at(U("name")).as_string());
	
	// masterList에서 찾기
	Content target(name);
	bool found = false;
	masterList.RetrieveItem(target, found);

	// response
	web::json::value newJson;
	if (found) {
		// 찾았다면 -> usedAt 갱신 후 content의 정보 json생성 후 response

		target.setUsedAtAsCurrent();

		newJson[U("name")] = web::json::value::string(sTows(target.getName()));
		newJson[U("uploaded")] = web::json::value::string(sTows(target.getUploadedAt()));
		newJson[U("used")] = web::json::value::string(sTows(target.getUsedAt()));
		newJson[U("description")] = web::json::value::string(sTows(target.getDescription()));
		newJson[U("tags")] = web::json::value::string(sTows(target.getTagsAsString()));

		Json = newJson;
	}
	else {
		// 못찾았다면 -> 못찾았음을 response
		newJson[U("result")] = web::json::value::string(L"failed");
	}
}
void Application::updateContent(web::json::value& Json) {
	// json으로부터 element 추출
	wcout << Json << endl;
	string name = wsTos(Json.at(U("name")).as_string());
	string desc = wsTos(Json.at(U("description")).as_string());
	string tags = wsTos(Json.at(U("tags")).as_string());

	// masterList에서 찾기
	Content target(name);
	bool found = false;
	masterList.RetrieveItem(target, found);

	// response
	web::json::value newJson;

	if (found) {
		// 찾았다면 -> usedAt 갱신 후 content 업데이트 후 ok response

		target.setUsedAtAsCurrent();

		target.setdescription(desc);

		// tag에 따라 folder 업데이트
		// 먼저 기존 tag folder에서 지워버리고
		MySortedLinkedList<string> tagList = target.getTags();
		tagList.ResetList();
		for (int i = 0; i < tagList.GetLength(); i++) {
			string tag;
			tagList.GetNextItem(tag);
			bool folderFound = false;

			Folder tempFolder(tag);
			FolderList.RetrieveItem(tempFolder, folderFound);

			// folder에서 target.name을 지움
			tempFolder.Delete(name);
			FolderList.UpdateItem(tempFolder);
		}

		// 새 tags기반으로 다시 folder에 삽입
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
		// 못찾았다면 -> failed response
		newJson[U("result")] = web::json::value::string(L"failed");
	}
}

void Application::showAllContent(web::json::value& Json) {
	// 콘솔에 JSON 출력
	wcout << Json << endl;

	//masterList를 sortedLinkedList로 변환
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
	// json을 콘솔에 출력
	wcout << Json << endl;

	// folderList를 sortedList로 변환
	MySortedLinkedList<Folder> folderList = FolderList.GetItemList();
	folderList.ResetList();

	// Json에 담기
	json::value arr;
	for (int i = 0; i < folderList.GetLength(); i++) {
		Folder target;
		folderList.GetNextItem(target);

		// folder의 count가 0이면 보여주지않음.
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
	// 콘솔에 json 출력
	wcout << Json << endl;

	// 요소 추출
	string name = wsTos(Json.at(U("name")).as_string());

	// FolderList에서 찾기
	bool found = false;
	Folder target(name);
	FolderList.RetrieveItem(target, found);

	// json에 넣기
	if (found) {
		// 찾았다면 -> 폴더의 내용물 response
		MySortedLinkedList<string> contentList = target.getContent().GetItemList();
		contentList.ResetList();

		// content를 담을 json-array
		web::json::value arr;

		// folder의 content<string> 기반으로 masterList에서 찾아 arr에 넣기
		for (int i = 0; i < contentList.GetLength(); i++) {
			string tempName;
			contentList.GetNextItem(tempName);
			Content tempContent(tempName);
			bool contentFound = false;
			masterList.RetrieveItem(tempContent, contentFound);

			// json으로 만들어 arr에 삽입
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
		// 못찾았다면 -> failed response
		web::json::value newJson;
		newJson[U("result")] = web::json::value::string(L"failed");
		Json = newJson;
	}
	
}
void Application::search(web::json::value& Json) {
	// tag이름이 겹치는것끼리 합집합 만들기
	// Json 출력
	wcout << Json << endl;

	// json으로부터 element 추출
	string searchWord = wsTos(Json.at(U("searchWord")).as_string());

	// searchWord 파싱 및 folder list 생성
	MySortedLinkedList<Folder> fList;

	stringstream ss(searchWord);
	string temp;

	while (getline(ss, temp, ' ')) {
		Folder f(temp);
		bool found = false;
		FolderList.RetrieveItem(f, found);
		fList.Add(f);
	}

	// fList의 합집합 구하기
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

	// 합집합배열의 content를 json으로 response
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