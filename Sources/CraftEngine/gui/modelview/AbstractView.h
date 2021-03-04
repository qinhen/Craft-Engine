#pragma once
#include "../widgets/ScrollWidget.h"
#include "./AbstractModel.h"
#include "./AbstractDelegate.h"

namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{

			class AbstractItemWidget;
			class AbstractItemView;


			class AbstractItemWidget :public Widget
			{
			public:
				AbstractItemWidget(AbstractItemView* view) : Widget(nullptr)
				{
					m_view = view;
					setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eItemView));
					setClickable(true);
				}
				const AbstractItemView* getView()const { return m_view; }
				AbstractItemView* getView() { return m_view; }
				const ModelIndex& getIndex()const { return m_index; }
				void setIndex(const ModelIndex& index) { m_index = index; onItemChanged(); }
				void setSelected(bool enable) { m_isSelected = enable; }
				bool isSelected() const { return m_isSelected; }
				virtual void onItemChanged() = 0;
			public:
				virtual void onPaint_drawEx()
				{

				}

				virtual void onPaint_drawBackground2() final
				{
					auto painter = getPainter();
					if (isSelected())
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
					else if (isHighlight())
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
				}

				virtual void onPaint_drawFrame2() final
				{
					auto painter = getThisPainter();
					if (isSelected() && getPalette().mFrameMode != Palette::FrameMode::eFrameMode_None)
						painter.drawRectFrame(Rect(Point(0), getSize()), getPalette().mFrameColor);
				}

				virtual void onPaintEvent() override final
				{
					auto painter = getPainter();
					drawBackground();
					onPaint_drawBackground2();
					onPaint_drawEx();
					drawFrame();
					onPaint_drawFrame2();
				}
			protected:

				virtual void onPressed(const MouseEvent& mouseEvent) override final {}
				virtual void onReleased(const MouseEvent& mouseEvent) override final {}
			private:
				AbstractItemView* m_view;
				ModelIndex m_index;
				bool m_isSelected = false;
			};



			class AbstractItemView :public ScrollWidget
			{
			public:

				AbstractItemView(Widget* parent) :
					ScrollWidget(parent), m_model(nullptr), m_signalConnectionKey(~0U) 
				{					
					// Set mouse event catch flags
					setMouseEventCatchFlags(MouseEvent::eAll);
				}

				~AbstractItemView() { setModel(nullptr); }

				craft_engine_gui_signal(itemClicked, void(const ModelIndex& index));
				craft_engine_gui_signal(currentIndexChanged, void(const ModelIndex& from, const ModelIndex& to));

				virtual AbstractItemModel* getModel() { return m_model; }
				virtual const AbstractItemModel* getModel()const { return m_model; }
				virtual void setModel(AbstractItemModel* model)
				{
					// Avoid redundant signal connecion
					if (m_model == model)
						return;

					if (m_model != nullptr)
					{
						m_model->referenceDecrease(m_signalConnectionKey);
						//if (m_model->referenceCount() == 0 && !(m_model->getModelFlags() & AbstractItemModel::eNotAllowDestroied))
						//	delete m_model;
						m_model = nullptr;
						m_signalConnectionKey = ~0U;
					}

					if (model != nullptr)
					{
						m_model = model;
						m_model->referenceIncrease();
						m_signalConnectionKey =
						craft_engine_gui_connect(getModel(), modelUpdated, this, onModelUpdated);
						craft_engine_gui_connect(getModel(), modelDeleted, this, onModelDeleted);
						craft_engine_gui_connect(getModel(), itemUpdated, this, onItemUpdated);
						craft_engine_gui_connect(getModel(), rowsInserted, this, onRowsInserted);
						craft_engine_gui_connect(getModel(), colsInserted, this, onColsInserted);
						craft_engine_gui_connect(getModel(), rowsRemoved, this, onRowsRemoved);
						craft_engine_gui_connect(getModel(), colsRemoved, this, onColsRemoved);
						craft_engine_gui_connect(getModel(), rowsMoved, this, onRowsMoved);
						craft_engine_gui_connect(getModel(), colsMoved, this, onColsMoved);
					}
				}
				/*
				 No signal emitted, no view update.
				*/
				void setModelItemData(const ModelIndex& index, const ItemData& data, int role)
				{
					getModel()->setItemData_NoSignal(index, data, role);
				}

				void setDefaultItemWidgetSize(const Size& size) { m_defaultSize = size; }
				const Size& getDefaultItemWidgetSize()const { return m_defaultSize; }

				virtual AbstractItemWidget* getItemWidget(const ModelIndex& index) = 0;
				virtual ModelIndex getCurrentIndex()const = 0;
				virtual void       setCurrentIndex(const ModelIndex& index) = 0;
			protected:
				virtual void onModelUpdated(AbstractItemModel* model) = 0;
				virtual void onModelDeleted(AbstractItemModel* model) = 0;
				virtual void onItemUpdated(const ModelIndex& index) = 0;
				virtual void onRowsInserted(int row, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual void onColsInserted(int col, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual void onRowsRemoved(int row, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual void onColsRemoved(int col, int count, const ModelIndex& parent = ModelIndex()) = 0;
				virtual void onRowsMoved(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) = 0;
				virtual void onColsMoved(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) = 0;

			protected:
				virtual AbstractItemWidget* createItemWidget() = 0;
			protected:
				virtual Point getChildOriginal(int groupid = GroupID::gDefaultWidget)const override final
				{
					switch (groupid)
					{
					case GroupID::gItemWidget:
						return ScrollWidget::getChildOriginal(0);
					default:
						return ScrollWidget::getChildOriginal(groupid);
					}
				}
			private:
				AbstractItemModel* m_model;
				uint32_t m_signalConnectionKey;
				Size m_defaultSize = Size(100, 22);

			private:
				virtual void bindChildWidget(Widget* child) override final
				{					
					// do nothing, only accept group 0
					Widget::bindChildWidget(child);
				}

				virtual bool onMouseEventCaught(const MouseEvent& mouseEvent, Widget* widget) override final
				{
					auto item_widget = (AbstractItemWidget*)widget;
					if (widget->getGroupid() == GroupID::gItemWidget && item_widget->getView() == this)
						return onHandleItemMouseEvent(mouseEvent, item_widget);
					return true;
				}

				virtual bool onHandleItemMouseEvent(const MouseEvent& mouseEvent, AbstractItemWidget* widget) final
				{
					switch (mouseEvent.type)
					{
					case MouseEvent::eClicked:
						return onItemClicked(widget, mouseEvent);
						break;
					case MouseEvent::eDoubleClicked:
						return onItemDoubleClicked(widget, mouseEvent);
						break;
					case MouseEvent::ePressed:
						return onItemPressed(widget, mouseEvent);
						break;
					case MouseEvent::eReleased:
						return onItemReleased(widget, mouseEvent);
						break;
					case MouseEvent::eCallMenu:
						setCurrentIndex(widget->getIndex());
						return onItemCallMenu(widget, mouseEvent);
						break;
					case MouseEvent::eWheel:
						return onItemWheel(widget, mouseEvent);
						break;
					case MouseEvent::eMoveIn:
						return onItemMoveIn(widget, mouseEvent);
						break;
					case MouseEvent::eMoveOut:
						return onItemMoveOut(widget, mouseEvent);
						break;
					case MouseEvent::eFocusIn:
						return onItemFocusIn(widget, mouseEvent);
						break;
					case MouseEvent::eFocusOut:
						return onItemFocusOut(widget, mouseEvent);
						break;
					case MouseEvent::eDrag:
						return onItemDrag(widget, mouseEvent);
						break;
					case MouseEvent::eDragApply:
						return onItemDragApply(widget, mouseEvent);
						break;
					}
					return true;	
				}
			protected:
				virtual bool onItemClicked(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemDoubleClicked(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemCallMenu(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemWheel(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemMoveIn(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemMoveOut(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemFocusIn(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemFocusOut(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemDrag(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onItemDragApply(AbstractItemWidget* widget, const MouseEvent& mouseEvent) { return true; }
			private:
				virtual bool onItemPressed(AbstractItemWidget* widget, const MouseEvent& mouseEvent) final
				{
					widget->setSelected(true);
					return false;
				}
				virtual bool onItemReleased(AbstractItemWidget* widget, const MouseEvent& mouseEvent) final
				{
					if (!widget->getIndex().isValid() || 
						widget->getView()->getCurrentIndex() != widget->getIndex())
						widget->setSelected(false);
					return false;
				}
			protected:
				virtual void onPressed(const MouseEvent& mouseEvent) override final
				{
					setCurrentIndex(ModelIndex());
				}
			};


			/*
			 *Accept Table/List Model
			*/
			class AbstractListView :public AbstractItemView
			{
			private:
				core::ArrayList<AbstractItemWidget*> m_itemWidgetList;
				core::ArrayList<uint32_t> m_rowHeight;
				int m_currentRow = -1;
			public:
				virtual AbstractItemWidget* getItemWidget(const ModelIndex& index) override { return m_itemWidgetList[index.getRow()]; }
				virtual ModelIndex getCurrentIndex() const override
				{
					if (getModel() != nullptr && m_currentRow >= 0 && m_currentRow < m_itemWidgetList.size())
						return getModel()->getModelIndex(m_currentRow, 0, ModelIndex());
					else
						return ModelIndex();
				}
				virtual void setCurrentIndex(const ModelIndex& index) override
				{
					auto old_index = getCurrentIndex();
					if (old_index.isValid())
						m_itemWidgetList[old_index.getRow()]->setSelected(false);
					if (!index.isValid())
					{
						m_currentRow = -1;
						return;
					}
					m_currentRow = index.getRow();
					m_itemWidgetList[m_currentRow]->setSelected(true);
				}
				
				int getCurrentRow() const { return m_currentRow; }
				int getListSize() const { return m_itemWidgetList.size(); }
			public:
				AbstractListView(Widget* parent) : AbstractItemView(parent)
				{

				}

				~AbstractListView()
				{
					setModel(nullptr);
				}

				virtual void setModel(AbstractItemModel* model) override final
				{
					AbstractItemView::setModel(model);
					_Set_Invalid_Current_Index();
					if (model != nullptr)
						_Resize_List(model->getRowCount());
					else
						_Resize_List(0);
				}

				void setRowHeight(int row, int height)
				{
					if (row < 0 || row >= m_rowHeight.size())
						craft_engine_error_v2("Invalid param \'row\'\n");
					m_rowHeight[row] = height;
					this->_Adjust_Item_Widget_Layout();
				}

				void setRowHeight(int height)
				{
					for (auto& it : m_rowHeight)
						it = height;
					this->_Adjust_Item_Widget_Layout();
				}
			public:


				virtual Widget* onDetect(const MouseEvent& mouseEvent) override final
				{
					if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
						return nullptr;

					Widget* hit;

					hit = onDetectScrollBar(mouseEvent);
					if (hit != nullptr)
						return hit;

					MouseEvent subEvent = mouseEvent;

					Point relative_point = mouseEvent.local - getPos();
					relative_point -= getChildOriginal(GroupID::gItemWidget);
					subEvent.local = relative_point;

					int lower_bound = m_itemWidgetList.size() - 1;
					int upper_bound = 0;
					int temp = 0;
					int pos = relative_point.y;
					while (upper_bound <= lower_bound)
					{
						temp = (upper_bound + lower_bound) / 2;
						int top = m_itemWidgetList[temp]->getRect().top();
						int bottom = m_itemWidgetList[temp]->getRect().bottom();
						if (pos < top)
							lower_bound = temp - 1;
						else if (pos >= bottom)
							upper_bound = temp + 1;
						else
						{
							hit = m_itemWidgetList[temp]->onMouseEvent(subEvent); //
							if (hit != nullptr)
								return hit;
						}
					}
					return this;
				}

			public:

				virtual void onPaintEvent() override
				{
					drawBackground();

					int min_one = 0;
					int max_one = m_itemWidgetList.size();
					{
						int lower_bound = m_itemWidgetList.size() - 1;
						int upper_bound = 0;
						int temp = 0;
						int bottom = 0;
						int pos = -getChildOriginal(GroupID::gItemWidget).y;
						while (upper_bound < lower_bound)
						{
							temp = (upper_bound + lower_bound) / 2;
							bottom = m_itemWidgetList[temp]->getRect().bottom();
							if (pos < bottom)
								lower_bound = temp;
							else
								upper_bound = temp + 1;
						}
						min_one = lower_bound;
					}
					{
						int lower_bound = m_itemWidgetList.size() - 1;
						int upper_bound = 0;
						int temp = 0;
						int top = 0;
						int pos = -getChildOriginal(GroupID::gItemWidget).y + getHeight();
						while (upper_bound < lower_bound)
						{
							temp = (upper_bound + lower_bound) / 2;
							top = m_itemWidgetList[temp]->getRect().bottom(); //
							if (pos < top)
								lower_bound = temp;
							else
								upper_bound = temp + 1;
						}
						max_one = lower_bound + 1;
					}

					min_one = math::clamp(min_one, 0, (int)m_itemWidgetList.size());
					max_one = math::clamp(max_one, 0, (int)m_itemWidgetList.size());


					for (int i = min_one; i < max_one; i++)
						drawChild(m_itemWidgetList[i]);
					onPaint_drawScrollBar();
					drawFrame();
				}

				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override final { ScrollWidget::onResizeEvent(resizeEvent); _Adjust_Item_Widget_Layout(); }

			protected:
				virtual void onKeyBoardEvent(const KeyboardEvent& keyboardEvent)override
				{
					if (keyboardEvent.down)
					{
						if (keyboardEvent.key == (KeyBoard::eKey_Up) && m_currentRow > 0)
						{
							auto old_index = getCurrentIndex();
							m_itemWidgetList[m_currentRow]->setSelected(false);
							m_currentRow--;
							m_itemWidgetList[m_currentRow]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						else if (keyboardEvent.key == (KeyBoard::eKey_Down) && m_currentRow >= 0 && m_itemWidgetList.size() > 0 && m_currentRow < m_itemWidgetList.size() - 1)
						{
							auto old_index = getCurrentIndex();
							m_itemWidgetList[m_currentRow]->setSelected(false);
							m_currentRow++;
							m_itemWidgetList[m_currentRow]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
					}
				}


			private:

				void _Resize_List(int row)
				{
					auto default_widget_size = this->getDefaultItemWidgetSize();
					m_rowHeight.resize(row, default_widget_size.y);
					auto before_row = m_itemWidgetList.size();
					if (row > before_row)
					{
						m_itemWidgetList.resize(row);
						for (int i = before_row; i < row; i++)
						{
							auto nwidget = createItemWidget();
							nwidget->setGroupid(GroupID::gItemWidget);
							nwidget->bindParentWidget(this);
							m_itemWidgetList[i] = nwidget;
						}
					}
					else if (row < before_row)
					{
						for (int i = before_row - 1; i >= row; i--)
							delete m_itemWidgetList[i];
						m_itemWidgetList.resize(row);
					}
					_Adjust_Item_Widget_Layout();
				}

				void _Adjust_Item_Widget_Layout()
				{
					auto list_size = getListSize();
					int y = 0;
					int x = 0;

					if (list_size > 0)
					{
						for (int i = 0; i < list_size; i++)
						{
							// for every row
							Rect rect = Rect(x, y, getWidth(), m_rowHeight[i]);
							if (!m_itemWidgetList[i]->getRect().equals(rect))
								m_itemWidgetList[i]->setRect(rect);
							auto index = getModel()->getModelIndex(i, 0, ModelIndex());
							m_itemWidgetList[i]->setIndex(index);
							y += m_rowHeight[i];
						}
					}
					setScrollArea(Size(x, y) + _Get_Table_Item_Offset());
				}

				Offset _Get_Table_Item_Offset()const
				{
					//Size offset;
					//offset.y = m_enableTableColLabel ? m_labelSize.y : 0;
					//offset.x = m_enableTableRowLabel ? m_labelSize.x : 0;
					//return offset;
					return Offset(0, 0);
				}


				bool onItemClicked(AbstractItemWidget* widget, const MouseEvent& mouseEvent) override final
				{
					auto old_index = getCurrentIndex();
					bool item_changed = false;
					if (old_index.isValid())
						m_itemWidgetList[old_index.getRow()]->setSelected(false);
					auto index = widget->getIndex();
					if (index != old_index)
						item_changed = true;
					m_currentRow = index.getRow();
					m_itemWidgetList[m_currentRow]->setSelected(true);
					craft_engine_gui_emit(itemClicked, index);
					if (item_changed)
						craft_engine_gui_emit(currentIndexChanged, old_index, index);
					return true;
				}

				void _Set_Invalid_Current_Index()
				{
					auto old_index = getCurrentIndex();
					if (old_index.isValid())
					{
						m_itemWidgetList[old_index.getRow()]->setSelected(false);
						craft_engine_gui_emit(currentIndexChanged, ModelIndex(), ModelIndex());
					}
					m_currentRow = -1;
				}
			protected:
				virtual void onModelUpdated(AbstractItemModel* model)
				{
					setModel(model);
				}
				virtual void onModelDeleted(AbstractItemModel* model) override
				{
					setModel(nullptr);
				}
				virtual void onItemUpdated(const ModelIndex& index) override
				{
					m_itemWidgetList[index.getRow()]->setIndex(index);
				}
				virtual void onRowsInserted(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					Size default_size = getDefaultItemWidgetSize();
					int before_row = getListSize();
					m_rowHeight.resize(getModel()->getRowCount());
					for (int i = row; i < before_row; i++)
						m_rowHeight[i + count] = m_rowHeight[i];
					for (int i = row; i < row + count; i++)
						m_rowHeight[i] = default_size.y;
					_Resize_List(getModel()->getRowCount());
				}
				virtual void onColsInserted(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					// Do Nothing.
				}
				virtual void onRowsRemoved(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					int before_row = getListSize();
					for (int i = row + count; i < before_row; i++)
						m_rowHeight[i - count] = m_rowHeight[i];
					m_rowHeight.resize(getModel()->getRowCount());
					_Resize_List(getModel()->getRowCount());
				}
				virtual void onColsRemoved(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					// Do Nothing.
				}
				virtual void onRowsMoved(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) override final
				{
					_Set_Invalid_Current_Index();
					if (destinationRow < sourceRow)
					{
						core::ArrayList<int> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_list[i] = m_rowHeight[j];
						int beg = sourceRow - 1;
						int end = destinationRow;
						for (int i = beg; i >= end; i--)
							m_rowHeight[i + count] = m_rowHeight[i];
						for (int i = 0, j = destinationRow; i < count; i++, j++)
							m_rowHeight[j] = temp_list[i];
					}
					else if (destinationRow > sourceRow + count)
					{
						core::ArrayList<int> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_list[i] = m_rowHeight[j];
						int beg = sourceRow + count;
						int end = destinationRow;
						for (int i = beg; i < end; i++)
							m_rowHeight[i - count] = m_rowHeight[i];
						for (int i = 0, j = destinationRow - count; i < count; i++, j++)
							m_rowHeight[j] = temp_list[i];
					}
					_Resize_List(getModel()->getRowCount());
				}
				virtual void onColsMoved(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) override final
				{
					// Do Nothing.
				}

			};


			/*
			 *Accept Table/List Model
			*/
			class AbstractTableView :public AbstractItemView
			{
			private:
				core::ArrayList<core::ArrayList<AbstractItemWidget*>> m_itemWidgetTable;
				int m_currentRow = -1;
				int m_currentCol = -1;
				Size m_tableSize = Size(0, 0);
				std::vector<uint32_t> m_rowHeight;
				std::vector<uint32_t> m_colWidth;

			public:
				virtual AbstractItemWidget* getItemWidget(const ModelIndex& index) override { return m_itemWidgetTable[index.getRow()][index.getCol()]; }
				virtual ModelIndex getCurrentIndex() const override
				{
					if (getModel() != nullptr && m_currentRow >= 0 && m_currentCol >= 0 && m_currentRow < m_itemWidgetTable.size() && m_currentCol < m_itemWidgetTable[0].size())
						return getModel()->getModelIndex(m_currentRow, m_currentCol, ModelIndex());
					else
						return ModelIndex();
				}
				virtual void setCurrentIndex(const ModelIndex& index) override
				{
					auto old_index = getCurrentIndex();
					if (old_index.isValid())
						m_itemWidgetTable[old_index.getRow()][old_index.getCol()]->setSelected(false);
					if (!index.isValid())
					{
						m_currentRow = -1;
						m_currentCol = -1;
						return;
					}
					m_currentRow = index.getRow();
					m_currentCol = index.getCol();
					m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(true);
				}
				int getCurrentRow() const { return m_currentRow; }
				int getCurrentCol() const { return m_currentCol; }
				const Size& getTableSize() const { return m_tableSize; }
			public:
				AbstractTableView(Widget* parent) : AbstractItemView(parent)
				{

				}

				~AbstractTableView()
				{
					setModel(nullptr);
				}

				virtual void setModel(AbstractItemModel* model) override final
				{
					AbstractItemView::setModel(model);
					_Set_Invalid_Current_Index();
					if (model != nullptr)
						_Resize_Table(model->getRowCount(), model->getColCount());
					else
						_Resize_Table(0, 0);
				}

				void setColWidth(int col, int width)
				{
					if (col < 0 || col >= m_colWidth.size())
						craft_engine_error_v2("Invalid param \'col\'\n");
					m_colWidth[col] = width;
					this->_Adjust_Item_Widget_Layout();
				}

				void setRowHeight(int row, int height)
				{
					if (row < 0 || row >= m_rowHeight.size())
						craft_engine_error_v2("Invalid param \'row\'\n");
					m_rowHeight[row] = height;
					this->_Adjust_Item_Widget_Layout();
				}
			public:

				virtual Widget* onDetect(const MouseEvent& mouseEvent) override final
				{
					if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
						return nullptr;

					Widget* hit;

					hit = onDetectScrollBar(mouseEvent);
					if (hit != nullptr)
						return hit;

					Point relative_point = mouseEvent.local - getPos();

					//if (m_enableTableColLabel && m_enableTableRowLabel)
					//{
					//	if (relative_point.x < m_labelSize.x && relative_point.y < m_labelSize.y)
					//		return this;
					//}

					Point relative_point_temp = relative_point;
					relative_point -= getChildOriginal(GroupID::gItemWidget);

					auto table_size = getTableSize();
					int row = -1;
					int col = -1;

					if (table_size.x > 0 && table_size.y > 0)
					{
						{
							int lower_bound = 0;
							int upper_bound = table_size.y - 1;
							int pos = relative_point.x;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_itemWidgetTable[0][temp]->getRect().right() < pos)
									lower_bound = temp + 1;
								else if (m_itemWidgetTable[0][temp]->getRect().left() >= pos)
									upper_bound = temp - 1;
								else
									break;
							}
							col = temp;
						}
						{
							int lower_bound = 0;
							int upper_bound = table_size.x - 1;
							int pos = relative_point.y;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_itemWidgetTable[temp][0]->getRect().bottom() < pos)
									lower_bound = temp + 1;
								else if (m_itemWidgetTable[temp][0]->getRect().top() >= pos)
									upper_bound = temp - 1;
								else
									break;
							}
							row = temp;
						}
					}
					MouseEvent subEvent = mouseEvent;

					//if (row >= 0 && col >= 0)
					//{
					//	if (m_enableTableColLabel)
					//	{
					//		subEvent.local = relative_point_temp - getChildOriginal(3);
					//		if (relative_point_temp.y > 0 && relative_point_temp.y < m_labelSize.y && col >= 0)
					//			return m_colLabels[col]->onMouseEvent(subEvent);
					//	}
					//	if (m_enableTableRowLabel)
					//	{
					//		subEvent.local = relative_point_temp - getChildOriginal(4);
					//		if (relative_point_temp.x > 0 && relative_point_temp.x < m_labelSize.x && row >= 0)
					//			return m_rowLabels[row]->onMouseEvent(subEvent);
					//	}
					//}
					subEvent.local = relative_point_temp - getChildOriginal(GroupID::gItemWidget);
					if (row >= 0 && col >= 0)
					{
						hit = m_itemWidgetTable[row][col]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}

					return this;
				}

			public:

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					drawBackground();
					auto range = _Get_Visible_Item_Range();
					int left = range.left();
					int right = range.right();
					int top = range.top();
					int bottom = range.bottom();

					for (int i = left; i < right; i++)
					{
						for (int j = top; j < bottom; j++)
						{
							drawChild(m_itemWidgetTable[j][i]);
						}
					}

					//if (m_enableTableRowLabel)
					//{
					//	Rect row_label_rect = Rect(this_global_rect.mOffset, Offset(m_labelSize.x, this_global_rect.mHeight));
					//	renderer->drawRect(row_label_rect, getPalette().mBackgroundColor, global_limit_rect);
					//	renderer->drawRectFrame(row_label_rect, getPalette().mFrameColor, global_limit_rect);
					//	auto label_base_point = this_global_rect.mOffset;
					//	label_base_point.y += getChildOriginal(4).y;
					//	subEvent.globalRelativePoint = label_base_point;
					//	for (int i = top; i < bottom; i++)
					//		m_rowLabels[i]->onPaintEvent(subEvent);
					//}

					//if (m_enableTableColLabel)
					//{
					//	Rect col_label_rect = Rect(this_global_rect.mOffset, Offset(this_global_rect.mWidth, m_labelSize.y));
					//	renderer->drawRect(col_label_rect, getPalette().mBackgroundColor, global_limit_rect);
					//	renderer->drawRectFrame(col_label_rect, getPalette().mFrameColor, global_limit_rect);
					//	auto label_base_point = this_global_rect.mOffset;
					//	label_base_point.x += getChildOriginal(3).x;
					//	subEvent.globalRelativePoint = label_base_point;
					//	for (int i = left; i < right; i++)
					//		m_colLabels[i]->onPaintEvent(subEvent);
					//}

					//if (m_enableTableRowLabel && m_enableTableColLabel)
					//{
					//	Rect col_label_rect = Rect(this_global_rect.mOffset, m_labelSize);
					//	renderer->drawRect(col_label_rect, getPalette().mBackgroundHighLightColor, global_limit_rect);
					//}
					onPaint_drawScrollBar();
					drawFrame();
				}

				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override final { ScrollWidget::onResizeEvent(resizeEvent); _Adjust_Item_Widget_Layout(); }

			protected:
				virtual void onKeyBoardEvent(const KeyboardEvent& keyboardEvent)override
				{
					if (keyboardEvent.down)
					{
						if (keyboardEvent.key == (KeyBoard::eKey_Up) && m_currentRow > 0)
						{
							auto old_index = getCurrentIndex();
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(false);
							m_currentRow--;
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						else if (keyboardEvent.key == (KeyBoard::eKey_Down) && m_currentRow >= 0 && m_itemWidgetTable.size() > 0 && m_currentRow < m_itemWidgetTable.size() - 1)
						{
							auto old_index = getCurrentIndex();
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(false);
							m_currentRow++;
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						if (keyboardEvent.key == (KeyBoard::eKey_Left) && m_currentCol > 0)
						{
							auto old_index = getCurrentIndex();
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(false);
							m_currentCol--;
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						else if (keyboardEvent.key == (KeyBoard::eKey_Right) && m_currentCol >= 0 && getTableSize().y > 0 && m_currentCol < getTableSize().y - 1)
						{
							auto old_index = getCurrentIndex();
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(false);
							m_currentCol++;
							m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
					}
				}

			private:

				bool onItemClicked(AbstractItemWidget* widget, const MouseEvent& mouseEvent) override final
				{
					auto old_index = getCurrentIndex();
					bool item_changed = false;
					if (old_index.isValid())
						m_itemWidgetTable[old_index.getRow()][old_index.getCol()]->setSelected(false);
					auto index = widget->getIndex();
					if (index != old_index)
						item_changed = true;
					m_currentRow = index.getRow();
					m_currentCol = index.getCol();
					m_itemWidgetTable[m_currentRow][m_currentCol]->setSelected(true);
					craft_engine_gui_emit(itemClicked, index);
					if (item_changed)
						craft_engine_gui_emit(currentIndexChanged, old_index, index);
					return true;
				}

				void _Set_Invalid_Current_Index()
				{
					auto old_index = getCurrentIndex();
					if (old_index.isValid())
					{
						m_itemWidgetTable[old_index.getRow()][old_index.getCol()]->setSelected(false);
						craft_engine_gui_emit(currentIndexChanged, ModelIndex(), ModelIndex());
					}
					m_currentRow = -1;
					m_currentCol = -1;
				}

			private:
				void _Resize_Table(int row, int col)
				{
					auto default_widget_size = this->getDefaultItemWidgetSize();
					m_colWidth.resize(col, default_widget_size.x);
					m_rowHeight.resize(row, default_widget_size.y);
					auto table_size = getTableSize();

					auto before_row = m_itemWidgetTable.size();
					if (row > before_row)
					{
						m_itemWidgetTable.resize(row);
						for (int i = before_row; i < row; i++)
						{
							m_itemWidgetTable[i].resize(m_itemWidgetTable[0].size());
							for (int j = 0; j < m_itemWidgetTable[i].size(); j++)
							{
								auto nwidget = createItemWidget();
								nwidget->setGroupid(GroupID::gItemWidget);
								nwidget->bindParentWidget(this);
								m_itemWidgetTable[i][j] = nwidget;
							}
						}
					}
					else if (row < before_row)
					{
						for (int i = before_row - 1; i >= row; i--)
						{
							for (int j = 0; j < m_itemWidgetTable[i].size(); j++)
							{
								delete m_itemWidgetTable[i][j];
							}
						}
						m_itemWidgetTable.resize(row);
					}

					m_tableSize = Size(row, col);

					if (m_itemWidgetTable.size() != 0)
					{
						auto before_col = m_itemWidgetTable[0].size();
						if (col > before_col)
						{
							int temp = col - before_col;
							for (int i = 0; i < m_itemWidgetTable.size(); i++)
							{
								m_itemWidgetTable[i].resize(col);
								for (int j = before_col; j < col; j++)
								{
									auto nwidget = createItemWidget();
									nwidget->setGroupid(GroupID::gItemWidget);
									nwidget->bindParentWidget(this);
									m_itemWidgetTable[i][j] = nwidget;
								}
							}
						}
						else if (col < before_col)
						{
							for (int i = 0; i < m_itemWidgetTable.size(); i++)
							{
								for (int j = before_col - 1; j >= col; j--)
								{
									delete m_itemWidgetTable[i][j];
								}
								m_itemWidgetTable[i].resize(col, nullptr);
							}
						}
					}

					_Adjust_Item_Widget_Layout();
				}

				void _Adjust_Item_Widget_Layout()
				{
					auto table_size = getTableSize();
					int y = 0;
					int x = 0;

					if (table_size.x > 0 && table_size.y > 0)
					{
						for (int i = 0; i < table_size.x; i++)
						{
							// for every row
							x = 0;
							for (int j = 0; j < table_size.y; j++)
							{
								// for every col
								Rect rect = Rect(x, y, m_colWidth[j], m_rowHeight[i]);
								if (!m_itemWidgetTable[i][j]->getRect().equals(rect))
									m_itemWidgetTable[i][j]->setRect(rect);

								auto index = getModel()->getModelIndex(i, j, ModelIndex());
								m_itemWidgetTable[i][j]->setIndex(index);

								x += m_colWidth[j];
							}
							y += m_rowHeight[i];
						}
					}
					setScrollArea(Size(x, y) + _Get_Table_Item_Offset());
				}

				Offset _Get_Table_Item_Offset()const
				{
					//Size offset;
					//offset.y = m_enableTableColLabel ? m_labelSize.y : 0;
					//offset.x = m_enableTableRowLabel ? m_labelSize.x : 0;
					//return offset;
					return Offset(0, 0);
				}


				Rect _Get_Visible_Item_Range()
				{
					auto table_size = getTableSize();
					if (!(table_size.x > 0 && table_size.y > 0))
					{
						return Rect(0, 0, 0, 0);
					}
					Rect range;
					Offset label_offset = _Get_Table_Item_Offset();
					{
						int min_one = 0;
						int max_one = table_size.x;
						{
							int lower_bound = table_size.x - 1;
							int upper_bound = 0;
							int temp = 0;
							int bottom = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).y + label_offset.y;
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								bottom = m_itemWidgetTable[temp][0]->getRect().bottom();
								if (pos < bottom)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							min_one = lower_bound;
						}
						{
							int lower_bound = table_size.x - 1;
							int upper_bound = 0;
							int temp = 0;
							int top = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).y + getHeight();
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								top = m_itemWidgetTable[temp][0]->getRect().bottom(); //
								if (pos < top)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							max_one = lower_bound + 1;
						}
						min_one = math::clamp(min_one, 0, (int)table_size.x);
						max_one = math::clamp(max_one, 0, (int)table_size.x);
						range.mY = min_one;
						range.mHeight = max_one - min_one;
					}
					{
						int min_one = 0;
						int max_one = table_size.y;
						{
							int lower_bound = table_size.y - 1;
							int upper_bound = 0;
							int temp = 0;
							int bottom = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).x + label_offset.x;
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								bottom = m_itemWidgetTable[0][temp]->getRect().right();
								if (pos < bottom)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							min_one = lower_bound;
						}
						{
							int lower_bound = table_size.y - 1;
							int upper_bound = 0;
							int temp = 0;
							int top = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).x + getWidth();
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								top = m_itemWidgetTable[0][temp]->getRect().right(); //
								if (pos < top)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							max_one = lower_bound + 1;
						}
						min_one = math::clamp(min_one, 0, (int)table_size.y);
						max_one = math::clamp(max_one, 0, (int)table_size.y);
						range.mX = min_one;
						range.mWidth = max_one - min_one;
					}
					return range;
				}



			protected:
				virtual void onModelUpdated(AbstractItemModel* model)
				{
					setModel(model);
				}
				virtual void onModelDeleted(AbstractItemModel* model) override
				{
					setModel(nullptr);
				}
				virtual void onItemUpdated(const ModelIndex& index) override
				{
					m_itemWidgetTable[index.getRow()][index.getCol()]->setIndex(index);
				}
				virtual void onRowsInserted(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					Size default_size = getDefaultItemWidgetSize();
					Size table_size = getTableSize();
					int before_row = table_size.x;
					m_rowHeight.resize(getModel()->getRowCount());
					for (int i = row; i < before_row; i++)
						m_rowHeight[i + count] = m_rowHeight[i];
					for (int i = row; i < row + count; i++)
						m_rowHeight[i] = default_size.y;
					_Resize_Table(getModel()->getRowCount(), getModel()->getColCount());
				}
				virtual void onColsInserted(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					Size default_size = getDefaultItemWidgetSize();
					Size table_size = getTableSize();
					int before_col = table_size.y;
					m_colWidth.resize(getModel()->getColCount());
					for (int i = col; i < before_col; i++)
						m_colWidth[i + count] = m_colWidth[i];
					for (int i = col; i < col + count; i++)
						m_colWidth[i] = default_size.x;
					_Resize_Table(getModel()->getRowCount(), getModel()->getColCount());
				}
				virtual void onRowsRemoved(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					Size table_size = getTableSize();
					int before_row = table_size.x;
					for (int i = row + count; i < before_row; i++)
						m_rowHeight[i - count] = m_rowHeight[i];
					m_rowHeight.resize(getModel()->getRowCount());
					_Resize_Table(getModel()->getRowCount(), getModel()->getColCount());
				}
				virtual void onColsRemoved(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					Size table_size = getTableSize();
					int before_col = table_size.y;
					for (int i = col + count; i < before_col; i++)
						m_colWidth[i - count] = m_colWidth[i];
					m_colWidth.resize(getModel()->getColCount());
					_Resize_Table(getModel()->getRowCount(), getModel()->getColCount());
				}
				virtual void onRowsMoved(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) override final
				{
					_Set_Invalid_Current_Index();
					if (destinationRow < sourceRow)
					{
						core::ArrayList<int> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_list[i] = m_rowHeight[j];
						int beg = sourceRow - 1;
						int end = destinationRow;
						for (int i = beg; i >= end; i--)
							m_rowHeight[i + count] = m_rowHeight[i];
						for (int i = 0, j = destinationRow; i < count; i++, j++)
							m_rowHeight[j] = temp_list[i];
					}
					else if (destinationRow > sourceRow + count)
					{
						core::ArrayList<int> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceRow; i < count; i++, j++)
							temp_list[i] = m_rowHeight[j];
						int beg = sourceRow + count;
						int end = destinationRow;
						for (int i = beg; i < end; i++)
							m_rowHeight[i - count] = m_rowHeight[i];
						for (int i = 0, j = destinationRow - count; i < count; i++, j++)
							m_rowHeight[j] = temp_list[i];
					}
					_Resize_Table(getModel()->getRowCount(), getModel()->getColCount());
				}
				virtual void onColsMoved(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) override final
				{
					_Set_Invalid_Current_Index();
					if (destinationCol < sourceCol)
					{
						core::ArrayList<int> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceCol; i < count; i++, j++)
							temp_list[i] = m_colWidth[j];
						int beg = sourceCol - 1;
						int end = destinationCol;
						for (int i = beg; i >= end; i--)
							m_colWidth[i + count] = m_colWidth[i];
						for (int i = 0, j = destinationCol; i < count; i++, j++)
							m_colWidth[j] = temp_list[i];
					}
					else if (destinationCol > sourceCol + count)
					{
						core::ArrayList<int> temp_list;
						temp_list.resize(count);
						for (int i = 0, j = sourceCol; i < count; i++, j++)
							temp_list[i] = m_colWidth[j];
						int beg = sourceCol + count;
						int end = destinationCol;
						for (int i = beg; i < end; i++)
							m_colWidth[i - count] = m_colWidth[i];
						for (int i = 0, j = destinationCol - count; i < count; i++, j++)
							m_colWidth[j] = temp_list[i];
					}
					_Resize_Table(getModel()->getRowCount(), getModel()->getColCount());
				}

			};


			/*
			 *Accept Tree Model Only
			*/
			class AbstractTreeView : public AbstractItemView
			{
			private:
				core::ArrayList<uint32_t> m_colWidth;


				class AbstractItemWidgetContainer : public Widget
				{
				public:
					AbstractTreeView* m_itemView;
					AbstractItemWidgetContainer* m_parentNode;
					int m_preserveWidth;
					AbstractItemWidgetContainer(AbstractTreeView* itemView, AbstractItemWidgetContainer* parentNode)
						: Widget(nullptr), m_itemView(itemView), m_parentNode(parentNode)
					{
						this->bindParentWidget(parentNode);
					}
					AbstractItemWidgetContainer(AbstractTreeView* itemView)
						: Widget(nullptr), m_itemView(itemView), m_parentNode(nullptr)
					{
						this->bindParentWidget(itemView);
					}
					bool isRoot()const { return this->m_parentNode == nullptr; }
					AbstractItemWidgetContainer* getParentNode() { return this->m_parentNode; }
					AbstractTreeView* getTreeView() { return this->m_itemView; }
					int getPreserveWidth() const { return this->m_preserveWidth; }
				public:
					virtual AbstractItemWidgetContainer* getChildItemWidgetContainerAtRow(int row) = 0;
					virtual AbstractItemWidget* getItemWidgetAtCol(int col) = 0;

					virtual void _Clear_Children() = 0;
					virtual void _Reset_Item_Model_Index(const ModelIndex& index) = 0;
					virtual void _Calculate_Preserve_Width() = 0;
					virtual void _Set_Height_PosY() = 0;
					virtual void _Set_Width_PosX() = 0;
					virtual void _Item_Widget_Index_Changed(const ModelIndex& index) = 0;
					virtual void _Item_Widget_Layout_Changed() = 0;
					virtual void _Child_Extend_State_Changed() = 0;
				private:
					virtual void bindChildWidget(Widget* child)override final
					{
						// do nothing, only accept group 0
						Widget::bindChildWidget(child);
					}
				};

				class ItemWidgetContainer :public AbstractItemWidgetContainer
				{
				private:
					PushButton* m_expandButton;
					bool m_isExtended = false;
					core::ArrayList<AbstractItemWidget*> m_itemWidgetList;
					core::ArrayList<ItemWidgetContainer*> m_childContainerList;

					void onExtendButtonClicked()
					{
						m_isExtended = !m_isExtended;
						_Set_Height_PosY();
						_Set_Width_PosX();
						_Item_Widget_Layout_Changed();
						getParentNode()->_Child_Extend_State_Changed();
						if (m_isExtended)
						{
							auto icon = GuiAssetsManager::loadImage("Triangle_Down");
							m_expandButton->setIcon(icon, getPalette().mForegroundColor);
						}
						else
						{
							auto icon = GuiAssetsManager::loadImage("Triangle_Right");
							m_expandButton->setIcon(icon, getPalette().mForegroundColor);
						}
					}

				public:
					ItemWidgetContainer(AbstractTreeView* view, AbstractItemWidgetContainer* parentNode) :AbstractItemWidgetContainer(view, parentNode)
					{
						m_expandButton = new PushButton(nullptr);
						m_expandButton->bindParentWidget(this);
						auto icon = GuiAssetsManager::loadImage("Triangle_Right");
						m_expandButton->setIcon(icon, getPalette().mForegroundColor);
						craft_engine_gui_connect(m_expandButton, clicked, this, onExtendButtonClicked);
					}

					~ItemWidgetContainer()
					{
						delete m_expandButton;
						this->_Clear_Children();
					}

					bool isExtendable()const { return m_childContainerList.size() > 0; }

					virtual AbstractItemWidgetContainer* getChildItemWidgetContainerAtRow(int row)
					{
						return m_childContainerList[row];
					}
					virtual AbstractItemWidget* getItemWidgetAtCol(int col)
					{
						return m_itemWidgetList[col];
					}

					virtual void _Clear_Children()override final
					{
						for (auto& it : m_childContainerList)
							delete it;
						m_childContainerList.clear();
						for (auto& it : m_itemWidgetList)
							delete it;
						m_itemWidgetList.clear();
					}

					virtual void _Reset_Item_Model_Index(const ModelIndex& index)override final
					{
						m_childContainerList.resize(index.getChildrenCount());
						for (int i = 0; i < m_childContainerList.size(); i++)
						{
							m_childContainerList[i] = new ItemWidgetContainer(getTreeView(), this);
							m_childContainerList[i]->_Reset_Item_Model_Index(index.getChildIndex(i, 0));
						}

						m_itemWidgetList.resize(index.getItemModel()->getColCount());
						for (int i = 0; i < m_itemWidgetList.size(); i++)
						{
							auto nwidget = getTreeView()->createItemWidget();
							nwidget->setGroupid(GroupID::gItemWidget);
							nwidget->bindParentWidget(this);
							m_itemWidgetList[i] = nwidget;
						}
					}

					virtual void _Calculate_Preserve_Width()override final
					{
						m_preserveWidth = this->getParentNode()->getPreserveWidth();
						for (int i = 0; i < m_childContainerList.size(); i++)
							m_childContainerList[i]->_Calculate_Preserve_Width();
					}
					virtual void _Set_Height_PosY()override final
					{
						auto default_height = this->getTreeView()->getDefaultItemWidgetSize().y;
						int height = 0;
						height += default_height;
						if (m_isExtended)
						{
							for (int i = 0; i < m_childContainerList.size(); i++)
							{
								m_childContainerList[i]->_Set_Height_PosY();
								m_childContainerList[i]->setRect(Rect(0, height, 0, m_childContainerList[i]->getHeight()));
								height += m_childContainerList[i]->getHeight();
							}
						}
						this->setSize(Size(this->getWidth(), height));
					}
					virtual void _Set_Width_PosX()override final
					{
						auto default_height = this->getTreeView()->getDefaultItemWidgetSize().y;
						auto default_button_width = default_height;
						m_expandButton->setRect(Rect(0, 0, default_button_width, default_height));

						auto button_with_first_col_width = this->getWidth() - this->getPreserveWidth(); // total width of expand button and first item widget
						auto x = button_with_first_col_width;
						for (int i = 1; i < m_itemWidgetList.size(); i++)
						{
							auto col_width = this->getTreeView()->m_colWidth[i];
							m_itemWidgetList[i]->setRect(Rect(x, 0, col_width, default_height));
							x += col_width;
						}
						if (m_itemWidgetList.size() > 0)
						{
							if (button_with_first_col_width < default_button_width)
							{
								m_itemWidgetList[0]->setRect(Rect(0, 0, 0, default_height));
								m_expandButton->setPos(Point(-default_button_width, 0));
							}
							else
								m_itemWidgetList[0]->setRect(Rect(default_button_width, 0, button_with_first_col_width - default_button_width, default_height));
						}
						if (m_isExtended)
						{
							auto child_width = this->getWidth() - default_button_width;
							if (child_width < this->getPreserveWidth())
								child_width = this->getPreserveWidth();
							auto child_x = this->getWidth() - child_width;
							for (int i = 0; i < m_childContainerList.size(); i++)
							{
								m_childContainerList[i]->setRect(Rect(child_x, m_childContainerList[i]->getY(), child_width, m_childContainerList[i]->getHeight()));
								m_childContainerList[i]->_Set_Width_PosX();
							}
						}
					}
					virtual void _Item_Widget_Index_Changed(const ModelIndex& index)override final
					{
						for (int i = 0; i < m_itemWidgetList.size(); i++)
							m_itemWidgetList[i]->setIndex(index.getSiblingIndexAtColumn(i));
						for (int i = 0; i < m_childContainerList.size(); i++)
							m_childContainerList[i]->_Item_Widget_Index_Changed(index.getChildIndex(i, index.getCol()));
					}
					virtual void _Item_Widget_Layout_Changed()override final
					{
						for (int i = 0; i < m_itemWidgetList.size(); i++)
							m_itemWidgetList[i]->onItemChanged();
						for (int i = 0; i < m_childContainerList.size(); i++)
							m_childContainerList[i]->_Item_Widget_Layout_Changed();
					}
					virtual void _Child_Extend_State_Changed()override final
					{
						auto height = getTreeView()->getDefaultItemWidgetSize().y;
						for (int i = 0; i < m_childContainerList.size(); i++)
						{
							m_childContainerList[i]->setPos(Point(m_childContainerList[i]->getX(), height));
							height += m_childContainerList[i]->getHeight();
						}
						this->setSize(Size(getWidth(), height));
						getParentNode()->_Child_Extend_State_Changed();
					}
				public:

					virtual Widget* onDetect(const MouseEvent& mouseEvent)override final
					{
						if (this->isHide() || this->isTransparent() || !getRect().inside(mouseEvent.local))
							return nullptr;

						Widget* hit;
						MouseEvent subEvent = mouseEvent;

						if (isExtendable())
						{
							subEvent.local = mouseEvent.local - this->getPos() - this->getChildOriginal(this->m_expandButton->getGroupid());
							hit = this->m_expandButton->onMouseEvent(subEvent);
							if (hit != nullptr)
								return hit;
						}
						int row = -1;
						int col = -1;
						subEvent.local = mouseEvent.local - this->getPos();
						if (m_itemWidgetList.size() > 0)
						{
							int lower_bound = 0;
							int upper_bound = m_itemWidgetList.size() - 1;
							int pos = subEvent.local.x;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_itemWidgetList[temp]->getRect().right() < pos) lower_bound = temp + 1;
								else if (m_itemWidgetList[temp]->getRect().left() >= pos) upper_bound = temp - 1;
								else break;
							}
							col = temp;
							if (col >= 0)
							{
								hit = this->m_itemWidgetList[col]->onMouseEvent(subEvent);
								if (hit != nullptr)
									return hit;
							}
						}
						if (m_childContainerList.size() > 0)
						{
							int lower_bound = 0;
							int upper_bound = m_childContainerList.size() - 1;
							int pos = subEvent.local.y;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_childContainerList[temp]->getRect().bottom() < pos) lower_bound = temp + 1;
								else if (m_childContainerList[temp]->getRect().top() >= pos) upper_bound = temp - 1;
								else break;
							}
							row = temp;
							if (row >= 0)
							{
								hit = this->m_childContainerList[row]->onMouseEvent(subEvent);
								if (hit != nullptr)
									return hit;
							}
						}
						return this;
					}

					virtual void onPaintEvent() override
					{
						auto painter = getPainter();
						switch (getPalette().mFillMode)
						{
						case Palette::FillMode::eFillMode_None:
							break;
						case Palette::FillMode::eFillMode_Color:
							if (isExtendable())
							{
								if (m_isExtended)
									painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
								else
									if (isHighlight())
										painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
									else
										painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundColor);
							}
							break;
						case Palette::FillMode::eFillMode_Image:
							painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundImage);
							break;
						default:
							break;
						}

						Point global_basepoint       = painter.getGlobalBasepoint();
						Rect child_global_limit_rect = painter.getGlobalLimit();
						if (child_global_limit_rect.isNull())
							return;

						if (isExtendable())
							drawChild(m_expandButton);
						if (m_itemWidgetList.size() > 0)
						{
							int min_one = 0;
							int max_one = m_itemWidgetList.size() - 1;
							{
								int lower_bound = m_itemWidgetList.size() - 1;
								int upper_bound = 0;
								int temp = 0;
								int right = 0;
								int pos = child_global_limit_rect.left();
								while (upper_bound < lower_bound)
								{
									temp = (upper_bound + lower_bound) / 2;
									auto temp_view = m_itemWidgetList[temp];
									right = temp_view->getRect().right() + global_basepoint.x;
									if (pos < right)
										lower_bound = temp;
									else
										upper_bound = temp + 1;
								}
								min_one = lower_bound;
							}
							{
								int lower_bound = m_itemWidgetList.size() - 1;
								int upper_bound = 0;
								int temp = 0;
								int left = 0;
								int pos = child_global_limit_rect.right();
								while (upper_bound < lower_bound)
								{
									temp = (upper_bound + lower_bound) / 2;
									auto temp_view = m_itemWidgetList[temp];
									left = temp_view->getRect().right() + global_basepoint.x;
									if (pos < left)
										lower_bound = temp;
									else
										upper_bound = temp + 1;
								}
								max_one = lower_bound + 1;//
							}

							min_one = math::clamp(min_one, 0, (int)m_itemWidgetList.size());
							max_one = math::clamp(max_one, 0, (int)m_itemWidgetList.size());

							for (int i = min_one; i < max_one; i++)
							{
								if (m_itemWidgetList[i]->isHide() || m_itemWidgetList[i]->isPopupable())
									continue;
								drawChild(m_itemWidgetList[i]);
							}
						}

						if (isExtendable() && m_isExtended)
						{
							int min_one = 0;
							int max_one = m_childContainerList.size() - 1;
							{
								int lower_bound = m_childContainerList.size() - 1;
								int upper_bound = 0;
								int temp = 0;
								int bottom = 0;
								int pos = child_global_limit_rect.top();
								while (upper_bound < lower_bound)
								{
									temp = (upper_bound + lower_bound) / 2;
									auto temp_view = m_childContainerList[temp];
									bottom = temp_view->getRect().bottom() + global_basepoint.y;
									if (pos < bottom)
										lower_bound = temp;
									else
										upper_bound = temp + 1;
								}
								min_one = lower_bound;
							}
							{
								int lower_bound = m_childContainerList.size() - 1;
								int upper_bound = 0;
								int temp = 0;
								int top = 0;
								int pos = child_global_limit_rect.bottom();
								while (upper_bound < lower_bound)
								{
									temp = (upper_bound + lower_bound) / 2;
									auto temp_view = m_childContainerList[temp];
									top = temp_view->getRect().bottom() + global_basepoint.y;
									if (pos < top)
										lower_bound = temp;
									else
										upper_bound = temp + 1;
								}
								max_one = lower_bound + 1;
							}

							min_one = math::clamp(min_one, 0, (int)m_childContainerList.size());
							max_one = math::clamp(max_one, 0, (int)m_childContainerList.size());

							for (int i = min_one; i < max_one; i++)
							{
								if (m_childContainerList[i]->isHide() || m_childContainerList[i]->isPopupable())
									continue;
								drawChild(m_childContainerList[i]); // drawChild2
							}
						}

					}

				};

				class RootItemWidgetContainer :public AbstractItemWidgetContainer
				{
				private:
					core::ArrayList<ItemWidgetContainer*> m_childContainerList;
					AbstractItemModel* m_model;
				public:
					RootItemWidgetContainer(AbstractTreeView* view) :AbstractItemWidgetContainer(view)
					{


					}
					~RootItemWidgetContainer()
					{
						_Clear_Children();
					}

					virtual AbstractItemWidgetContainer* getChildItemWidgetContainerAtRow(int row)
					{
						return m_childContainerList[row];
					}
					virtual AbstractItemWidget* getItemWidgetAtCol(int col)
					{
						return nullptr;
					}

					virtual void _Clear_Children()override final
					{
						for (auto& it : m_childContainerList)
							delete it;
						m_childContainerList.clear();
					}
					virtual void _Reset_Item_Model_Index(const ModelIndex& index)override final
					{
						_Clear_Children();
						m_childContainerList.resize(index.getChildrenCount());
						for (int i = 0; i < m_childContainerList.size(); i++)
						{
							m_childContainerList[i] = new ItemWidgetContainer(getTreeView(), this);
							m_childContainerList[i]->_Reset_Item_Model_Index(index.getChildIndex(i, index.getCol()));
						}
					}
					virtual void _Calculate_Preserve_Width()override final
					{
						m_preserveWidth = 0;
						for (int i = 1; i < getTreeView()->m_colWidth.size(); i++)
							m_preserveWidth += getTreeView()->m_colWidth[i];
						for (int i = 0; i < m_childContainerList.size(); i++)
							m_childContainerList[i]->_Calculate_Preserve_Width();
					}
					virtual void _Set_Height_PosY()override final
					{
						auto default_height = this->getTreeView()->getDefaultItemWidgetSize().y;
						int height = 0;
						for (int i = 0; i < m_childContainerList.size(); i++)
						{
							m_childContainerList[i]->_Set_Height_PosY();
							m_childContainerList[i]->setRect(Rect(0, height, 0, m_childContainerList[i]->getHeight()));
							height += m_childContainerList[i]->getHeight();
						}
						this->setRect(Rect(0, 0, 0, height));
					}
					virtual void _Set_Width_PosX()override final
					{
						auto default_height = this->getTreeView()->getDefaultItemWidgetSize().y;
						auto default_button_width = default_height;
						auto this_width = 0;
						for (int i = 0; i < getTreeView()->m_colWidth.size(); i++)
							this_width += getTreeView()->m_colWidth[i];
						this->setRect(Rect(0, this->getY(), this_width, this->getHeight()));
						auto child_width = this->getWidth();
						auto child_x = 0;
						for (int i = 0; i < m_childContainerList.size(); i++)
						{
							m_childContainerList[i]->setRect(Rect(child_x, m_childContainerList[i]->getY(), child_width, m_childContainerList[i]->getHeight()));
							m_childContainerList[i]->_Set_Width_PosX();
						}
					}
					virtual void _Item_Widget_Index_Changed(const ModelIndex& index)override final
					{
						for (int i = 0; i < m_childContainerList.size(); i++)
							m_childContainerList[i]->_Item_Widget_Index_Changed(index.getChildIndex(i, index.getCol()));
					}
					virtual void _Item_Widget_Layout_Changed()override final
					{
						for (int i = 0; i < m_childContainerList.size(); i++)
							m_childContainerList[i]->_Item_Widget_Layout_Changed();
					}
					virtual void _Child_Extend_State_Changed()override final
					{
						auto height = 0;
						for (int i = 0; i < m_childContainerList.size(); i++)
						{
							m_childContainerList[i]->setPos(Point(m_childContainerList[i]->getX(), height));
							height += m_childContainerList[i]->getHeight();
						}
						this->setSize(Size(getWidth(), height));
						this->getTreeView()->setScrollArea(this->getSize());
					}

					virtual Widget* onDetect(const MouseEvent& mouseEvent)override final
					{
						if (this->isHide() || this->isTransparent() || !getRect().inside(mouseEvent.local))
							return nullptr;

						Widget* hit;
						MouseEvent subEvent = mouseEvent;

						int row = -1;
						subEvent.local = mouseEvent.local - this->getPos();
						if (m_childContainerList.size() > 0)
						{
							int lower_bound = 0;
							int upper_bound = m_childContainerList.size() - 1;
							int pos = subEvent.local.y;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_childContainerList[temp]->getRect().bottom() < pos)
									lower_bound = temp + 1;
								else if (m_childContainerList[temp]->getRect().top() >= pos)
									upper_bound = temp - 1;
								else
									break;
							}
							row = temp;
							if (row >= 0)
							{
								hit = this->m_childContainerList[row]->onMouseEvent(subEvent);
								if (hit != nullptr)
									return hit;
							}
						}

						//for (int i = this->m_childContainerList.size() - 1; i >= 0; i--)
						//{
						//	subEvent.local = mouseEvent.local - this->getPos() - this->getChildOriginal(this->m_childContainerList[i]->getGroupid());
						//	hit = this->m_childContainerList[i]->onMouseEvent(subEvent);
						//	if (hit != nullptr)
						//		return hit;
						//}
						return this;
					}

					virtual void onPaintEvent() override
					{
						auto painter = getPainter();

						Point global_basepoint = painter.getGlobalBasepoint();
						Rect child_global_limit_rect = painter.getGlobalLimit();
						if (child_global_limit_rect.isNull())
							return;

						{
							int min_one = 0;
							int max_one = m_childContainerList.size() - 1;
							{
								int lower_bound = m_childContainerList.size() - 1;
								int upper_bound = 0;
								int temp = 0;
								int bottom = 0;
								int pos = child_global_limit_rect.top();
								while (upper_bound < lower_bound)
								{
									temp = (upper_bound + lower_bound) / 2;
									auto temp_view = m_childContainerList[temp];
									bottom = temp_view->getRect().bottom() + global_basepoint.y;
									if (pos < bottom)
										lower_bound = temp;
									else
										upper_bound = temp + 1;
								}
								min_one = lower_bound;
							}
							{
								int lower_bound = m_childContainerList.size() - 1;
								int upper_bound = 0;
								int temp = 0;
								int top = 0;
								int pos = child_global_limit_rect.bottom();
								while (upper_bound < lower_bound)
								{
									temp = (upper_bound + lower_bound) / 2;
									auto temp_view = m_childContainerList[temp];
									top = temp_view->getRect().top() + global_basepoint.y;
									if (pos < top)
										lower_bound = temp;
									else
										upper_bound = temp + 1;
								}
								max_one = lower_bound + 1;
							}

							min_one = math::clamp(min_one, 0, (int)m_childContainerList.size());
							max_one = math::clamp(max_one, 0, (int)m_childContainerList.size());

							for (int i = min_one; i < max_one; i++)
							{
								if (m_childContainerList[i]->isHide() || m_childContainerList[i]->isPopupable())
									continue;
								drawChild(m_childContainerList[i]);
							}

						}

					}


				public:

					void resetCurrentModel()
					{
						_Clear_Children();
						auto model = getTreeView()->getModel();
						if (model != nullptr)
						{
							auto row_count = model->getRowCount();
							if (row_count > 0)
							{
								auto root_index = model->getModelIndex(0, 0, ModelIndex());
								_Reset_Item_Model_Index(root_index);
								_Calculate_Preserve_Width();
								_Set_Height_PosY();
								_Set_Width_PosX();
								_Item_Widget_Index_Changed(root_index);
							}
						}
					}

					void resetCurrentLayout()
					{
						auto model = getTreeView()->getModel();
						if (model != nullptr)
						{
							auto row_count = model->getRowCount();
							if (row_count > 0)
							{
								auto root_index = model->getModelIndex(0, 0, ModelIndex());
								_Reset_Item_Model_Index(root_index);
								_Calculate_Preserve_Width();
								_Set_Height_PosY();
								_Set_Width_PosX();
								_Item_Widget_Index_Changed(root_index);
							}
						}
					}
				};



				RootItemWidgetContainer* m_rootItemWidgetContainer;
				ModelIndex m_currentIndex;
			public:
				AbstractTreeView(Widget* parent) :AbstractItemView(parent), m_rootItemWidgetContainer(nullptr), m_currentIndex()
				{
					m_rootItemWidgetContainer = new RootItemWidgetContainer(this);
				}
				~AbstractTreeView()
				{
					delete m_rootItemWidgetContainer;
				}

				virtual void setModel(AbstractItemModel* model) override final
				{
					m_colWidth.clear();
					m_colWidth.resize(model->getColCount(), getDefaultItemWidgetSize().x);

					AbstractItemView::setModel(model);
					_Set_Invalid_Current_Index();
					m_rootItemWidgetContainer->resetCurrentModel();
					setScrollArea(m_rootItemWidgetContainer->getSize());
				}

				void setColWidth(int col, int width)
				{
					if (col < 0 || col >= m_colWidth.size())
						craft_engine_error_v2("Invalid param \'col\'\n");
					m_colWidth[col] = width;
					m_rootItemWidgetContainer->resetCurrentLayout();
					setScrollArea(m_rootItemWidgetContainer->getSize());
				}

				virtual AbstractItemWidget* getItemWidget(const ModelIndex& index) override final
				{
					if (!index.isValid())
						return nullptr;

					core::ArrayList<int> path;
					auto copy_index = index;
					auto root_index = getModel()->getModelIndex(-1, -1, ModelIndex());
					while (copy_index != root_index)
					{
						path.push_back(copy_index.getRow());
						copy_index = getModel()->getParentModelIndex(copy_index);
					}
					if (path.size() == 0)
						craft_engine_error_v2("Invalid param \'index\'\n");

					AbstractItemWidgetContainer* node = m_rootItemWidgetContainer;
					for (int i = path.size() - 1; i >= 0; i--)
						node = node->getChildItemWidgetContainerAtRow(path[i]);
					return node->getItemWidgetAtCol(index.getCol());
				}

				virtual ModelIndex getCurrentIndex()const override final
				{
					return m_currentIndex;
				}

				virtual void setCurrentIndex(const ModelIndex& index) override
				{
					auto old_index = getCurrentIndex();
					if (old_index.isValid())
						getItemWidget(old_index)->setSelected(false);
					if (!index.isValid())
					{
						m_currentIndex = ModelIndex();
						return;
					}
					m_currentIndex = index;
					getItemWidget(index)->setSelected(true);
				}
			private:

				void _Set_Invalid_Current_Index()
				{
					auto old_index = getCurrentIndex();
					if (old_index.isValid())
					{
						getItemWidget(old_index)->setSelected(false);
						craft_engine_gui_emit(currentIndexChanged, ModelIndex(), ModelIndex());
					}
					old_index = ModelIndex();
				}

				Offset _Get_Table_Item_Offset()const
				{
					//Size offset;
					//offset.y = m_enableTableColLabel ? m_labelSize.y : 0;
					//offset.x = m_enableTableRowLabel ? m_labelSize.x : 0;
					//return offset;
					return Offset(0, 0);
				}

				void _On_Extend_State_Changed()
				{
					m_rootItemWidgetContainer->resetCurrentLayout();
					setScrollArea(m_rootItemWidgetContainer->getSize());
				}

				bool onItemClicked(AbstractItemWidget* widget, const MouseEvent& mouseEvent) override final
				{
					auto old_index = getCurrentIndex();
					bool item_changed = false;
					if (old_index.isValid())
						getItemWidget(old_index)->setSelected(false);
					auto index = widget->getIndex();
					m_currentIndex = index;
					if (index != old_index)
						item_changed = true;
					getItemWidget(index)->setSelected(true);
					craft_engine_gui_emit(itemClicked, index);
					if (item_changed)
						craft_engine_gui_emit(currentIndexChanged, old_index, index);
					return true;
				}

				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override final
				{
					ScrollWidget::onResizeEvent(resizeEvent);
				}

				virtual Widget* onDetect(const MouseEvent& mouseEvent) override final
				{
					if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
						return nullptr;

					Widget* hit;
					hit = onDetectScrollBar(mouseEvent);
					if (hit != nullptr)
						return hit;

					MouseEvent subEvent = mouseEvent;
					subEvent.local = mouseEvent.local - this->getPos() - this->getChildOriginal(this->m_rootItemWidgetContainer->getGroupid());
					hit = this->m_rootItemWidgetContainer->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
					return this;
				}
			protected:

				virtual void onPaintEvent() override
				{
					drawBackground();
					drawChild(m_rootItemWidgetContainer);
					onPaint_drawScrollBar();
					drawFrame();
				}



			protected:
				virtual void onModelUpdated(AbstractItemModel* model)
				{
					setModel(model);
				}
				virtual void onModelDeleted(AbstractItemModel* model)
				{
					setModel(nullptr);
				}
				virtual void onItemUpdated(const ModelIndex& index) override final
				{
					getItemWidget(index)->setIndex(index);
				}
				virtual void onRowsInserted(int row, int count, const ModelIndex& parent = ModelIndex())
				{
					setModel(getModel());
				}
				virtual void onColsInserted(int col, int count, const ModelIndex& parent = ModelIndex())
				{
					setModel(getModel());
				}
				virtual void onRowsRemoved(int row, int count, const ModelIndex& parent = ModelIndex())
				{
					setModel(getModel());
				}
				virtual void onColsRemoved(int col, int count, const ModelIndex& parent = ModelIndex())
				{
					setModel(getModel());
				}
				virtual void onRowsMoved(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count)
				{
					setModel(getModel());
				}
				virtual void onColsMoved(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count)
				{
					setModel(getModel());
				}

			};


			/*
			 *Accept List Model Only
			*/
			class AbstractFlowView :public AbstractItemView
			{
			private:
				core::ArrayList<core::ArrayList<AbstractItemWidget*>> m_itemWidgetTable;
				int m_currentRowIndex = -1;
				Size m_tableSize = Size(0, 1);
				Size m_itemIntervalSize = Size(0, 0);

				bool _Valid_Model_Index(int row, int col){
					if (getModel() == nullptr) return false;
					if ((row * getTableSize().y + col) >= getModel()->getRowCount())return false;
					return true;
				}
				Point _Get_Current_Coord() const { if (m_currentRowIndex >= 0) return _Transfrom_Index_To_Coord(m_currentRowIndex); else return Point(-1); }
				Point _Transfrom_Index_To_Coord(int index) const { if (getModel() == nullptr)return Point(-1, -1); else return Point(index / getTableSize().y, index % getTableSize().y); }
				int _Transfrom_Index_To_CoordRow(int index) const { if (getModel() == nullptr)return -1; else return index / getTableSize().y; }
				int _Transfrom_Index_To_CoordCol(int index) const { if (getModel() == nullptr)return -1; else return index % getTableSize().y; }
				int _Transfrom_Coord_To_Index(Point coord) const { if (getModel() == nullptr)return -1; else return coord.x * getTableSize().y + coord.y; }
			public:
				virtual AbstractItemWidget* getItemWidget(const ModelIndex& index) override { return m_itemWidgetTable[index.getRow()][index.getCol()]; }
				virtual ModelIndex getCurrentIndex() const override
				{
					if (getModel() != nullptr && m_currentRowIndex >= 0)
						return getModel()->getModelIndex(m_currentRowIndex, 0, ModelIndex());
					else
						return ModelIndex();
				}
				virtual void setCurrentIndex(const ModelIndex& index) override
				{
					auto old_index = getCurrentIndex();
					auto old_coord = _Get_Current_Coord();
					if (old_index.isValid())
						m_itemWidgetTable[old_coord.x][old_coord.y]->setSelected(false);
					if (!index.isValid())
					{
						m_currentRowIndex = -1;
						return;
					}
					m_currentRowIndex = index.getRow();
					auto new_coord = _Get_Current_Coord();
					m_itemWidgetTable[new_coord.x][new_coord.y]->setSelected(true);
				}
				int getCurrentRow() const { return m_currentRowIndex; }
				const Size& getTableSize() const { return m_tableSize; }
			public:
				AbstractFlowView(Widget* parent) : AbstractItemView(parent)
				{

				}

				~AbstractFlowView()
				{
					setModel(nullptr);
				}

				virtual void setModel(AbstractItemModel* model) override final
				{
					AbstractItemView::setModel(model);
					_Set_Invalid_Current_Index();
					_Resize_Table();
				}

				void setItemSize(const Size& size)
				{
					setDefaultItemWidgetSize(size);
					this->_Adjust_Item_Widget_Layout();
				}
				const Size& getItemSize() const
				{
					return getDefaultItemWidgetSize();
				}
				void setItemIntervalSize(const Size& size)
				{
					m_itemIntervalSize = size;
					this->_Adjust_Item_Widget_Layout();
				}
				const Size& getItemIntervalSize() const
				{
					return m_itemIntervalSize;
				}
			public:

				virtual Widget* onDetect(const MouseEvent& mouseEvent) override final
				{
					if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
						return nullptr;

					Widget* hit;

					hit = onDetectScrollBar(mouseEvent);
					if (hit != nullptr)
						return hit;

					Point relative_point = mouseEvent.local - getPos();

					Point relative_point_temp = relative_point;
					relative_point -= getChildOriginal(GroupID::gItemWidget);

					auto table_size = getTableSize();
					int row = -1;
					int col = -1;

					if (table_size.x > 0 && table_size.y > 0)
					{
						{
							int lower_bound = 0;
							int upper_bound = table_size.y - 1;
							int pos = relative_point.x;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_itemWidgetTable[0][temp]->getRect().right() < pos)
									lower_bound = temp + 1;
								else if (m_itemWidgetTable[0][temp]->getRect().left() >= pos)
									upper_bound = temp - 1;
								else
									break;
							}
							col = temp;
						}
						{
							int lower_bound = 0;
							int upper_bound = table_size.x - 1;
							int pos = relative_point.y;
							auto temp = 0;
							while (lower_bound <= upper_bound)
							{
								temp = (lower_bound + upper_bound) / 2;
								if (m_itemWidgetTable[temp][0]->getRect().bottom() < pos)
									lower_bound = temp + 1;
								else if (m_itemWidgetTable[temp][0]->getRect().top() >= pos)
									upper_bound = temp - 1;
								else
									break;
							}
							row = temp;
						}
					}
					MouseEvent subEvent = mouseEvent;

					subEvent.local = relative_point_temp - getChildOriginal(GroupID::gItemWidget);
					if (row >= 0 && col >= 0 && _Valid_Model_Index(row, col))
					{
						hit = m_itemWidgetTable[row][col]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}

					return this;
				}
			public:

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					auto range = _Get_Visible_Item_Range();
					int left = range.left();
					int right = range.right();
					int top = range.top();
					int bottom = range.bottom();
					drawBackground();
					for (int i = left; i < right; i++)
						for (int j = top; j < bottom; j++)
							if (_Valid_Model_Index(j, i))
								drawChild(m_itemWidgetTable[j][i]);
					onPaint_drawScrollBar();
					drawFrame();
				}

				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override final 
				{
					ScrollWidget::onResizeEvent(resizeEvent);
					_Adjust_Item_Widget_Layout();
				}

			protected:
				virtual void onKeyBoardEvent(const KeyboardEvent& keyboardEvent)override
				{
					if (keyboardEvent.down)
					{
						auto cur_coord = _Get_Current_Coord();
						auto old_index = getCurrentIndex();
						if (keyboardEvent.key == (KeyBoard::eKey_Up) && cur_coord.x > 0)
						{
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(false);
							cur_coord.x--;
							m_currentRowIndex = _Transfrom_Coord_To_Index(cur_coord);
							if (m_currentRowIndex < 0)
								m_currentRowIndex = 0;
							cur_coord = _Get_Current_Coord();
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						else if (keyboardEvent.key == (KeyBoard::eKey_Down) && cur_coord.x >= 0 && getTableSize().x > 0 && cur_coord.x < getTableSize().x - 1)
						{
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(false);
							cur_coord.x++;
							m_currentRowIndex = _Transfrom_Coord_To_Index(cur_coord);
							if (m_currentRowIndex > getModel()->getRowCount() - 1)
								m_currentRowIndex = getModel()->getRowCount() - 1;
							cur_coord = _Get_Current_Coord();
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(true);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						if (keyboardEvent.key == (KeyBoard::eKey_Left) && m_currentRowIndex > 0)
						{
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(false);
							m_currentRowIndex--;
							cur_coord = _Get_Current_Coord();
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(true);
							m_currentRowIndex = _Transfrom_Coord_To_Index(cur_coord);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
						else if (keyboardEvent.key == (KeyBoard::eKey_Right) && m_currentRowIndex >= 0 && (m_currentRowIndex < getModel()->getRowCount() - 1))
						{
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(false);
							m_currentRowIndex++;
							cur_coord = _Get_Current_Coord();
							m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(true);
							m_currentRowIndex = _Transfrom_Coord_To_Index(cur_coord);
							craft_engine_gui_emit(currentIndexChanged, old_index, getCurrentIndex());
						}
					}
				}

			private:

				bool onItemClicked(AbstractItemWidget* widget, const MouseEvent& mouseEvent) override final
				{
					auto old_index = getCurrentIndex();
					auto old_coord = _Get_Current_Coord();
					bool item_changed = false;
					if (old_index.isValid())
						m_itemWidgetTable[old_coord.x][old_coord.y]->setSelected(false);
					auto index = widget->getIndex();
					if (index != old_index)
						item_changed = true;
					m_currentRowIndex = index.getRow();
					auto new_coord = _Get_Current_Coord();
					m_itemWidgetTable[new_coord.x][new_coord.y]->setSelected(true);
					craft_engine_gui_emit(itemClicked, index);
					if (item_changed)
						craft_engine_gui_emit(currentIndexChanged, old_index, index);
					return true;
				}

				void _Set_Invalid_Current_Index()
				{
					auto old_index = getCurrentIndex();
					auto old_coord = _Get_Current_Coord();
					if (old_index.isValid())
					{
						m_itemWidgetTable[old_coord.x][old_coord.y]->setSelected(false);
						craft_engine_gui_emit(currentIndexChanged, ModelIndex(), ModelIndex());
					}
					m_currentRowIndex = -1;
				}
			private:
				void _Resize_Table()
				{
					_Adjust_Item_Widget_Layout();
				}

				void _Adjust_Item_Widget_Layout()
				{
					auto item_size = this->getDefaultItemWidgetSize();
					auto total_item_count = getModel() != nullptr ? getModel()->getRowCount() : 0;
					auto col_count = math::max(1, (getWidth() - m_itemIntervalSize.x) / (item_size.x + m_itemIntervalSize.x));
					auto row_count = (total_item_count / col_count) + ((total_item_count % col_count) > 0 ? 1 : 0);
					auto table_size = getTableSize();
					auto before_row = m_itemWidgetTable.size();

					core::ArrayList<AbstractItemWidget*> widgets(table_size.x * table_size.y);
					widgets.resize(0);

					for (int i = 0; i < table_size.x; i++)
						for (int j = 0; j < table_size.y; j++)
							widgets.push_back(m_itemWidgetTable[i][j]);

					m_itemWidgetTable.resize(row_count);
					for (int i = 0; i < row_count; i++)
						m_itemWidgetTable[i].resize(col_count);
					m_tableSize = Size(row_count, col_count);


					for (int i = 0; i < m_tableSize.x; i++)
						for (int j = 0; j < m_tableSize.y; j++) {
							if (widgets.size() > 0) {
								m_itemWidgetTable[i][j] = widgets.back();
								widgets.pop_back();
							}else {
								auto nwidget = createItemWidget();
								nwidget->setGroupid(GroupID::gItemWidget);
								nwidget->bindParentWidget(this);
								m_itemWidgetTable[i][j] = nwidget;
							}
						}
					for (auto it : widgets)
						delete it;

					{
						auto table_size = getTableSize();
						auto& item_size = getItemSize();
						auto& interval_size = getItemIntervalSize();
						int x_advance = item_size.x + interval_size.x;
						int y_advance = item_size.y + interval_size.y;
						int x = interval_size.x;
						int y = interval_size.y;

						if (table_size.x > 0 && table_size.y > 0)
						{
							for (int i = 0; i < table_size.x; i++)
							{
								// for every row
								x = interval_size.x;
								for (int j = 0; j < table_size.y; j++)
								{
									// for every col
									Rect rect = Rect(x, y, item_size.x, item_size.y);
									if (!m_itemWidgetTable[i][j]->getRect().equals(rect))
										m_itemWidgetTable[i][j]->setRect(rect);
									if (_Valid_Model_Index(i, j))
									{
										auto index = getModel()->getModelIndex(i * getTableSize().y + j, 0, ModelIndex());
										m_itemWidgetTable[i][j]->setIndex(index);
									}
									else
									{
										m_itemWidgetTable[i][j]->setIndex(ModelIndex());
									}
									m_itemWidgetTable[i][j]->setSelected(false);
									x += x_advance;
								}
								y += y_advance;
							}
						}
						setScrollArea(Size(x_advance * table_size.y + interval_size.x, y)); // 
					}

					if (getModel() != nullptr && m_currentRowIndex >= 0 && m_currentRowIndex < getModel()->getRowCount())
					{
						auto cur_coord = _Get_Current_Coord();
						m_itemWidgetTable[cur_coord.x][cur_coord.y]->setSelected(true);
					}
				}

				// width in col, height in row
				Rect _Get_Visible_Item_Range()
				{
					auto table_size = getTableSize();
					if (!(table_size.x > 0 && table_size.y > 0))
					{
						return Rect(0, 0, 0, 0);
					}
					Rect range;
					Offset label_offset = Offset(0);
					{
						int min_one = 0;
						int max_one = table_size.x;
						{
							int lower_bound = table_size.x - 1;
							int upper_bound = 0;
							int temp = 0;
							int bottom = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).y + label_offset.y;
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								bottom = m_itemWidgetTable[temp][0]->getRect().bottom();
								if (pos < bottom)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							min_one = lower_bound;
						}
						{
							int lower_bound = table_size.x - 1;
							int upper_bound = 0;
							int temp = 0;
							int top = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).y + getHeight();
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								top = m_itemWidgetTable[temp][0]->getRect().bottom(); //
								if (pos < top)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							max_one = lower_bound + 1;
						}
						min_one = math::clamp(min_one, 0, (int)table_size.x);
						max_one = math::clamp(max_one, 0, (int)table_size.x);
						range.mY = min_one;
						range.mHeight = max_one - min_one;
					}
					{
						int min_one = 0;
						int max_one = table_size.y;
						{
							int lower_bound = table_size.y - 1;
							int upper_bound = 0;
							int temp = 0;
							int bottom = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).x + label_offset.x;
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								bottom = m_itemWidgetTable[0][temp]->getRect().right();
								if (pos < bottom)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							min_one = lower_bound;
						}
						{
							int lower_bound = table_size.y - 1;
							int upper_bound = 0;
							int temp = 0;
							int top = 0;
							int pos = -getChildOriginal(GroupID::gItemWidget).x + getWidth();
							while (upper_bound < lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								top = m_itemWidgetTable[0][temp]->getRect().right(); //
								if (pos < top)
									lower_bound = temp;
								else
									upper_bound = temp + 1;
							}
							max_one = lower_bound + 1;
						}
						min_one = math::clamp(min_one, 0, (int)table_size.y);
						max_one = math::clamp(max_one, 0, (int)table_size.y);
						range.mX = min_one;
						range.mWidth = max_one - min_one;
					}
					return range;
				}

			protected:
				virtual void onModelUpdated(AbstractItemModel* model)
				{
					setModel(model);
				}
				virtual void onModelDeleted(AbstractItemModel* model) override
				{
					setModel(nullptr);
				}
				virtual void onItemUpdated(const ModelIndex& index) override
				{
					m_itemWidgetTable[index.getRow() / getTableSize().y][index.getRow() % getTableSize().y]->setIndex(index);
				}
				virtual void onRowsInserted(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					_Resize_Table();
				}
				virtual void onColsInserted(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Resize_Table();
				}
				virtual void onRowsRemoved(int row, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Set_Invalid_Current_Index();
					_Resize_Table();
				}
				virtual void onColsRemoved(int col, int count, const ModelIndex& parent = ModelIndex()) override final
				{
					_Resize_Table();
				}
				virtual void onRowsMoved(const ModelIndex& sourceParent, int sourceRow, const ModelIndex& destinationParent, int destinationRow, int count) override final
				{
					_Set_Invalid_Current_Index();
					_Resize_Table();
				}
				virtual void onColsMoved(const ModelIndex& sourceParent, int sourceCol, const ModelIndex& destinationParent, int destinationCol, int count) override final
				{
					_Resize_Table();
				}

			};



		}


	}

}

