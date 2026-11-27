#pragma once
#include <cstddef>

// A singly linked list built from scratch (no std containers).
// Each node knows only the NEXT node, so the list can only be walked forward.
// The nested Iterator lets other code walk the list without knowing how it is stored.
template<class T>
class SinglyLinkedList
{
private:
	struct Node
	{
		T     fValue;
		Node* fNext;

		explicit Node(const T& aValue) noexcept : fValue(aValue), fNext(nullptr) {}
	};

	Node*  fHead;
	Node*  fTail;
	size_t fSize;

public:
	class Iterator
	{
	private:
		Node* fCurrent;
		friend class SinglyLinkedList;
		explicit Iterator(Node* aNode) noexcept : fCurrent(aNode) {}

	public:
		Iterator() noexcept : fCurrent(nullptr) {}

		T& operator*() const noexcept { return fCurrent->fValue; }
		T* operator->() const noexcept { return &fCurrent->fValue; }

		Iterator& operator++() noexcept                 // prefix: move to the next node
		{
			if (fCurrent != nullptr)
			{
				fCurrent = fCurrent->fNext;
			}
			return *this;
		}
		Iterator operator++(int) noexcept               // postfix
		{
			Iterator old = *this;
			++(*this);
			return old;
		}

		bool operator==(const Iterator& aOther) const noexcept { return fCurrent == aOther.fCurrent; }
		bool operator!=(const Iterator& aOther) const noexcept { return fCurrent != aOther.fCurrent; }
	};

	SinglyLinkedList() noexcept : fHead(nullptr), fTail(nullptr), fSize(0) {}
	~SinglyLinkedList() { clear(); }

	SinglyLinkedList(const SinglyLinkedList&) = delete;
	SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

	bool   isEmpty() const noexcept { return fSize == 0; }
	size_t size() const noexcept { return fSize; }

	void pushFront(const T& aValue)
	{
		Node* node = new Node(aValue);
		node->fNext = fHead;
		fHead = node;
		if (fTail == nullptr)
		{
			fTail = node;
		}
		fSize++;
	}

	void pushBack(const T& aValue)
	{
		Node* node = new Node(aValue);
		if (fTail == nullptr)
		{
			fHead = node;
		}
		else
		{
			fTail->fNext = node;
		}
		fTail = node;
		fSize++;
	}

	// Unlinks the first node that holds aValue. Returns false if there is none.
	bool remove(const T& aValue)
	{
		Node* previous = nullptr;
		for (Node* node = fHead; node != nullptr; node = node->fNext)
		{
			if (node->fValue == aValue)
			{
				if (previous == nullptr)
				{
					fHead = node->fNext;
				}
				else
				{
					previous->fNext = node->fNext;
				}
				if (node == fTail)
				{
					fTail = previous;
				}
				delete node;
				fSize--;
				return true;
			}
			previous = node;
		}
		return false;
	}

	void clear() noexcept
	{
		while (fHead != nullptr)
		{
			Node* next = fHead->fNext;
			delete fHead;
			fHead = next;
		}
		fTail = nullptr;
		fSize = 0;
	}

	Iterator begin() const noexcept { return Iterator(fHead); }
	Iterator end() const noexcept { return Iterator(nullptr); }
};
