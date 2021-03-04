#pragma once
#include "./Common.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace model_view
		{


			class AbstractItemModel;

			struct ModelIndex
			{
			private:
				const AbstractItemModel* m_model;
				union
				{
					int64_t id;
					void* ptr;
				}m_index;
				int m_row;
				int m_col;
			public:
				ModelIndex()
					:m_row(-1), m_col(-1), m_model(nullptr) {
					m_index.id = -1;
				}
				ModelIndex(int row, int col, int id, const AbstractItemModel* pmodel)
					:m_row(row), m_col(col), m_model(pmodel) {
					m_index.id = id;
				}
				ModelIndex(int row, int col, void* ptr, const AbstractItemModel* pmodel)
					:m_row(row), m_col(col), m_model(pmodel) {
					m_index.ptr = ptr;
				}
				const AbstractItemModel* getItemModel()const { return m_model; }
				int       getRow()const { return m_row; }
				int       getCol()const { return m_col; }
				int       getID()const { return m_index.id; }
				void*     getPointer()const { return m_index.ptr; }
				bool      isValid()const { return m_row >= 0 && m_col >= 0 && m_model != nullptr; }
				ItemData  getData(int role)const;

				ModelIndex getParentIndex() const;
				ModelIndex getChildIndex(int row, int column) const;
				ModelIndex getSiblingIndex(int row, int column) const;
				ModelIndex getSiblingIndexAtColumn(int column) const;
				ModelIndex getSiblingIndexAtRow(int row) const;
				int getChildrenCount() const;

				bool operator==(const ModelIndex& other)const { return other.m_row == m_row && other.m_col == m_col && m_index.id == other.m_index.id && other.m_model == m_model; }
				bool operator!=(const ModelIndex& other)const { return !(*this == other); }
			};



			class AbstractItemModel
			{
			private:
				friend class AbstractItemView;

			public:
				enum ModelFlag
				{
					eNotAllowDestroied = 0x1,
				};

				AbstractItemModel() {}
				~AbstractItemModel()
				{
					if (referenceCount() != 0)
					{
						notifyDestroied(); ///
						//craft_engine_error_v2("There is(are) still view(s) using this model. Call [notifyDestroied()] before deleting this model.");
					}
				}

				uint32_t getModelFlags()const { return m_modelFlags; }
				void enableModelFlagBits(uint32_t bits) { m_modelFlags |= bits; }
				void disableModelFlagBits(uint32_t bits) { m_modelFlags &= (~bits); }
				/*
				 lock the model so that any modification would not emit signal.
				*/
				void setModelSignalLock(bool enable) { m_isModelSignalLocked = enable; }
				bool isModelSignalLocked()const { return m_isModelSignalLocked; }

				/*
				 Call this before deleting.
				*/
				void notifyDestroied()
				{
					enableModelFlagBits(eNotAllowDestroied);
					craft_engine_gui_emit(modelDeleted, this);
					disableModelFlagBits(eNotAllowDestroied);
				}
				void notifyUpdated()
				{
					craft_engine_gui_emit(modelUpdated, this);
				}
				void notifyUpdated(const ModelIndex& index)
				{
					craft_engine_gui_emit(itemUpdated, index);
				}

				virtual ModelIndex getModelIndex(int row, int col, const ModelIndex& parent = ModelIndex())const = 0;
				virtual ModelIndex getParentModelIndex(const ModelIndex& child)const = 0;
				virtual ModelIndex getSiblingModelIndex(int row, int column, const ModelIndex& index)const = 0;
				//virtual ModelIndex getChildModelIndex(const ModelIndex& parent, int index)const = 0;
				virtual bool       hasChildren(const ModelIndex& index)const = 0;
				virtual int        getChildrenCount(const ModelIndex& index)const = 0;

				virtual int getRowCount()const = 0;
				virtual int getColCount()const = 0;
				//virtual int getRowItemCount(int row)const { return getColCount(); }
				//virtual int getColItemCount(int col)const { return getRowCount(); }

				virtual ItemDescription getItemDescription()const = 0;

				/*
				 obtain item data
				*/
				inline ItemData getItemData(const ModelIndex& index, int role)const
				{
					return onGetItemData(index, role);
				}
				/*
				 manipulate item data by model
				*/
				inline void setItemData(const ModelIndex& index, const ItemData& data, int role)
				{
					onSetItemData(index, data, role);
					if (!isModelSignalLocked())
						notifyUpdated(index);
				}
				/*
				*insertRows
				 1.Param <row> must be in the range of [0, rowCount]
				 2.After inserting, having: rowCount' -> rowCount + count
				*/
				inline bool insertRows(int row, int count, const ModelIndex& parent = ModelIndex())
				{
					if (onInsertRows(row, count, parent)) { if (!isModelSignalLocked())craft_engine_gui_emit(rowsInserted, row, count, parent); return true; }
					else return false;
				};
				inline bool insertCols(int col, int count, const ModelIndex& parent = ModelIndex())
				{
					if (onInsertCols(col, count, parent)) { if (!isModelSignalLocked())craft_engine_gui_emit(colsInserted, col, count, parent); return true; }
					else return false;
				};
				inline bool removeRows(int row, int count, const ModelIndex& parent = ModelIndex())
				{
					if (onRemoveRows(row, count, parent)) { if (!isModelSignalLocked())craft_engine_gui_emit(rowsRemoved, row, count, parent); return true; }
					else return false;
				};
				inline bool removeCols(int col, int count, const ModelIndex& parent = ModelIndex())
				{
					if (onRemoveCols(col, count, parent)) { if (!isModelSignalLocked())craft_engine_gui_emit(colsRemoved, col, count, parent); return true; }
					else return false;
				};
				inline bool moveRows(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count)
				{
					if (onMoveRows(sourceParent, sourceRow, destinationParent, destinationRow, count)){
						if (!isModelSignalLocked()) craft_engine_gui_emit(rowsMoved, sourceParent, sourceRow, destinationParent, destinationRow, count);
						return true;
					}
					else return false;
				}
				inline bool moveCols(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count)
				{
					if (onMoveCols(sourceParent, sourceCol, destinationParent, destinationCol, count)){
						if (!isModelSignalLocked()) craft_engine_gui_emit(rowsMoved, sourceParent, sourceCol, destinationParent, destinationCol, count);
						return true;
					}
					else return false;
				}
				inline bool insertRow(int row, const ModelIndex& parent = ModelIndex()) { return insertRows(row, 1, parent); };
				inline bool insertCol(int col, const ModelIndex& parent = ModelIndex()) { return insertCols(col, 1, parent); };
				inline bool removeRow(int row, const ModelIndex& parent = ModelIndex()) { return removeRows(row, 1, parent); };
				inline bool removeCol(int col, const ModelIndex& parent = ModelIndex()) { return removeCols(col, 1, parent); };
				inline bool moveRow(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow) { return moveRows(sourceParent, sourceRow, destinationParent, destinationRow, 1); };
				inline bool moveCol(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol) { return moveCols(sourceParent, sourceCol, destinationParent, destinationCol, 1); };
			protected:
				virtual ItemData onGetItemData(const ModelIndex& index, int role)const = 0;
				virtual void     onSetItemData(const ModelIndex& index, const ItemData& data, int role) = 0;
				virtual bool     onInsertRows(int row, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual bool     onInsertCols(int col, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual bool     onRemoveRows(int row, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual bool     onRemoveCols(int col, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual bool     onMoveRows(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) = 0;
				virtual bool     onMoveCols(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) = 0;
			private:
				craft_engine_gui_signal(modelUpdated, void(AbstractItemModel* model));
				craft_engine_gui_signal(modelDeleted, void(AbstractItemModel* model));
				craft_engine_gui_signal(itemUpdated, void(const ModelIndex& index));
				craft_engine_gui_signal(rowsInserted, void(int row, int count, const ModelIndex& parent));
				craft_engine_gui_signal(colsInserted, void(int col, int count, const ModelIndex& parent));
				craft_engine_gui_signal(rowsRemoved, void(int row, int count, const ModelIndex& parent));
				craft_engine_gui_signal(colsRemoved, void(int col, int count, const ModelIndex& parent));
				craft_engine_gui_signal(rowsMoved, void(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count));
				craft_engine_gui_signal(colsMoved, void(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count));
			private:

				inline void setItemData_NoSignal(const ModelIndex& index, const ItemData& data, int role)
				{
					onSetItemData(index, data, role);
				}
				void referenceIncrease() { m_referenceCount++; }
				void referenceDecrease(uint32_t key)
				{
					m_referenceCount--;
					craft_engine_gui_disconnect(this, modelUpdated, key);
					craft_engine_gui_disconnect(this, modelDeleted, key);
					craft_engine_gui_disconnect(this, itemUpdated, key);
					craft_engine_gui_disconnect(this, rowsInserted, key);
					craft_engine_gui_disconnect(this, colsInserted, key);
					craft_engine_gui_disconnect(this, rowsRemoved, key);
					craft_engine_gui_disconnect(this, colsRemoved, key);
					craft_engine_gui_disconnect(this, rowsMoved, key);
					craft_engine_gui_disconnect(this, colsMoved, key);
				}
				int  referenceCount() const { return m_referenceCount; }
				int      m_referenceCount = 0;
				uint32_t m_modelFlags = 0;
				bool     m_isModelSignalLocked = false;
			};

			/* Implementation of the member function in ModelIndex */
			ItemData   ModelIndex::getData(int role)const { return getItemModel()->getItemData(*this, role); }
			ModelIndex ModelIndex::getParentIndex() const { return getItemModel()->getParentModelIndex(*this); }
			ModelIndex ModelIndex::getChildIndex(int row, int column) const { return getItemModel()->getModelIndex(row, column, *this); }
			ModelIndex ModelIndex::getSiblingIndex(int row, int column) const { return getItemModel()->getSiblingModelIndex(row, column, *this); }
			ModelIndex ModelIndex::getSiblingIndexAtColumn(int column) const { return getItemModel()->getSiblingModelIndex(getRow(), column, *this); }
			ModelIndex ModelIndex::getSiblingIndexAtRow(int row) const { return getItemModel()->getSiblingModelIndex(row, getCol(), *this); }
			int ModelIndex::getChildrenCount() const { return getItemModel()->getChildrenCount(*this); }


			class AbstractListModel :public AbstractItemModel
			{
			private:

			public:
				virtual ModelIndex getModelIndex(int row, int col = 0, const ModelIndex& parent = ModelIndex())const override final { return ModelIndex(row, 0, row, this); }
				virtual int getColCount()const override final { return 1; };
			private:
				virtual ModelIndex getParentModelIndex(const ModelIndex& child)const override final { return child; };
				virtual ModelIndex getSiblingModelIndex(int row, int column, const ModelIndex& index) const override final { return getModelIndex(row, column, index); };
				//virtual ModelIndex getChildModelIndex(const ModelIndex& parent, int index)const override final { return parent; }
				virtual bool hasChildren(const ModelIndex& index)const override final { return false; }
				virtual int  getChildrenCount(const ModelIndex& index)const override final { return 0; }

				virtual bool onInsertCols(int column, int count, const ModelIndex& parent = ModelIndex()) override final { return false; }
				virtual bool onRemoveCols(int column, int count, const ModelIndex& parent = ModelIndex()) override final { return false; }
				virtual bool onMoveCols(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) override final { return false; }
			};


			class AbstractTableModel :public AbstractItemModel
			{
			private:

			public:
				virtual ModelIndex getModelIndex(int row, int col, const ModelIndex& parent = ModelIndex())const override final { return ModelIndex(row, col, row * getColCount() + col, this); }
			private:
				virtual ModelIndex getParentModelIndex(const ModelIndex& child)const override final { return child; };
				virtual ModelIndex getSiblingModelIndex(int row, int column, const ModelIndex& index) const override final { return getModelIndex(row, column, index); };
				//virtual ModelIndex getChildModelIndex(const ModelIndex& parent, int index)const override final { return parent; }
				virtual bool hasChildren(const ModelIndex& index)const override final { return false; }
				virtual int  getChildrenCount(const ModelIndex& index)const override final { return 0; }
			};



			template<typename Type>
			class TemplateListModel : public AbstractListModel
			{
			public:
				using item_type = Type;

				TemplateListModel() :
					m_itemList() {}

				virtual int getRowCount() const override final { return m_itemList.size(); }

			protected:
				const Type& getListModelItemData(int row) const { return m_itemList[row]; }
				Type& getListModelItemData(int row) { return m_itemList[row]; }
				const core::ArrayList<Type>& getItemList() const
				{
					return m_itemList;
				}
				void setItemList(const core::ArrayList<Type>& itemList)
				{
					m_itemList = itemList;
				}
			public:
				void setItemObject(const ModelIndex& idx, const Type& item)
				{
					getListModelItemData(idx.getRow()) = item;
				}
			private:
				virtual bool onInsertRows(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					int t_size = m_itemList.size();
					m_itemList.resize(m_itemList.size() + count);
					int beg = t_size - 1;
					int end = row;
					for (int i = beg; i >= end; i--)
						m_itemList[i + count] = m_itemList[i];
					return true;
				};
				virtual bool onRemoveRows(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					int beg = row;
					int end = m_itemList.size() - count;
					for (int i = beg; i < end; i++)
						m_itemList[i] = m_itemList[i + count];
					m_itemList.resize(m_itemList.size() - count);
					return true;
				};
				virtual bool onMoveRows(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) override final
				{
					if (destinationRow < sourceRow)
					{
						core::ArrayList<Type> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_list[i] = m_itemList[j];
						int beg = sourceRow - 1;
						int end = destinationRow;
						for (int i = beg; i >= end; i--)
							m_itemList[i + count] = m_itemList[i];
						for (int i = 0, j = destinationRow; i < count; i++, j++)
							m_itemList[j] = temp_list[i];
					}
					else if (destinationRow > sourceRow + count)
					{
						core::ArrayList<Type> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_list[i] = m_itemList[j];
						int beg = sourceRow + count;
						int end = destinationRow;
						for (int i = beg; i < end; i++)
							m_itemList[i - count] = m_itemList[i];
						for (int i = 0, j = destinationRow - count; i < count; i++, j++)
							m_itemList[j] = temp_list[i];
					}
					else
					{
						return false;
					}
					return true;
				};
			private:
				core::ArrayList<Type> m_itemList;
			};



			template<typename Type>
			class TemplateTableModel : public AbstractTableModel
			{
			public:
				using item_type = Type;

				TemplateTableModel() :
					m_itemTable() {}

				virtual int  getRowCount() const override final { return m_itemTable.size(); }
				virtual int  getColCount() const override final { return m_colCount; }

			protected:
				const Type& getTableModelItemData(int row, int col) const { return m_itemTable[row][col]; }
				Type& getTableModelItemData(int row, int col) { return m_itemTable[row][col]; }
				const core::ArrayList<core::ArrayList<Type>>& getItemTable() const { return m_itemTable; }
				void setItemTable(const core::ArrayList<core::ArrayList<Type>>& itemTable) { m_itemTable = itemTable; }
			public:
				void setItemObject(const ModelIndex& idx, const Type& item) { getTableModelItemData(idx.getRow(), idx.getCol()) = item; }
			private:
				virtual bool onInsertRows(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					int t_size = m_itemTable.size();
					m_itemTable.resize(m_itemTable.size() + count);
					int beg = t_size - 1;
					int end = row;
					for (int i = beg; i >= end; i--)
						m_itemTable[i].swap(m_itemTable[i + count]);
					for (int i = row; i < row + count; i++)
						m_itemTable[i].resize(getColCount());
					return true;
				};
				virtual bool onRemoveRows(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					int beg = row;
					int end = m_itemTable.size() - count;
					for (int i = beg; i < end; i++)
						m_itemTable[i].swap(m_itemTable[i + count]);
					m_itemTable.resize(m_itemTable.size() - count);
					return true;
				};
				virtual bool onMoveRows(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) override final
				{
					if (destinationRow < sourceRow)
					{
						core::ArrayList<core::ArrayList<Type>> temp_table;
						temp_table.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_table[i].swap(m_itemTable[j]);
						int beg = sourceRow - 1;
						int end = destinationRow;
						for (int i = beg; i >= end; i--)
							m_itemTable[i + count].swap(m_itemTable[i]);
						for (int i = 0, j = destinationRow; i < count; i++, j++)
							m_itemTable[j].swap(temp_table[i]);
					}
					else if (destinationRow > sourceRow + count)
					{
						core::ArrayList<core::ArrayList<Type>> temp_table;
						temp_table.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_table[i].swap(m_itemTable[j]);
						int beg = sourceRow + count;
						int end = destinationRow;
						for (int i = beg; i < end; i++)
							m_itemTable[i - count].swap(m_itemTable[i]);
						for (int i = 0, j = destinationRow - count; i < count; i++, j++)
							m_itemTable[j].swap(temp_table[i]);
					}
					else
					{
						return false;
					}
					return true;
				};
				virtual bool onInsertCols(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					int beg = getColCount() - 1;
					int end = col;
					m_colCount += count; //
					for (int r = 0; r < getRowCount(); r++)
					{
						m_itemTable[r].resize(getColCount());
						for (int c = beg; c >= end; c--)
							m_itemTable[r][c + count] = m_itemTable[r][c];
					}
					return true;
				}
				virtual bool onRemoveCols(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					int beg = col;
					int end = getColCount() - count;
					m_colCount -= count; //
					for (int r = 0; r < getRowCount(); r++)
					{
						for (int c = beg; c < end; c++)
							m_itemTable[r][c] = m_itemTable[r][c + count];
						m_itemTable[r].resize(getColCount());
					}
					return true;
				}
				virtual bool onMoveCols(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) override final
				{
					if (destinationCol < sourceCol)
					{
						core::ArrayList<Type> temp_list;
						temp_list.resize(count);
						for (int r = 0; r < getRowCount(); r++)
						{
							for (int i = 0, j = sourceCol; i < count; i++, j++)
								temp_list[i] = m_itemTable[r][j];
							int beg = sourceCol - 1;
							int end = destinationCol;
							for (int i = beg; i >= end; i--)
								m_itemTable[r][i + count] = m_itemTable[r][i];
							for (int i = 0, j = destinationCol; i < count; i++, j++)
								m_itemTable[r][j] = temp_list[i];
						}
					}
					else if (destinationCol > sourceCol + count)
					{
						core::ArrayList<Type> temp_list;
						temp_list.resize(count);
						for (int r = 0; r < getRowCount(); r++)
						{
							for (int i = 0, j = sourceCol; i < count; i++, j++)
								temp_list[i] = m_itemTable[r][j];
							int beg = sourceCol + count;
							int end = destinationCol;
							for (int i = beg; i < end; i++)
								m_itemTable[r][i - count] = m_itemTable[r][i];
							for (int i = 0, j = destinationCol - count; i < count; i++, j++)
								m_itemTable[r][j] = temp_list[i];
						}
					}
					else
					{
						return false;
					}
					return true;
				}

			private:
				core::ArrayList<core::ArrayList<Type>> m_itemTable;
				int m_colCount;
			};




			template<typename Type>
			class TemplateTreeModel : public AbstractItemModel
			{
			private:
				struct ItemNode
				{
					int index = -1;
					int row = -1;
					int parent = -1;
					core::ArrayList<int> children;//core::ArrayList
					core::ArrayList<Type> data_list;//core::ArrayList
				};
				core::ArrayList<int>       m_itemNodeMap;  //core::ArrayList
				core::ArrayList<ItemNode*> m_itemNodeList; //core::ArrayList
				int m_colCount;

			public:
				using item_type = Type;

				TemplateTreeModel()
					:m_itemNodeList(), m_colCount(0)
				{
					m_itemNodeMap.resize(1);
					m_itemNodeMap[0] = 0;
					m_itemNodeList.resize(1);
					m_itemNodeList[0] = new ItemNode;
					m_itemNodeList[0]->parent = -1;
					m_itemNodeList[0]->index = 0;
					m_itemNodeList[0]->row = 0;
				}

				~TemplateTreeModel()
				{
					for (auto& it : m_itemNodeList)
						delete it;
					m_itemNodeList.clear();
					m_itemNodeMap.clear();
					m_colCount = 0;
				}
			protected:
				const Type& getTreeModelItemData(const ModelIndex& index) const
				{
					return m_itemNodeList[_Get_Mapped_Index(index.getID())]->data_list[index.getCol()];
				}
				Type& getTreeModelItemData(const ModelIndex& index)
				{
					return m_itemNodeList[_Get_Mapped_Index(index.getID())]->data_list[index.getCol()];
				}
			public:
				void setItemObject(const ModelIndex& idx, const Type& item)
				{
					getTreeModelItemData(idx) = item;
				}
			public:
				ModelIndex getRootModelIndex()const
				{
					return ModelIndex(0, 0, _Get_Root_Node()->index, this);
				}

				/*
				* ModelIndex getModelIndex(int row, int col, const ModelIndex& parent = ModelIndex())
				 row: 父节点的子节点序号，必须存在该行
				 col: 列序号，可不存在该列
				 parent: 父节点索引，当传入一个invalid索引时返回根节点索引
				*/
				virtual ModelIndex getModelIndex(int row, int col, const ModelIndex& parent = ModelIndex())const override final
				{
					if (parent.isValid())
						return ModelIndex(row, col, m_itemNodeList[_Get_Mapped_Index(parent.getID())]->children[row], this);
					else
						return getRootModelIndex();
				}

				virtual ModelIndex getParentModelIndex(const ModelIndex& child)const override final
				{
					if (!child.isValid() || child.getItemModel() != this)
						craft_engine_error_v2("Invalid param \'child\'\n");
					if (child.getID() == 0)
						return ModelIndex();
					auto parent_id = m_itemNodeList[_Get_Mapped_Index(child.getID())]->parent;
					if (parent_id == 0)
						return getRootModelIndex();
					else
						return ModelIndex(m_itemNodeList[_Get_Mapped_Index(parent_id)]->row, child.getCol(), parent_id, this);
				}

				virtual ModelIndex getSiblingModelIndex(int row, int column, const ModelIndex& index) const override final
				{
					return getModelIndex(row, column, index.getParentIndex());
				};

				//virtual ModelIndex getChildModelIndex(const ModelIndex& parent, int index)const override final
				//{
				//	if (!parent.isValid())
				//		craft_engine_error_v2("Invalid param \'parent\'\n");
				//	auto child_id = m_itemNodeList[_Get_Mapped_Index(parent.getID())]->children[index];
				//	return ModelIndex(index, parent.getCol(), child_id, this);
				//}

				virtual bool hasChildren(const ModelIndex& index)const override final
				{
					return getChildrenCount(index) > 0;
				}

				virtual int getChildrenCount(const ModelIndex& index)const override final
				{
					if (!index.isValid())
						craft_engine_error_v2("Invalid param \'index\'\n");
					return m_itemNodeList[_Get_Mapped_Index(index.getID())]->children.size();
				}

				virtual int getRowCount()const override final
				{
					return m_itemNodeList.size() - 1;
				}

				virtual int getColCount()const override final
				{
					return m_colCount;
				}

			private:
				virtual bool onInsertRows(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					if (!parent.isValid())
						craft_engine_error_v2("Invalid param \'parent\'\n");
					auto col_count = getColCount();
					core::ArrayList<ItemNode*> node_list;
					node_list.resize(count);
					for (int i = 0; i < count; i++)
					{
						node_list[i] = new ItemNode;
						node_list[i]->index = m_itemNodeList.size();
						node_list[i]->data_list.resize(col_count);
						node_list[i]->parent = parent.getID();
						node_list[i]->row = row + i;
						m_itemNodeList.push_back(node_list[i]);
					}

					core::ArrayList<int> free_slots;
					for (int i = 0; i < m_itemNodeMap.size(); i++)
					{
						if (m_itemNodeMap[i] < 0)
							free_slots.push_back(i);
						if (free_slots.size() == count)
							break;
					}
					if (free_slots.size() < count)
					{
						int temp_size = m_itemNodeMap.size();
						int more_slots_count = count - free_slots.size();
						m_itemNodeMap.resize(temp_size + more_slots_count);
						for (int i = 0; i < more_slots_count; i++)
							free_slots.push_back(temp_size + i);
					}

					for (int i = 0; i < count; i++)
					{
						m_itemNodeMap[free_slots[i]] = node_list[i]->index;
						node_list[i]->index = free_slots[i];
					}

					auto parent_node = m_itemNodeList[_Get_Mapped_Index(parent.getID())];
					int t_size = parent_node->children.size();
					parent_node->children.resize(t_size + count);
					int beg = t_size - 1;
					int end = row;
					for (int i = beg; i >= end; i--)
						parent_node->children[i + count] = parent_node->children[i];
					for (int i = 0, j = row; i < count; i++, j++)
						parent_node->children[i] = node_list[j]->index;
					beg = row + count;
					end = parent_node->children.size();
					for (int i = beg; i < end; i++)
						m_itemNodeList[_Get_Mapped_Index(parent_node->children[i])]->row = i;
				}

				virtual bool onInsertCols(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					if (!parent.isValid())
						craft_engine_error_v2("Invalid param \'parent\'\n");

					int t_size = getColCount();
					m_colCount += count;
					for (int n = 0; n < m_itemNodeList.size(); n++)
					{
						m_itemNodeList[n]->data_list.resize(t_size + count);
						int beg = t_size - 1;
						int end = col;
						for (int i = beg; i >= end; i--)
							m_itemNodeList[n]->data_list[i + count] = m_itemNodeList[n]->data_list[i];
						for (int i = 0, j = col; i < count; i++, j++)
							m_itemNodeList[n]->data_list[i] = Type();
					}
				}

				virtual bool onRemoveRows(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					if (!parent.isValid())
						craft_engine_error_v2("Invalid param \'parent\'\n");

					core::ArrayList<int> remove_list;
					std::function<void(int)> recursive_get_remove_list;

					recursive_get_remove_list = [&](int index)
					{
						int mapped_index = _Get_Mapped_Index(index);
						if (m_itemNodeList[mapped_index]->children.size() > 0)
							for (int i = 0; i < m_itemNodeList[mapped_index]->children.size(); i++)
								recursive_get_remove_list(m_itemNodeList[mapped_index]->children[i]);
						remove_list.push_back(index);
					};
					for (int i = row; i < row + count; i++)
						recursive_get_remove_list(m_itemNodeList[_Get_Mapped_Index(parent.getID())]->children[i]);

					{
						int mapped_index = _Get_Mapped_Index(parent.getID());
						int beg = row;
						int end = m_itemNodeList[mapped_index]->children.size() - count;
						for (int i = beg; i < end; i++)
							m_itemNodeList[mapped_index]->children[i] = m_itemNodeList[mapped_index]->children[i + count];
						for (int i = beg; i < end; i++)
							m_itemNodeList[_Get_Mapped_Index(m_itemNodeList[mapped_index]->children[i])]->row = i;
						m_itemNodeList[mapped_index]->children.resize(m_itemNodeList[mapped_index]->children.size() - count);
					}


					for (int i = 0; i < remove_list.size(); i++)
					{
						int mapped_index = _Get_Mapped_Index(remove_list[i]);

						m_itemNodeMap[remove_list[i]] = -1;
						delete m_itemNodeList[mapped_index];

						if (mapped_index != m_itemNodeList.size() - 1)
						{
							auto last_node = m_itemNodeList[m_itemNodeList.size() - 1];
							m_itemNodeMap[last_node->index] = mapped_index;
							m_itemNodeList[mapped_index] = last_node;
						}
						m_itemNodeList.resize(m_itemNodeList.size() - 1);
					}

					for (int i = m_itemNodeMap.size() - 1; i >= 0; i--)
						if (m_itemNodeMap[i] < 0)
							m_itemNodeMap.resize(i);
						else
							break;
				}

				virtual bool onRemoveCols(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					auto root_index = _Get_Root_Node()->index;
					if (!parent.isValid() || root_index != parent.getID())
						craft_engine_error_v2("Invalid param \'parent\'\n");
					int beg = col;
					int end = getColCount() - count;
					m_colCount -= count; //
					for (int n = 0; n < m_itemNodeList.size(); n++)
					{
						for (int c = beg; c < end; c++)
							m_itemNodeList[n]->data_list[c] = m_itemNodeList[n]->data_list[c + count];
						m_itemNodeList[n]->data_list.resize(getColCount());
					}
					return true;
				}

				virtual bool onMoveRows(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) override final
				{
					if (!sourceParent.isValid())
						craft_engine_error_v2("Invalid param \'sourceParent\'\n");
					if (!destinationParent.isValid())
						craft_engine_error_v2("Invalid param \'destinationParent\'\n");

					auto src_node = m_itemNodeList[_Get_Mapped_Index(sourceParent.getID())];
					auto dst_node = m_itemNodeList[_Get_Mapped_Index(destinationParent.getID())];
					core::ArrayList<int> temp_node_list;
					temp_node_list.resize(count);
					for (int i = 0; i < count; i++)
					{
						temp_node_list[i] = src_node->children[i + sourceRow];
						auto node = m_itemNodeList[_Get_Mapped_Index(temp_node_list[i])];
						node->parent = destinationParent.getID();
						node->row = destinationRow + i;
					}

					{
						int beg = sourceRow;
						int end = src_node->children.size() - count;
						for (int i = beg; i < end; i++)
							src_node->children[i] = src_node->children[i + count];
						for (int i = beg; i < end; i++)
							m_itemNodeList[_Get_Mapped_Index(src_node->children[i])]->row = i;
						src_node->children.resize(src_node->children.size() - count);
					}
					{
						int t_size = dst_node->children.size();
						dst_node->children.resize(t_size + count);
						int beg = t_size - 1;
						int end = destinationRow;
						for (int i = beg; i >= end; i--)
							dst_node->children[i + count] = dst_node->children[i];
						for (int i = 0, j = destinationRow; i < count; i++, j++)
							dst_node->children[i] = temp_node_list[j];
						beg = destinationRow + count;
						end = dst_node->children.size();
						for (int i = beg; i < end; i++)
							m_itemNodeList[_Get_Mapped_Index(dst_node->children[i])]->row = i;
					}
				}

				virtual bool onMoveCols(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) override final
				{
					if (!sourceParent.isValid())
						craft_engine_error_v2("Invalid param \'sourceParent\'\n");
					if (!destinationParent.isValid())
						craft_engine_error_v2("Invalid param \'destinationParent\'\n");

					if (destinationCol < sourceCol)
					{
						core::ArrayList<Type> temp_list;
						temp_list.resize(count);
						for (int n = 0; n < getRowCount(); n++)
						{
							for (int i = 0, j = sourceCol; i < count; i++, j++)
								temp_list[i] = m_itemNodeList[n]->data_list[j];
							int beg = sourceCol - 1;
							int end = destinationCol;
							for (int i = beg; i >= end; i--)
								m_itemNodeList[n]->data_list[i + count] = m_itemNodeList[n]->data_list[i];
							for (int i = 0, j = destinationCol; i < count; i++, j++)
								m_itemNodeList[n]->data_list[j] = temp_list[i];
						}
					}
					else if (destinationCol > sourceCol + count)
					{
						core::ArrayList<Type> temp_list;
						temp_list.resize(count);
						for (int n = 0; n < getRowCount(); n++)
						{
							int mapped_index = _Get_Mapped_Index(n);
							for (int i = 0, j = sourceCol; i < count; i++, j++)
								temp_list[i] = m_itemNodeList[mapped_index]->data_list[j];
							int beg = sourceCol + count;
							int end = destinationCol;
							for (int i = beg; i < end; i++)
								m_itemNodeList[mapped_index]->data_list[i - count] = m_itemNodeList[mapped_index]->data_list[i];
							for (int i = 0, j = destinationCol - count; i < count; i++, j++)
								m_itemNodeList[mapped_index]->data_list[j] = temp_list[i];
						}
					}
					else
					{
						return false;
					}
					return true;

				}

			private:

				int _Get_Mapped_Index(int index) const
				{
					return m_itemNodeMap[index];
				}

				ItemNode* _Get_Root_Node() const
				{
					if (m_itemNodeList.size() != 0 && m_itemNodeMap.size() != 0 && _Get_Mapped_Index(0) == 0)
						return m_itemNodeList[_Get_Mapped_Index(0)];
					else
						return nullptr;
				}

				ItemNode* _Get_Parent_Node(ItemNode* child) const
				{
					return m_itemNodeList[_Get_Mapped_Index(child->parent)];
				}

				ItemNode* _Get_Child_Node(ItemNode* parent, int row) const
				{
					return m_itemNodeList[_Get_Mapped_Index(parent->children[row])];
				}
			};


			template<typename Type>
			using TemplateFlowModel = TemplateListModel<Type>;

		}


	}

}
