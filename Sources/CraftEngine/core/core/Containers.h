#pragma once
#include "./Memory.h"

namespace CraftEngine
{
	namespace core
	{

		// 定长数组
		template<class Type, uint32_t Length>
		class Array
		{
		private:
			Type m_data[Length];
		public:

			const std::initializer_list<Type> operator()()
			{
				return std::initializer_list<Type>(begin(), end());
			}

			uint32_t size() const { return Length; }
			Type* data() { return m_data; }
			const Type* data() const { return m_data; };
			Type& operator[](uint32_t index) { return m_data[index]; }
			const Type& operator[](uint32_t index) const { return m_data[index]; }
			Type* begin() { return m_data; }
			const Type* begin() const { return m_data; }
			Type* end() { return m_data + Length; }
			const Type* end() const { return m_data + Length; }
		};


		template<typename Type, typename MemAllocator = MemoryAllocator<Type>>
		class ArrayList
		{
		protected:
			Type* m_data;
			int32_t m_size;
			int32_t m_capacity;

			// AlignedMemoryAllocator<Type, alignment>
			MemAllocator m_allocator;

			int32_t ExpandRule(int32_t size) { return size ? 2 * size : 1; }

			void CopyTo(Type* dest, int32_t begin, int32_t end)const
			{
				if (std::is_pod<Type>::value)
				{
					memcpy(dest, &m_data[begin], (end - begin) * sizeof(Type));
				}
				else
				{
					for (int i = begin, j = 0; i < end; i++, j++)
						new(&dest[i]) Type(m_data[j]);
				}
			}

			void CopyTo(Type* dest, int32_t count)const
			{
				if (std::is_pod<Type>::value)
				{
					memcpy(dest, m_data, count * sizeof(Type));
				}
				else
				{
					for (int i = 0; i < count; i++)
						new(&dest[i]) Type(m_data[i]);
				}
			}

			void Init()
			{
				m_capacity = 0;
				m_data = nullptr;
				m_size = 0;
			}

			void* Allocate(int32_t size) {
				if (size)
					return m_allocator.allocate(size);
				return 0;
			}

			void Deallocate() {
				if (m_data)
				{
					if (!std::is_pod<Type>::value)
						for (int i = 0; i < size(); i++)
							m_data[i].~Type();
					m_allocator.deallocate(m_data);
				}
				m_data = nullptr;
				return;
			}

			void Reallocate(int32_t nSize)
			{
				if (capacity() < nSize)
				{
					Type* s = (Type*)Allocate(nSize);
					CopyTo(s, size());
					Deallocate();
					m_data = s;
					m_capacity = nSize;
				}
			}

			int32_t capacity()const { return m_capacity; }

		public:
			ArrayList() {
				//static_assert(std::is_pod<Type>::value, "The template pamram \'Type\' must be a POD type");
				Init();
			}

			CRAFT_ENGINE_EXPLICIT ArrayList(int32_t size) {
				Init();
				resize(size);
			}

			ArrayList(const std::initializer_list<Type>& list) : ArrayList()
			{
				for (auto& it : list)
					push_back(it);
			}

			ArrayList(ArrayList<Type>&& list) : ArrayList()
			{
				swap(list);
			}

			ArrayList(const ArrayList<Type>& other) {
				Init();
				Reallocate(other.capacity());
				other.CopyTo(this->m_data, other.size());
				this->m_size = other.size();
			}

			ArrayList<Type>& operator=(const ArrayList<Type>& other)
			{
				if (this == &other)
					return *this;
				resize(0);
				if (capacity() < other.size())
					Reallocate(other.size());
				other.CopyTo(this->m_data, other.size());
				this->m_size = other.size();
				return *this;
			}

			virtual ~ArrayList() {
				Deallocate();
				Init();
			}

			void clear()
			{
				resize(0);
			}

			void resize(int32_t nSize)
			{
				if (nSize > size())
				{
					Reallocate(nSize);
					for (int i = size(); i < nSize; i++)
						new(&m_data[i]) Type();
				}
				else
				{
					for (int i = nSize; i < size(); i++)
						m_data[i].~Type();
				}
				m_size = nSize;
			}

			void resize(int32_t nSize, const Type& data)
			{
				if (nSize > size())
				{
					Reallocate(nSize);
					for (int i = size(); i < nSize; i++)
						new(&m_data[i]) Type(data);
				}
				else
				{
					for (int i = nSize; i < size(); i++)
						m_data[i].~Type();
				}
				m_size = nSize;
			}
			//void append(const ArrayList<Type>& other)
			//{
			//	int32_t this_size = other.size();
			//	int32_t other_size = other.size();
			//	resize(size() + other.size());
			//	other.CopyTo(data() + this_size, other_size);
			//}

			//void copy_to(Type* pDst, int32_t count, int32_t offset)const
			//{
			//	assert(offset + count <= m_size);
			//	CopyTo(pDst, offset, offset + count);
			//}

