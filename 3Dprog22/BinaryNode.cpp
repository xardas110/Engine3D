#include "BinaryNode.h"
#include <vector>
#include <initializer_list>
#include <iostream>
#include <stack>

template <class T>
BinaryNode<T>::BinaryNode(const T& data)
{
	m_data = data;
}

template <class T>
BinaryNode<T>::BinaryNode(const std::initializer_list<T>& list)
{
	m_data = *list.begin();

	auto begin = list.begin() + sizeof(T);
	auto end = list.end();
	for (auto beg = list.begin(); beg != end; ++beg)
		Insert(*beg);
}

template <class T>
void BinaryNode<T>::Insert(const T& val)
{
	if (!m_data)
	{
		m_data = val;
		return;
	}

	if (val < m_data)
	{
		if (m_left)
			m_left->Insert(val);
		else
			m_left = new BinaryNode<T>(val);
	}
	else
	{
		if (m_right)
			m_right->Insert(val);
		else
			m_right = new BinaryNode<T>(val);
	}
}

template <class T>
void BinaryNode<T>::Print() const
{
	std::cout << m_data << std::endl;
}

template <class T>
void BinaryNode<T>::InOrder() const
{
	if (m_left)
		m_left->InOrder();

	std::cout << m_data << std::endl;

	if (m_right)
		m_right->InOrder();
}

template <class T>
void BinaryNode<T>::PreOrder() const
{
	Print();
	if (m_left)
		m_left->PreOrder();

	if (m_right)
		m_right->PreOrder();
}

template <class T>
void BinaryNode<T>::PostOrder() const
{
	if (m_left)
		m_left->PostOrder();
	if (m_right)
		m_right->PostOrder();
	Print();
}

template <class T>
void BinaryNode<T>::InOrderIterative()
{
	std::stack<decltype(this)> stack;
	decltype(this) p = this;

	do
	{
		while (p)
		{
			stack.push(p);
			p = p->m_left;
		}
		if (!stack.empty())
		{
			std::cout << stack.top()->GetData() << std::endl;
			p = stack.top()->m_right;
			stack.pop();
		}
	} while (!stack.empty() || p != nullptr);
}

template <class T>
void BinaryNode<T>::PostOrderIterative()
{
	std::stack<decltype(this)> stack;
	std::stack<decltype(this)> post_order_stack;
	decltype(this) p = this;
	stack.push(p);

	while (!stack.empty())
	{
		p = stack.top();
		post_order_stack.push(p);
		stack.pop();

		if (p->m_left)
			stack.push(p->m_left);

		if (p->m_right)
			stack.push(p->m_right);
	}
	while (!post_order_stack.empty())
	{
		post_order_stack.top()->Print();
		post_order_stack.pop();
	}
}

template <class T>
void BinaryNode<T>::InOrderReverse() const
{
	if (m_right)
		m_right->InOrderReverse();
	std::cout << m_data << std::endl;
	if (m_left)
		m_left->InOrderReverse();
}

template <class T>
void BinaryNode<T>::InOrder(std::vector<T>& container) const
{
	if (m_left)
		m_left->InOrder(container);

	container.push_back(m_data);

	if (m_right)
		m_right->InOrder(container);
}

template <class T>
void BinaryNode<T>::InOrderReverse(std::vector<T>& container) const
{
	if (m_right)
		m_right->InOrderReverse(container);

	container.push_back(m_data);

	if (m_left)
		m_left->InOrderReverse(container);
}

template <class T>
const T& BinaryNode<T>::GetData() const
{
	return m_data;
}

template<class T>
BinaryNode<T>* BinaryNode<T>::Find(const T& val)
{
	if (val == m_data)
		return this;

	if (val < m_data)
	{
		if (m_left)
			return m_left->Find(val);
	}
	else if (val >= m_data)
	{
		if (m_right)
			return m_right->Find(val);
	}

	return nullptr;
}
