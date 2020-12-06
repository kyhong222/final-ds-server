#pragma once

template <typename T>
class UnsortedList
{
private:
	T* m_Array;			///< list array.
	int m_Length;		///< number of elements in list.
	int m_CurPointer;	///< iterator pointer.
	int m_MaxSize;		///< max size.

public:
	UnsortedList();
	~UnsortedList();

	void MakeEmpty();
	int GetLength();
	bool IsFull();

	void ResetList();
	int GetNextItem(T& data);

	int Add(const T& inData);
	int Get(T& data);
	int Delete(T data);
	int Replace(const T& data);
};

template<typename T>
inline UnsortedList<T>::UnsortedList()
{
	m_MaxSize = 10;
	m_Length = 0;
	ResetList();
	m_Array = new T[m_MaxSize];
}

template<typename T>
inline UnsortedList<T>::~UnsortedList()
{
	delete[] m_Array;
}

template<typename T>
inline void UnsortedList<T>::MakeEmpty()
{
	m_Length = 0;
}

template<typename T>
inline int UnsortedList<T>::GetLength()
{
	return m_Length;
}

template<typename T>
inline bool UnsortedList<T>::IsFull()
{
	if (m_Length > m_MaxSize - 1)
		return true;
	else
		return false;
}

template<typename T>
inline void UnsortedList<T>::ResetList()
{
	m_CurPointer = -1;
}

template<typename T>
inline int UnsortedList<T>::GetNextItem(T& data)
{
	m_CurPointer++;
	if (m_CurPointer == m_Length)
		return -1;
	data = m_Array[m_CurPointer];

	return m_CurPointer;
}

template<typename T>
inline int UnsortedList<T>::Add(const T& inData)
{
	if (!IsFull())
	{
		m_Array[m_Length] = inData;
		m_Length++;
	}
	else
		return 0;

	return 1;
}

template<typename T>
inline int UnsortedList<T>::Get(T& data)
{
	for (int i = 0; i < m_Length; i++)
	{
		if (m_Array[i] == data)
		{
			data = m_Array[i];
			return 1;
		}
	}
	return 0;
}

template<typename T>
inline int UnsortedList<T>::Delete(T data)
{
	for (int i = 0; i < m_Length; i++)
	{
		if (m_Array[i] == data)
		{
			for (int j = i; j < m_Length - 1; j++)
			{
				m_Array[j] = m_Array[j + 1];
			}
			m_Length--;
			return 1;
		}
	}
	return 0;
}

template<typename T>
inline int UnsortedList<T>::Replace(const T& data)
{
	for (int i = 0; i < m_Length; i++)
	{
		if (m_Array[i] == data)
		{
			m_Array[i] = data;
			return 1;
		}
	}
	return 0;
}