			void erase(int32_t count, int32_t offset)
			{
				auto start = offset;
				auto end = size();
				for (int i = offset; i < end; i++)
					m_data[i] = std::move(m_data[i + count]);
				resize(size() - count);
			}

			void erase(int32_t index)
			{
				auto end_pos = size() - 1;
				for (int i = index; i < end_pos; i++)
					m_data[i] = std::move(m_data[i + 1]);
				resize(size() - 1);
			}

			void insert(int32_t index, Type const& val)
			{
				resize(size() + 1);
				int i = m_size - 1;
				for (; i > index; i--)
					m_data[i] = m_data[i - 1];
				m_data[index] = val;
			}

			void insert(int32_t index, ArrayList<Type> const& list)
			{
				resize(size() + list.size());
				int i = size() - 1, j = 0;
				for (; i > index; i--)
					m_data[i] = m_data[i - list.size()];
				for (i = 0; i < list.size(); i++)
					m_data[i + index] = list[i];
			}

			/* 添加对象 */
			void push_back(Type const& val)
			{
				if (size() == capacity())
					Reallocate(ExpandRule(size()));
				if constexpr (std::is_pod<Type>::value)
					m_data[size()] = val;
				else
					new(&m_data[size()]) Type(val);
				m_size++;
			}

			/* 添加对象 */
			void emplace_back(Type&& val)
			{
				if (size() == capacity())
					Reallocate(ExpandRule(size()));
				new(&m_data[size()]) Type(std::move(val));
				m_size++;
			}

			/* 析构尾部对象 */
			void pop_back()
			{
				if (m_size > 0)
					if (std::is_pod<Type>::value)
						m_data[--m_size].~Type();
					else
						m_size--;
			}

			/* 交换索引 i j 对应的对象数据 */
			void swap(int32_t i, int32_t j)
			{
				Type temp = m_data[i];
				m_data[i] = m_data[j];
				m_data[j] = temp;
			}

			void swap(ArrayList<Type>& other)
			{
				auto t_capacity = other.m_capacity;
				auto t_data = other.m_data;
				auto t_size = other.m_size;
				auto t_allocator = other.m_allocator;
				other.m_capacity = this->m_capacity;
				other.m_data = this->m_data;
				other.m_size = this->m_size;
				other.m_allocator = this->m_allocator;
				this->m_capacity = t_capacity;
				this->m_data = t_data;
				this->m_size = t_size;
				this->m_allocator = t_allocator;
			}

			bool empty() const { return m_size == 0; }
			int32_t size() const { return m_size; }
			const Type& front() const { return m_data[0]; }
			const Type& back()  const { return m_data[size() - 1]; }
			Type& front() { return m_data[0]; }
			Type& back() { return m_data[size() - 1]; }
			Type* data() { return m_data; }
			const Type* data() const { return m_data; };
			Type& operator[](int32_t index) { return m_data[index]; }
			const Type& operator[](int32_t index) const { return m_data[index]; }
			Type* begin() { return m_data; }
			const Type* begin() const { return m_data; }
			Type* end() { return m_data + m_size; }
			const Type* end() const { return m_data + m_size; }

		};



		template<typename Type>
		class LinkedList
		{
		private:
			struct LLNode
			{
				LLNode* last;
				LLNode* next;
				Type data;
			};

		public:

			class Iterator
			{
			private:
				friend class LinkedList;
				LLNode* m_node;
			public:
				Iterator() :m_node(nullptr) { }
				Iterator(const Iterator& other) :m_node(other.m_node) { }
				Iterator& operator=(const Iterator& other) { m_node = (other.m_node); return *this; }
				Iterator(LLNode* pNode) :m_node(pNode) { }
				Iterator& operator++() { m_node = m_node->next; return *this; }
				Iterator& operator--() { m_node = m_node->last; return *this; }
				Iterator& operator++(int) { m_node = m_node->next; return *this; }
				Iterator& operator--(int) { m_node = m_node->last; return *this; }
				bool operator==(const Iterator& right) const { return m_node == right.m_node; }
				bool operator!=(const Iterator& right) const { return m_node != right.m_node; }
				const Type& operator*() const { return m_node->data; }
				Type& operator*() { return m_node->data; }
				const Type* operator->() const { return &m_node->data; }
				Type* operator->() { return &m_node->data; }
			};

			class ConstIterator
			{
			private:
				friend class LinkedList;
				const LLNode* m_node;
			public:
				ConstIterator(const LLNode* pNode) :m_node(pNode) { }
				ConstIterator() :m_node(nullptr) { }
				ConstIterator& operator++() { m_node = m_node->next; return *this; }
				ConstIterator& operator--() { m_node = m_node->last; return *this; }
				ConstIterator& operator++(int) { m_node = m_node->next; return *this; }
				ConstIterator& operator--(int) { m_node = m_node->last; return *this; }
				bool operator==(const ConstIterator& right) const { return m_node == right.m_node; }
				bool operator!=(const ConstIterator& right) const { return m_node != right.m_node; }
				const Type& operator*() const { return m_node->data; }
				const Type* operator->() const { return &m_node->data; }
			};

