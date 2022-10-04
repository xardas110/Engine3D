#pragma once
#include <vector>
#include <initializer_list>

template<class T>
class BinaryNode
{
	using BinaryPtr = BinaryNode<T>*;
public:
	BinaryNode() = default;
	explicit BinaryNode(const T& data);
	BinaryNode(const std::initializer_list<T>& list);

	BinaryNode<T>* Find(const T& val);

	void Insert(const T& val);

	void Print() const;

	void InOrder() const;
	void PreOrder() const;
	void PostOrder() const;

	void InOrderIterative();
	void PostOrderIterative();

	void InOrderReverse() const;

	void InOrder(std::vector<T>& container) const;
	void InOrderReverse(std::vector<T>& container) const;

	const T& GetData() const;

private:

	T m_data{};
	BinaryNode<T>* m_right{ nullptr };
	BinaryNode<T>* m_left{ nullptr };
};

template class BinaryNode<int>;
template class BinaryNode<char>;
template class BinaryNode<double>;
template class BinaryNode<float>;
template class BinaryNode<const class Node*>;