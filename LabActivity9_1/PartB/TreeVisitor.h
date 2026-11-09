#pragma once
#include <iostream>
using namespace std;

template <class T>
class TreeVisitor
{
public:
	virtual ~TreeVisitor() {} //destructor
	virtual void preVisit(const T& aKey) const {}
	virtual void postVisit(const T& aKey) const {}
	virtual void inVisit(const T& aKey) const {}

	//This just print out the tree node value aKey
	virtual void visit(const T& aKey) const
	{
		cout << aKey << " ";
	}
};

//Deprived classes from the visitor pattern: 
// Pre-order
template <class T>
class PreOrderVisitor : public TreeVisitor<T>
{
public:
	virtual void preVisit(const T& aKey) const
	{
		TreeVisitor<T>::visit(aKey);
	}
};

//Post-Order
template <class T>
class PostOrderVisitor : public TreeVisitor<T>
{
public:
	virtual void preVisit(const T& aKey) const
	{
		TreeVisitor<T>::visit(aKey);
	}
};


//In-Order
template<class T>
class InOrderVisitor : public TreeVisitor<T>
{
public:
	virtual void InVisit(const T& aKey) const
	{
		TreeVisitor<T>::visit(aKey);
	}
};