			LinkedList() :m_front(nullptr), m_back(nullptr), m_size(0)
			{

			}

			LinkedList(const std::initializer_list<Type>& ilist) :LinkedList()
			{
				for (auto& it : ilist)
					push_back(it);
			}

			LinkedList(const LinkedList& ilist) :LinkedList()
			{
				for (auto& it : ilist)
					push_back(it);
			}

			LinkedList& operator=(const LinkedList& ilist)
			{
				clear();
				for (auto& it : ilist)
					push_back(it);
				return *this;
			}

			~LinkedList()
			{
				clear();
			}

			void clear()
			{
				auto temp = m_back;
				while (temp != nullptr)
				{
					m_back = m_back->last;
					delete temp;
					temp = m_back;
				}
				m_front = nullptr;
				m_back = nullptr;
				m_size = 0;
			}
			void push_back(const Type& data)
			{
				auto nnode = new LLNode;
				nnode->next = nullptr;
				nnode->data = data;
				if (m_back == nullptr)
				{
					nnode->last = m_back;
					m_front = nnode;
					m_back = nnode;
				}
				else if (m_back == m_front)
				{
					nnode->last = m_back;
					m_back->next = nnode;
					m_front->next = nnode;
					m_back = nnode;
				}
				else
				{
					nnode->last = m_back;
					m_back->next = nnode;
					m_back = nnode;
				}
				m_size++;
			}
			void push_front(const Type& data)
			{
				auto nnode = new LLNode;
				nnode->last = nullptr;
				nnode->data = data;
				if (m_front == nullptr)
				{
					nnode->next = m_front;
					m_front = nnode;
					m_back = nnode;
				}
				else if (m_front == m_back)
				{
					nnode->next = m_front;
					m_front->last = nnode;
					m_back->last = nnode;
					m_front = nnode;
				}
				else
				{
					nnode->next = m_front;
					m_front->last = nnode;
					m_front = nnode;
				}
				m_size++;
			}
			void pop_back()
			{
				if (m_back != nullptr)
				{
					auto temp = m_back;
					m_back = m_back->last;
					m_back->next = nullptr;
					if (m_back == nullptr)
						m_front = nullptr;
					else if (m_back == m_front)
						m_front->next = nullptr;
					m_size--;
					delete temp;
				}
			}
			void pop_front()
			{
				if (m_front != nullptr)
				{
					auto temp = m_front;
					m_front = m_back->next;
					m_front->last = nullptr;
					if (m_front == nullptr)
						m_back = nullptr;
					else if (m_front == m_back)
						m_back->last = nullptr;
					m_size--;
					delete temp;
				}
			}
			void insert(const Iterator& it, const Type& data)
			{
				if (it.m_node == m_front)
					push_front(data);
				else if (it.m_node == m_back)
					push_back(data);
				else
				{
					auto nnode = new LLNode;
					nnode->last = it.m_node->last;
					nnode->next = it.m_node;
					nnode->data = data;
					nnode->last->next = nnode;
					nnode->next->last = nnode;
					m_size++;
				}
			}
			void erase(const Iterator& it)
			{
				if (it.m_node == m_front)
					pop_front();
				else if (it.m_node == m_back)
					pop_back();
				else
				{
					auto temp = it.m_node;
					temp->last->next = temp->next;
					temp->next->last = temp->last;
					m_size--;
					delete temp;
				}
			}

			bool empty() const { return m_size == 0; }
			int32_t size() const { return m_size; }
			const Type& front() const { return m_front->data; }
			const Type& back()  const { return m_back->data; }
			Type& front() { return m_front->data; }
			Type& back() { return m_back->data; }
			Iterator begin() { return Iterator(m_front); }
			ConstIterator begin() const { return ConstIterator(m_front); }
			Iterator end() { return Iterator(nullptr); }
			ConstIterator end() const { return ConstIterator(nullptr); }
		private:
			LLNode* m_front;
			LLNode* m_back;
			int32_t m_size;
		};




		template<typename Type>
		class GeneralTree
		{
		private:
			struct Node
			{
				Type             mData;
				Node*            mParent = nullptr;
				ArrayList<Node*> mChildren;
			};
			Node* m_root;
		public:

			class Iterator
			{
			private:
				friend class GeneralTree;
				Node*        m_node;
				GeneralTree* m_tree;
				Iterator(GeneralTree* tree, Node* node)
				{
					m_tree = tree;
					m_node = node;
				}
			public:
				Iterator()
				{
					m_tree = nullptr;
					m_node = nullptr;
				}

