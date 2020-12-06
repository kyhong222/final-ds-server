#pragma once

#include <iostream>
using namespace std;

template<typename T>
struct NodeType {
	T data;
	NodeType* next;
};

template<typename T>
class MySortedLinkedList {
public:
	MySortedLinkedList();
	~MySortedLinkedList();

	void MakeEmpty();
	int GetLength() { return size; }

	int Add(T item);
	int Delete(T item);
	int Replace(T item);
	int Get(T& item);

	void ResetList();
	void GetNextItem(T& item);

private:
	NodeType<T>* head;
	NodeType<T>* current;
	int size;
};

template<typename T>
MySortedLinkedList<T>::MySortedLinkedList() {
	head = nullptr;
	current = nullptr;
	size = 0;
}

template<typename T>
MySortedLinkedList<T>::~MySortedLinkedList() {
	/*cout << size << endl;
	cout << head << endl;
	MakeEmpty();*/
}

template<typename T>
void MySortedLinkedList<T>::MakeEmpty() {
	NodeType<T>* temp;

	while (head != nullptr) {
		temp = head;
		head = head->next;
		delete temp;
	}

	size = 0;
}

template<typename T>
int MySortedLinkedList<T>::Add(T item) {
	// reset list
	ResetList();

	NodeType<T>* target = new NodeType<T>;
	NodeType<T>* pre;

	T temp;	// is temp object to use GetNextItem()

	target->data = item;
	target->next = nullptr;

	// if size == 0, then target is head
	if (!size) {
		head = target;
		size++;
		return 1;
	}

	while (true) {
		// set pre-node
		pre = current;

		// get next thing
		GetNextItem(temp);

		// if there is a same item, then do not add.
		if (temp == target->data) {
			//cout << "there is same item." << endl;
			return 0;
		}

		// if current data is bigger then target data
		if (current->data > target->data) {
			// if head is bigger then target data, then add item before head
			if (pre == nullptr) {
				target->next = current;
				head = target;
				break;
			}

			// add item after current
			pre->next = target;
			target->next = current;
			break;
		}

		// if there is no item that bigger then target, add it on tail
		if (current->next == nullptr) {
			current->next = target;
			break;
		}
	}

	// increase size
	size++;
	return 1;
}
template<typename T>
int MySortedLinkedList<T>::Delete(T item) {
	// reset list
	ResetList();

	NodeType<T>* pre;
	T temp;	// it is used to use GetNextItem()

	while (true) {
		// set pre-node
		pre = current;

		// get next item
		GetNextItem(temp);

		// if found
		if (temp == item) {
			// if it is head, then set head as next
			if (pre == nullptr) {
				head = current->next;
			}
			// else, then set pre's next as current's next
			else {
				pre->next = current->next;
			}

			// delete current
			delete current;
			return 1;
		}

		// if not found
		if (current == nullptr) {
			cout << "there is no item like that." << endl;
			return 0;
		}
	}
}
template<typename T>
int MySortedLinkedList<T>::Replace(T item) {
	// reset list
	ResetList();

	T temp;	// it is used to use GetNextItem()

	while (true) {
		// get next item
		GetNextItem(temp);

		// if found
		if (temp == item) {
			// replace it
			current->data = item;
			return 1;
		}

		// if not found
		if (current == nullptr) {
			cout << "there is no item like that." << endl;
			return 0;
		}
	}
}
template<typename T>
int MySortedLinkedList<T>::Get(T& item) {
	// reset list
	ResetList();

	T temp;	// it is used to use GetNextItem()

	for (int i = 0; i < size; i++) {
		// get next item
		GetNextItem(temp);

		// if found
		if (temp == item) {
			// get it
			item = temp;
			return 1;
		}
	}

	// if not found
	if (current == nullptr) {
		//cout << "there is no item like that." << endl;
		return 0;
	}
	return 0;
}

template<typename T>
void MySortedLinkedList<T>::ResetList() {
	current = nullptr;
}

template<typename T>
void MySortedLinkedList<T>::GetNextItem(T& item) {
	if (current == nullptr) {
		current = head;
	}
	else {
		current = current->next;
	}

	item = current->data;
}