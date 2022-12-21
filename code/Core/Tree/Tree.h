/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

namespace Core
{
	//A node for a tree. The node is not the owner of its content. You need to delete the content yourself
	template <class T> class TreeNode
	{
	public:
		TreeNode(TreeNode* pParent, T* content);
		~TreeNode();

		T* GetContent();
		const T* GetContent() const;

		const std::vector<TreeNode*>& GetChildren() const;

		void AddChildren(T* child);

	private:
		T* m_pContent;

		TreeNode* m_pParent;
		std::vector<TreeNode*> m_children;
	};

	template <class T> class Tree
	{
	public:
		Tree();
		~Tree();

		TreeNode<T>* GetRoot() const;

	private:
		TreeNode<T>* m_pRoot;

	};

	template<class T> TreeNode<T>::TreeNode(TreeNode* pParent, T* pContent)
		: m_pParent(pParent)
		, m_pContent(pContent)
	{}

	template<class T> TreeNode<T>::~TreeNode()
	{
		for (TreeNode<T>* pChild : m_children)
			delete pChild;

		m_children.clear();
	}

	template<class T> T* TreeNode<T>::GetContent()
	{
		return m_pContent;
	}

	template<class T> const T* TreeNode<T>::GetContent() const
	{
		return m_pContent;
	}

	template<class T> const std::vector<TreeNode<T>*>& TreeNode<T>::GetChildren() const
	{
		return m_children;
	}

	template<class T> void TreeNode<T>::AddChildren(T* child)
	{
		TreeNode<T>* pNewNode = new TreeNode<T>(this, child);
		m_children.push_back(pNewNode);
	}

	template<class T> Tree<T>::Tree()
		: m_pRoot(new TreeNode<T>(nullptr, nullptr))
	{}

	template<class T> Tree<T>::~Tree()
	{
		delete m_pRoot;
	}

	template<class T> TreeNode<T>* Tree<T>::GetRoot() const
	{
		return m_pRoot;
	}
}