				bool        isRoot() const { return m_node->mParent == nullptr; }
				bool        isValid() const { return m_node != nullptr; }
				void        setData(const Type& val) { m_node->mData = val; }
				void        setData(Type&& val) { m_node->mData = std::move(val); }
				const Type& getData() const { if (m_node == nullptr)return *((Type*)nullptr); return m_node->mData; }
				Type&       getData() { if (m_node == nullptr)return *((Type*)nullptr); return m_node->mData; }
				size_t      getChildrenCount() const { if (m_node == nullptr)return 0; return m_node->mChildren.size(); }
				Iterator    getChild(uint32_t index) { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mChildren[index]); }
				Iterator    getParent() { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mParent); }

				size_t getIndex() const
				{
					if (isRoot())
						return -1;
					auto parent = getParent();
					for (int i = 0; i < parent.getChildrenCount(); i++)
						if (parent.getChild(i).m_node == m_node)
							return i;
					return -1;
				}

				void removeThis()
				{
					if (isRoot())
						m_tree->clear();
					else
						getParent().removeChild(getIndex());
				}

				void clearChildren()
				{
					if (!isValid())
						return;
					for (auto it : m_node->mChildren)
					{
						Iterator(m_tree, it).clearChildren();
						delete it;
					}
					m_node->mChildren.clear();
				}

				int32_t addChild(const Type& data)
				{
					if (m_node == nullptr)
						return -1;
					m_node->mChildren.push_back(new Node());
					getChild(getChildrenCount() - 1).setData(data);
					return getChildrenCount() - 1;
				}

				int32_t addChild(Type&& data)
				{
					if (m_node == nullptr)
						return -1;
					m_node->mChildren.push_back(new Node());
					getChild(getChildrenCount() - 1).setData(std::forward(data));
					return getChildrenCount() - 1;
				}

				int32_t addChild()
				{
					if (m_node == nullptr)
						return -1;
					m_node->mChildren.push_back(new Node());
					return getChildrenCount() - 1;
				}

				void removeChild(uint32_t index)
				{
					if (m_node == nullptr)
						return;
					auto temp = m_node->mChildren[index];
					m_node->mChildren.erase(m_node->mChildren.begin() + index);
					Iterator(m_tree, temp).clearChildren();
				}
			};

			class ConstIterator
			{
			private:
				friend class GeneralTree;
				const Node* m_node;
				const GeneralTree* m_tree;
				ConstIterator(const GeneralTree* tree, const Node* node)
				{
					m_tree = tree;
					m_node = node;
				}
			public:
				ConstIterator()
				{
					m_tree = nullptr;
					m_node = nullptr;
				}

				bool isRoot()const { return m_node->mParent == nullptr; }
				bool isValid()const { return m_node != nullptr; }

				const Type&   getData() const { if (m_node == nullptr)return *((Type*)nullptr); return m_node->mData; }
				size_t        getChildrenCount()const { if (m_node == nullptr)return 0; return m_node->mChildren.size(); }
				ConstIterator getChild(uint32_t index) const { if (m_node == nullptr)return ConstIterator(); return ConstIterator(m_tree, m_node->mChildren[index]); }
				Iterator      getParent() { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mParent); }

