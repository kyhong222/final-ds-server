#include"content.h"

Content::Content() {
	name = "";
	uploadedAt = "";
	usedAt = "";
	description = "";
}
Content::Content(string name) {
	this->name = name;
	uploadedAt = "";
	usedAt = "";
	description = "";
};
Content::Content(string name, string uploaded, string used, string description, string tags){
	this->name = name;
	this->uploadedAt = uploaded;
	this->usedAt = used;
	this->description = description;

	this->setTags(tags);
}
string Content::currentTime(){
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링

	return buf;
}