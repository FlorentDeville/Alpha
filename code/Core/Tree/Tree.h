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
		TreeNode(TreeNode* pParent, const T& content);
		~TreeNode();

		T& GetContent();
		const T& GetContent() const;

		void SetContent(T& content);

		const std::vector<TreeNode*>& GetChildren() const;

		const TreeNode<T>* GetParent() const;
		TreeNode<T>* GetParent();

		TreeNode<T>* AddChildren(T& child);
		void DeleteChild(T& child);

	private:
		T m_content;

		TreeNode* m_pParent;
		std::vector<TreeNode*> m_children;
	};

	template <class T> class Tree
	{
	public:
		Tree();
		~Tree();

		TreeNode<T>* GetRoot() const;

		void clear();

	private:
		TreeNode<T>* m_pRoot;

	};

	template<class T> TreeNode<T>::TreeNode(TreeNode* pParent, const T& content)
		: m_pParent(pParent)
		, m_content(content)
	{}

	template<class T> TreeNode<T>::~TreeNode()
	{
		for (TreeNode<T>* pChild : m_children)
			delete pChild;

		m_children.clear();
	}

	template<class T> T& TreeNode<T>::GetContent()
	{
		return m_content;
	}

	template<class T> const T& TreeNode<T>::GetContent() const
	{
		return m_content;
	}

	template<class T> void TreeNode<T>::SetContent(T& content)
	{
		m_content = content;
	}


	template<class T> const std::vector<TreeNode<T>*>& TreeNode<T>::GetChildren() const
	{
		return m_children;
	}

	template<class T> const TreeNode<T>* TreeNode<T>::GetParent() const
	{
		return m_pParent;
	}

	template<class T> TreeNode<T>* TreeNode<T>::GetParent()
	{
		return m_pParent;
	}

	template<class T> TreeNode<T>* TreeNode<T>::AddChildren(T& child)
	{
		TreeNode<T>* pNewNode = new TreeNode<T>(this, child);
		m_children.push_back(pNewNode);

		return pNewNode;
	}

	template<class T> void TreeNode<T>::DeleteChild(T& child)
	{
		typename std::vector<TreeNode<T>*>::const_iterator it = std::find_if(m_children.begin(), m_children.end(), [child](TreeNode<T>* pNode) { return pNode->GetContent() == child; });

		if (it == m_children.end())
			return;

		TreeNode<T>* pEntity = (*it);
		delete pEntity;

		m_children.erase(it);	
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

	template<class T> void Tree<T>::clear()
	{
		delete m_pRoot;
		m_pRoot = new TreeNode<T>(nullptr, nullptr);
	}
}