				size_t getIndex() const
				{
					if (isRoot())
						return -1;
					auto parent = getParent();
					for (int i = 0; i < parent.getChildrenCount(); i++)
						if (parent.getChild(i).m_node == m_node)
							return i;
					return -1;
				}
			};


			GeneralTree() : m_root(nullptr) { }
			~GeneralTree() { clear(); }
			Iterator createRoot() { Iterator(this, m_root = new Node); }
			ConstIterator getRoot() const { return ConstIterator(this, m_root); }
			Iterator      getRoot() { return Iterator(this, m_root); }
			bool          isEmpty() const { return m_root == nullptr; }
			void clear()
			{
				if (m_root == nullptr)
					return;
				getRoot().clearChildren();
				delete m_root;
			}
		};



		template<typename Type>
		class BinaryTree
		{
		private:
			struct Node
			{
				Type  mData;
				Node* mParent = nullptr;
				Node* mLeft = nullptr;
				Node* mRight = nullptr;
			};
			Node* m_root;
		public:

			class Iterator
			{
			private:
				friend class BinaryTree;
				Node* m_node;
				BinaryTree* m_tree;
				Iterator(BinaryTree* tree, Node* node)
				{
					m_tree = tree;
					m_node = node;
				}
			public:
				Iterator()
				{
					m_tree = nullptr;
					m_node = nullptr;
				}

				bool        isRoot() const { return m_node->mParent == nullptr; }
				bool        isValid() const { return m_node != nullptr; }
				void        setData(const Type& val) { m_node->mData = val; }
				void        setData(Type&& val) { m_node->mData = std::move(val); }
				const Type& getData() const { if (m_node == nullptr)return *((Type*)nullptr); return m_node->mData; }
				Type& getData() { if (m_node == nullptr)return *((Type*)nullptr); return m_node->mData; }
				Iterator    getRight() { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mRight); }
				Iterator    getLeft() { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mLeft); }
				Iterator    getParent() { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mParent); }
				bool        isRight() const { if (isRoot()) return false; return m_node->mParent->mRight == m_node; }
				bool        isLeft() const { if (isRoot()) return false; return m_node->mParent->mLeft == m_node; }
				bool        hasRight() const { if (!isValid()) return false; return m_node->mRight != nullptr; }
				bool        hasLeft() const { if (!isValid()) return false; return m_node->mLeft != nullptr; }

				Iterator addLeft(const Type& data)
				{
					if (m_node == nullptr)
						return -1;
					removeLeft();
					m_node->mLeft = new Node;
					m_node->mLeft->mParent = m_node;
					return Iterator(m_tree, m_node->mLeft);
				}
				void removeLeft()
				{
					if (m_node == nullptr || !hasLeft())
						return;
					getLeft().clearChildren();
					delete m_node->mLeft;
					m_node->mLeft = nullptr;
				}
				Iterator addRight(const Type& data)
				{
					if (m_node == nullptr)
						return -1;
					removeRight();
					m_node->mRight = new Node;
					m_node->mRight->mParent = m_node;
					return Iterator(m_tree, m_node->mRight);
				}
				void removeRight()
				{
					if (m_node == nullptr || !hasRight())
						return;
					getRight().clearChildren();
					delete m_node->mRight;
					m_node->mRight = nullptr;
				}
				void clearChildren()
				{
					removeLeft();
					removeRight();
				}
				void removeThis()
				{
					if (isRoot())
						m_tree->clear();
					else if (isLeft())
						getParent().removeLeft();
					else
						getParent().removeRight();
				}
			};

			class ConstIterator
			{
			private:
				friend class BinaryTree;
				const Node* m_node;
				const BinaryTree* m_tree;
				ConstIterator(const BinaryTree* tree, const Node* node)
				{
					m_tree = tree;
					m_node = node;
				}
			public:
				ConstIterator()
				{
					m_tree = nullptr;
					m_node = nullptr;
				}

				bool        isRoot() const { return m_node->mParent == nullptr; }
				bool        isValid() const { return m_node != nullptr; }
				const Type& getData() const { if (m_node == nullptr)return *((Type*)nullptr); return m_node->mData; }
				ConstIterator    getRight() { if (m_node == nullptr)return Iterator(); return Iterator(m_tree, m_node->mRight); }
				ConstIterator    getLeft() { if (m_node == nullptr)return ConstIterator(); return ConstIterator(m_tree, m_node->mLeft); }
				ConstIterator    getParent() { if (m_node == nullptr)return ConstIterator(); return ConstIterator(m_tree, m_node->mParent); }
				bool        isRight() const { if (isRoot()) return false; return m_node->mParent->mRight == m_node; }
				bool        isLeft() const { if (isRoot()) return false; return m_node->mParent->mLeft == m_node; }
				bool        hasRight() const { if (!isValid()) return false; return m_node->mRight != nullptr; }
				bool        hasLeft() const { if (!isValid()) return false; return m_node->mLeft != nullptr; }
			};


			BinaryTree() : m_root(nullptr) { }
			~BinaryTree() { clear(); }
			Iterator      createRoot() { Iterator(this, m_root = new Node); }
			ConstIterator getRoot() const { return ConstIterator(this, m_root); }
			Iterator      getRoot() { return Iterator(this, m_root); }
			bool          isEmpty() const { return m_root == nullptr; }
			void clear()
			{
				if (m_root = nullptr)
					return;
				getRoot().clearChildren();
				delete m_root;
			}
		};




		template<typename KeyType, typename DataType>
		class RBTree
		{
		private:
			enum NodeColor
			{
				RBTREE_BLACK_NODE = 0,
				RBTREE_RED_NODE = 1
			};

			struct Node
			{
				Node* mLeft;
				Node* mRight;
				Node* mParent;
				NodeColor mColor;
				KeyType mKey;
				DataType mData;
			};

			Node* m_root;
			uint32_t m_size;

			bool isRed(Node* node) { return node->mColor == RBTREE_RED_NODE; }
			bool isBlack(Node* node) { return node->mColor == RBTREE_BLACK_NODE; }
			bool hasLeftChild(Node* node) { return node->mLeft != nullptr; }
			bool hasRightChild(Node* node) { return node->mRight != nullptr; }
			void setRed(Node* node) { node->mColor = RBTREE_RED_NODE; }
			void setBlack(Node* node) { node->mColor = RBTREE_BLACK_NODE; }


			template<typename ComparableKeyType>
			Node* FindNode(ComparableKeyType key)
			{
				Node* node = m_root;
				while (node != nullptr)
				{
					if (node->mKey == key)
						return node;
					else if (node->mKey > key)
						node = node->mLeft;
					else
						node = node->mRight;
				}
				return nullptr;
			}

			static Node* FindSuccessorNode(const Node* node)
			{
				if (node->mRight == nullptr)
				{
					Node* n = node;
					Node* p = node->mParent;
					while (p != nullptr)
						if (n == p->mLeft)
							return p;
						else
							p = (n = p)->mParent;
					return nullptr;
				}
				else
				{
					Node* r = node->mRight;
					while (r->mLeft != nullptr)
						r = r->mLeft;
					return r;
				}
			}

			static void ClearNode(Node* node)
			{
				if (node != nullptr)
				{
					ClearNode(node->mLeft);
					ClearNode(node->mRight);
					delete node;
				}
			}

			void RotateL(Node* node, bool left = true)
			{
				Node* node_p = node->mParent;
				if (left)
				{
					if (node_p == nullptr)
						m_root = node->mRight;
					else
						if (node == node_p->mLeft)
							node_p->mLeft = node->mRight;
						else
							node_p->mRight = node->mRight;
					node->mRight->mParent = node_p;
					node->mParent = node->mRight;
					if (node->mRight->mLeft != nullptr)
						node->mRight->mLeft->mParent = node;
					node->mRight = node->mRight->mLeft;
					node->mParent->mLeft = node;
				}
				else
				{
					if (node_p == nullptr)
						m_root = node->mLeft;
					else
						if (node == node_p->mLeft)
							node_p->mLeft = node->mLeft;
						else
							node_p->mRight = node->mLeft;
					node->mLeft->mParent = node_p;
					node->mParent = node->mLeft;
					if (node->mLeft->mRight != nullptr)
						node->mLeft->mRight->mParent = node;
					node->mLeft = node->mLeft->mRight;
					node->mParent->mRight = node;
				}
			}

		public:

			RBTree()
			{
				static_assert(std::is_pod<DataType>::value, "The template pamram \'DataType\' must be a POD type");
				static_assert(std::is_pod<KeyType>::value, "The template pamram \'KeyType\' must be a POD type");
				m_root = nullptr;
			}

			~RBTree()
			{
				clear();
			}

			template<typename ComparableKeyType>
			DataType* find(const ComparableKeyType& key)
			{
				auto node = FindNode(key);
				if (node != nullptr)
					return &node->mData;
				else
					return nullptr;
			}

			bool insert(const KeyType& key, const DataType& data)
			{
				Node* temp = m_root;
				Node* node_p = nullptr;

				Node* node_gp;
				Node* node_u;
				bool logic_a;
				bool logic_b;

				while (temp != nullptr)
				{
					node_p = temp;
					if (key > temp->mKey)
						temp = temp->mRight;
					else if (key < temp->mKey)
						temp = temp->mLeft;
					else
						return false;
				}

				Node* node = new Node;
				node->mKey = key;
				node->mParent = node_p;
				node->mColor = RBTREE_RED_NODE;
				node->mLeft = nullptr;
				node->mRight = nullptr;
				node->mData = data;

				m_size++;
				// 空树
				if (node_p == nullptr)
				{
					node->mColor = RBTREE_BLACK_NODE;
					m_root = node;
					return true;
				}

				if (node->mKey > node_p->mKey)
					node_p->mRight = node;
				else
					node_p->mLeft = node;

				while (1)
				{
					if (isBlack(node_p))
					{
						if (node->mKey > node_p->mKey)
							node_p->mRight = node;
						else
							node_p->mLeft = node;
						break;
					}
					else
					{
						node_gp = node_p->mParent;
						node_u = nullptr;
						node_u = node_gp->mRight == node_p ? node_gp->mLeft : node_gp->mRight;
						logic_a = node_gp->mRight == node_p;
						logic_b = node_p->mRight == node;
						if (node_u == nullptr || isBlack(node_u))
						{
							if (logic_a ^ logic_b)
							{
								setRed(node_gp);
								setBlack(node);
								RotateL(node_p, !logic_a);
								RotateL(node_gp, logic_a);
							}
							else
							{
								setBlack(node_p);
								setRed(node_gp);
								RotateL(node_gp, logic_a);
							}
						}
						else
						{
							setBlack(node_p);
							setBlack(node_u);
							setRed(node_gp);
							node = node_gp;
							node_p = node_gp->mParent;
							if (node_p == nullptr)
							{
								setBlack(node_gp);
								break;
							}
						}
					}
				}
				return true;
			}


			bool remove(const KeyType& key)
			{
				Node* node_rm = FindNode(key);
				Node* node_rp = node_rm;
				Node* node_p; // 父节点
				Node* node_b; // 兄弟节点
				bool logic_a;

				if (node_rm == nullptr)
					return false;

				// 替换节点
				while (node_rp->mLeft != nullptr || node_rp->mRight != nullptr)
				{
					if ((node_rp->mLeft == nullptr) ^ (node_rp->mRight == nullptr))
						node_rm = node_rp->mRight != nullptr ? node_rp->mRight : node_rp->mLeft;
					else
						node_rm = FindSuccessorNode(node_rp);
					node_rp->mKey = node_rm->mKey;//
					node_rp->mData = node_rm->mData;//
					node_rp = node_rm;
				}

				// 平衡红黑树
				while (1)
				{
					if (isRed(node_rp))
					{
						setBlack(node_rp);
						break;
					}
					else
					{
						node_p = node_rp->mParent;
						if (node_p != nullptr)
						{
							logic_a = (node_p->mRight == node_rp) ? true : false;
							node_b = logic_a ? node_p->mLeft : node_p->mRight;
						}
						else
							break;

						if (isRed(node_b))
						{
							setBlack(node_b);
							setRed(node_p);
							RotateL(node_p, !logic_a);
						}
						else
						{
							if (logic_a ? (hasLeftChild(node_b) && isRed(node_b->mLeft)) : (hasRightChild(node_b) && isRed(node_b->mRight)))
							{
								node_b->mColor = node_p->mColor;
								setBlack(node_p);
								setBlack(node_b->mLeft);
								RotateL(node_p, !logic_a);
								break;
							}
							else if (
								(hasLeftChild(node_b) && hasRightChild(node_b) && isBlack(node_b->mLeft) && isBlack(node_b->mRight)) ||
								(!hasLeftChild(node_b) && !hasRightChild(node_b))
								)
							{
								setRed(node_b);
								node_rp = node_p;
							}
							else
							{
								setRed(node_b);
								setBlack(node_b->mRight);
								RotateL(node_b, logic_a);
							}
						}
					}
				}

				// 删除节点
				if (node_rm->mParent != nullptr)
					if (node_rm->mParent->mLeft == node_rm)
						node_rm->mParent->mLeft = nullptr;
					else
						node_rm->mParent->mRight = nullptr;
				else
					m_root = nullptr;

				delete node_rm;
			}

			bool contain(const KeyType& key)
			{
				auto node = FindNode(key);
				if (node == nullptr)
					return false;
				else
					return true;
			}

			void clear()
			{
				ClearNode(m_root);
				m_root = nullptr;
			}


			class Iterator
			{
			private:
				friend class RBTree;
				Node* m_node;
				Iterator(Node* node) :m_node(node) {};
			public:
				Iterator() :m_node(nullptr) {};
				bool operator==(const Iterator& other) { return this->m_node == other.m_node; }
				bool operator!=(const Iterator& other) { return this->m_node != other.m_node; }
				Iterator operator++() { m_node = FindSuccessorNode(this->m_node); return *this; }
				Iterator operator++(int) { m_node = FindSuccessorNode(this->m_node); return *this; }
				DataType& operator*() { return m_node->mData; }
				const DataType& operator*()const { return m_node->mData; }
			};

			class ConstIterator
			{
			private:
				friend class RBTree;
				const Node* m_node;
				ConstIterator(const Node* node) :m_node(node) {};
			public:
				ConstIterator() :m_node(nullptr) {};
				bool operator==(const ConstIterator& other) { return this->m_node == other.m_node; }
				bool operator!=(const ConstIterator& other) { return this->m_node != other.m_node; }
				ConstIterator operator++() { m_node = FindSuccessorNode(this->m_node); return *this; }
				ConstIterator operator++(int) { m_node = FindSuccessorNode(this->m_node); return *this; }
				const DataType& operator*()const { return m_node->mData; }
			};

			Iterator begin() {
				Node* res = m_root;
				if (res != nullptr)
					while (res->mLeft != nullptr)
						res = res->mLeft;
				return Iterator(res);
			}

			Iterator end() {
				return Iterator(nullptr);
			}

			ConstIterator begin() {
				const Node* res = m_root;
				if (res != nullptr)
					while (res->mLeft != nullptr)
						res = res->mLeft;
				return ConstIterator(res);
			}

			ConstIterator end() {
				return ConstIterator(nullptr);
			}
		};




		template<typename Type, uint32_t Length>
		class ObjectPool
		{
		private:
			Array<Type, Length> m_data;
			Array<Type*, Length> m_slots;
			Array<bool, Length> m_vaild;

			uint32_t   m_allocatedCount;
			std::mutex m_mutex;

			bool contain(Type* address) { return address >= m_data.begin() && address < m_data.end(); }
		public:

			struct AddressSpace
			{
				const void* begin;
				const void* end;

				bool operator<(const AddressSpace& other)const { return begin < other.begin; }
				bool operator>(const AddressSpace& other)const { return begin > other.begin; }
				bool operator==(const AddressSpace& other)const {
					return begin == other.begin && end == other.end;
				}

				bool operator<(const void* other)const { return begin < other; }
				bool operator>(const void* other)const { return end >= other; }
				bool operator==(const void* other)const {
					return other >= begin && other < end;
				}
			};

			ObjectPool()
			{
				//static_assert(std::is_pod<Type>::value, "the template pamram \'Type\' must be a POD type");
				init();
			}

			void init()
			{
				m_allocatedCount = 0;
				for (int i = 0; i < Length; i++)
				{
					m_vaild[i] = false;
					m_slots[i] = &m_data[i];
				}
			}

			Type* alloc_unsafe()
			{
				Type* address = nullptr;
				if (m_allocatedCount < Length)
				{
					address = m_slots[m_allocatedCount++];
					m_vaild[index_of(address)] = true;
				}
				else
				{
					throw std::runtime_error("Bad memory allocate, pool is full!");
				}
				return address;
			}

			Type* alloc()
			{
				Type* address = nullptr;
				std::lock_guard<std::mutex> lock(m_mutex);
				if (m_allocatedCount < Length)
				{
					address = m_slots[m_allocatedCount++];
					m_vaild[index_of(address)] = true;
				}
				else
				{
					throw std::runtime_error("Bad memory allocate, pool is full!");
				}
				return address;
			}


			Type* store_unsafe(const Type& elem)
			{
				auto address = alloc_unsafe();
				*address = elem;
				return address;
			}

			Type* store(const Type& elem)
			{
				auto address = alloc();
				*address = elem;
				return address;
			}

			Type* store_unsafe(Type&& elem)
			{
				auto address = alloc_unsafe();
				*address = std::move(elem);
				return address;
			}

			Type* store(Type&& elem)
			{
				auto address = alloc();
				*address = std::move(elem);
				return address;
			}

			void free_unsafe(Type* address)
			{
				uint32_t index = index_of(address);
				if (contain(address) && (m_vaild[index] == true))
				{
					(void*)address;
					m_vaild[index] = false;
					m_slots[--m_allocatedCount] = address;
					//memset(address, 0, sizeof(Type));
				}
				else if (m_vaild[index] == false)
				{
					throw std::runtime_error("Bad memory release, address is invalid!");
				}
				else
				{
					throw std::runtime_error("Bad memory release, address is out of range!");
				}
			}

			void free(Type* address)
			{
				uint32_t index = index_of(address);
				std::lock_guard<std::mutex> lock(m_mutex);
				if (contain(address) && (m_vaild[index] == true))
				{
					(void*)address;
					m_vaild[index] = false;
					m_slots[--m_allocatedCount] = address;
					//memset(address, 0, sizeof(Type));
				}
				else if (m_vaild[index] == false)
				{
					throw std::runtime_error("Bad memory release, address is invalid!");
				}
				else
				{
					throw std::runtime_error("Bad memory release, address is out of range!");
				}
			}

			int32_t index_of(Type* address) const { return address - m_data.data(); }
			AddressSpace address_space() { return { m_data.begin(), m_data.end() }; }
			uint32_t capacity() const { return Length - m_allocatedCount; }
			uint32_t count() const { return m_allocatedCount; }
			bool is_full()const { return count() == capacity(); }
		};


		template<typename Type, uint32_t Length>
		class MultiObjectPool
		{
		private:
			typedef ObjectPool<Type, Length>                        base_pool;
			typedef ObjectPool<Type, Length>* base_pool_pointer;
			typedef typename ObjectPool<Type, Length>::AddressSpace base_pool_address_space;

			RBTree<base_pool_address_space, base_pool_pointer> m_allocatedPool;
			ArrayList<base_pool_pointer>                       m_vaildPool;
			AlignedMemoryAllocator<base_pool, 16>              m_allocator;

			std::mutex m_mutex;
		public:

			MultiObjectPool()
			{
				static_assert(std::is_pod<Type>::value, "the template pamram \'Type\' must be a POD type");
			}

			~MultiObjectPool()
			{
				clear();
			}

			void clear()
			{
				for (auto pool : m_allocatedPool)
					m_allocator.deallocate(pool);
				m_allocatedPool.clear();
			}

			Type* alloc_unsafe()
			{
				if (m_vaildPool.size() == 0)
				{
					auto pool = m_allocator.allocate(1);
					new(pool) base_pool();
					m_allocatedPool.insert(pool->address_space(), pool);
					m_vaildPool.push_back(pool);
				}
				auto address = (*(m_vaildPool.end() - 1))->alloc_unsafe();
				if ((*(m_vaildPool.end() - 1))->is_full())
					m_vaildPool.pop_back();
				return address;
			}

			void free_unsafe(Type* address)
			{
				base_pool_pointer pool = *m_allocatedPool.find(address);
				pool->free_unsafe(address);
				if (pool->count() == pool->capacity() - 1)
					m_vaildPool.push_back(pool);
			}

			Type* alloc()
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (m_vaildPool.size() == 0)
				{
					auto pool = m_allocator.allocate(1);
					new(pool) base_pool();
					m_allocatedPool.insert(pool->address_space(), pool);
					m_vaildPool.push_back(pool);
				}
				auto address = (*(m_vaildPool.end() - 1))->alloc_unsafe();
				if ((*(m_vaildPool.end() - 1))->is_full())
					m_vaildPool.pop_back();
				return address;
			}

			void free(Type* address)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				base_pool_pointer pool = *m_allocatedPool.find(address);
				pool->free_unsafe(address);
				if (pool->count() == pool->capacity() - 1)
					m_vaildPool.push_back(pool);
			}

		};




	}
}