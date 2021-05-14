#pragma once
#include <vector>
#include <iostream>

//We will design the Circular buffer as a class with a template. This is because parts of the code will be required to be of type int and some of type float. 
//This will create much cleaner and more efficient code. 
template <typename T>
class Circularbuffer
{
private:
	int tail; //Tail is a reader in the Queue buffer (also known as Circular buffer).
	int head; //Head is a writer.
	int position; //To read current Position of the array
	int length; //Size of the array
	std::vector<T> buffer; //An array where buffer will write information into. 

public:
	//Constructor
	Circularbuffer(int sampleSize) : length(sampleSize), tail(0), head(0), position(0), buffer(std::vector<T>(sampleSize)) {}
	//Deconstructor
	~Circularbuffer() { buffer.clear(); }

	//Displays Length of an array. Another approach that could be used would be creating a function that would check the size of the buffer directly. 
	int DisplayLength() {
		return length;
	}

	//Displays current position of the buffer. 
	int DisplayPosition(int a) {
		position = buffer[a];
		return position;
	}

	//Returns positions of the head (reader).
	int HeadPos() {
		return head;
	}

	//Returns position of the tail (writer).
	int TailPos() {
		return tail;
	}

	//Chain formula that supposed to use index notation to reference items. Code inspired by Stuart Mathews code (2020). //https://www.stuartmathews.com/index.php/91-programming/1028-this-weekend
	T DisplayPreviousPosition(int a = 0) {
		return buffer[PreviousIndex(-a)];
	}

	int PreviousIndex(int a = 0) {
		return CurrentIndexPosition(TailPos() - a, length);
	}

	int CurrentIndexPosition(int a, int size) {
		return (((a % size) + size) % size);
	}

	//Adds an item onto the buffer by writting at the current position of the Tail and then Tail is incremented by one to an empty space. 
	//Head is kept at the old position of the tail where buffer currently have written new value.
	int AddToBuffer(T a) {
		//if (tail == length) {
		//	isFull();
		//	return NULL;
		//}
		//else if (position == 0 && tail == 0) {
		//	isEmpty();
		//	return NULL;
		//}
		//else {
			buffer[HeadPos()] = a;
			tail = HeadPos();
			std::cout << a << " has been added to the array at position: " << DisplayPosition(head);
			head = (head + 1) % length;
			return tail;
		//}
	}

	//Used to manipulate vector.
	T* ToArray() { return &buffer[0]; }

	//Checks whether the buffer is full and if yes, prints to the console. 
	void isFull() {
		std::cout << "Array is full.";
	}

	//Checks whether the buffer is empty and if yes, prints it to the console. 
	void isEmpty() {
		std::cout << "Array is empty.";
	}
};

//Old code that is unused for the purpose of this task as it is a bit too complex for what is required of this coursework. 
////Code inspired from 'Embedded Software and Hardware Architecture' video available at: https://www.youtube.com/watch?v=eVW5lUx4enM
//typedef struct {
//	uint8_t* base;
//	uint8_t* head;
//	uint8_t* tail;
//	uint32_t length;
//	uint32_t count;
//}cirbuff_type;

//typedef enum{
//	No_Error = 0,
//	Not_Full,
//	Full,
//	Empty,
//	Not_Empty,
//	Null,
//}cirbuff_status;

////Checks whether buffer is null
//cirbuff_status IsNull(cirbuff_type* type)
//{
//	if (!type || !type->base || !type->head || !type->tail)
//		return Null;
//}

////Checks whether buffer is full
//cirbuff_status IsFull(cirbuff_type* type) 
//{
//	IsNull(type);

//		if (type->head == type->tail + (type->length - 1))
//			return Full;
//		else if (type->tail == type->head + 1)
//			return Full;
//		else
//			return Not_Full;
//}

////Checks whether buffer is empty
//cirbuff_status isEmpty(cirbuff_type* type) 
//{
//	IsNull(type);

//	if (type->head == type->tail)
//		return Empty;
//	else
//		return Not_Empty;
//}

////Adding element to the buffer
//cirbuff_status Add(cirbuff_type* type, int positionInBuffer) 
//{
//	IsNull(type);
//	IsFull(type);
//	
//	*type->head = positionInBuffer;
//	if (type->head == type->head + (type->length - 1))
//		type->head = type->base;
//	else
//		type->head++;
//;}