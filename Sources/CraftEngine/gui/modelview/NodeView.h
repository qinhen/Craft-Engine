#pragma once
#include "../widgets/ScrollWidget.h"
#include "../widgets/FrameWidget.h"
#include "../widgets/Label.h"
#include "./NodeModel.h"

namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{

			class NodeView;


			class AbstractNodeWidget : public Widget
			{
			private:
				friend class NodeView;
				NodeView* m_view;
				int       m_instanceID;
				bool      m_isSelected = false;
			public:

				AbstractNodeWidget(NodeView* view, int instanceID) : Widget(Rect(0, 0, 100, 44), nullptr)
				{
					setDragable(true);
					m_view = view;
					m_instanceID = instanceID;
					_On_Create();
				}

				~AbstractNodeWidget()
				{
					for (int i = 0; i < m_input.size(); i++)
						delete m_input[i];
					m_input.clear();
					for (int i = 0; i < m_output.size(); i++)
						delete m_output[i];
					m_output.clear();
				}
			public:
				virtual Point getInputPortCenter(int portId)const = 0;
				virtual Point getOutputPortCenter(int portId)const = 0;
				virtual Point getInputPortPos(int portId)const = 0;
				virtual Point getOutputPortPos(int portId)const = 0;
				virtual Point getInputPortSize(int portId)const = 0;
				virtual Point getOutputPortSize(int portId)const = 0;
				Rect getInputPortRect(int portId)const { return Rect(getInputPortPos(portId), getInputPortSize(portId)); }
				Rect getOutputPortRect(int portId)const { return Rect(getOutputPortPos(portId), getOutputPortSize(portId)); }

				virtual Widget* getPortWidget(const Point& point) final
				{
					Point pos = point - getPos() - getChildOriginal(0);
					if (!m_portBoundingBox.inside(pos))
						return nullptr;
					for (auto it : m_input)
						if (it->getRect().inside(pos))
							return it;
					for (auto it : m_output)
						if (it->getRect().inside(pos))
							return it;
					return nullptr;
				}

				int       getInstanceID() { return m_instanceID; }
				NodeView* getView() { return m_view; }
				void      setSelected(bool enable) { m_isSelected = enable; }
				bool      isSelect() { return m_isSelected; }

			protected:
				core::ArrayList<Widget*> m_input;
				core::ArrayList<Widget*> m_output;
				Rect m_portBoundingBox;
			protected:
				void updatePortWidgetLayout()
				{
					for (int i = 0; i < m_input.size(); i++)
						m_input[i]->setRect(getInputPortRect(i));
					for (int i = 0; i < m_output.size(); i++)
						m_output[i]->setRect(getOutputPortRect(i));
					m_portBoundingBox = Rect();
					if (m_input.size() > 0)
						m_portBoundingBox = m_input[0]->getRect();
					else if (m_output.size() > 0)
						m_portBoundingBox = m_output[0]->getRect();

					for (int i = 1; i < m_input.size(); i++)
						m_portBoundingBox = m_portBoundingBox.conjunction(m_input[i]->getRect());
					for (int i = 0; i < m_output.size(); i++)
						m_portBoundingBox = m_portBoundingBox.conjunction(m_output[i]->getRect());
				}
			protected:
				virtual Widget* onDetectPortWidget(const MouseEvent& mouseEvent) final
				{
					Widget* hit;
					Point relative_point = mouseEvent.local - getPos();
					relative_point -= getChildOriginal(GroupID::gPortWidget);
					MouseEvent subEvent = mouseEvent;
					subEvent.local = relative_point;
					for (int i = m_input.size() - 1; i >= 0; i--) //
					{
						hit = m_input[i]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
					for (int i = m_output.size() - 1; i >= 0; i--) //
					{
						hit = m_output[i]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
					return nullptr;
				}
			public:
				virtual Widget* onDetect(const MouseEvent& mouseEvent) override final
				{
					Widget* hit;
					if (isHide() || isTransparent())
						return nullptr;

					hit = onDetectPortWidget(mouseEvent);
					if (hit != nullptr)
						return hit;

					Point relative_point = mouseEvent.local - getPos();
					relative_point -= getChildOriginal(0);
					MouseEvent subEvent = mouseEvent;
					subEvent.local = relative_point;
					for (int i = getChildrenWidgets().size() - 1; i >= 0; i--) //
					{
						hit = getChildrenWidgets()[i]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
					if (!getRect().inside(mouseEvent.local))
						return nullptr;
					return this;
				}
			public:
				virtual void onPaintEvent() override
				{
					drawBackground();			
					Rect child_rect;
					Rect this_rect = Rect(Offset(0), getSize());
					for (int i = 0; i < getChildrenWidgets().size(); i++)
					{
						if (getChildrenWidgets()[i]->isHide() || getChildrenWidgets()[i]->isPopupable())
							continue;
						child_rect = getChildrenWidgets()[i]->getRect();
						child_rect.mOffset += getChildOriginal(GroupID::gDefaultWidget);
						if (Rect::haveIntersection(this_rect, child_rect))
							drawChild(getChildrenWidgets()[i]);
					}
					drawFrame();
					onPaint_drawPortWidget();
				}
			protected:
				void onPaint_drawPortWidget()
				{
					for (int i = 0; i < m_input.size(); i++)
						drawChild2(m_input[i]);
					for (int i = 0; i < m_output.size(); i++)
						drawChild2(m_output[i]);
				}
			protected:
				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { updatePortWidgetLayout(); }
				virtual void onUpdateNode() = 0;
			private:
				void _On_Create();
			};






			class NodeView : public ScrollWidget
			{
			private:
				friend class AbstractNodeWidget;

				class NodeIOPortWidget : public Widget
				{
				private:
					NodeView* m_view;
					AbstractNodeWidget* m_nodeWidget;
					bool                m_slotActivated = false;
					bool                m_isInputPort;
					int                 m_port;
				public:
					bool        isInputPort()const { return m_isInputPort; }
					int         getPort()const { return m_port; }
					AbstractNodeWidget* getNodeWidget()const { return m_nodeWidget; }

					NodeIOPortWidget(int port, bool isInputPort, NodeView* view, AbstractNodeWidget* parent) : Widget(nullptr)
					{
						setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::ePushButton));
						getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
						setDragable(true);
						m_port = port;
						m_isInputPort = isInputPort;
						m_view = view;
						m_nodeWidget = parent;
					}

					void onPaintEvent() override
					{
						auto painter = getPainter();
						drawBackground();
						if (isFocus())
							painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
						else if (isHighlight())
							painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
						drawFrame();
					}

					virtual void onDrag(const MouseEvent& mouseEvent) override
					{
						m_view->_Handle_Port_Widget_Drag(this, mouseEvent.global, mouseEvent.offset);
					}
					virtual void onDragApply(const MouseEvent& mouseEvent) override
					{
						m_view->_Handle_Port_Widget_Drag_Apply(this, mouseEvent.global, mouseEvent.offset);
					}
				};

				class NodeWidget_Default : public AbstractNodeWidget
				{
				private:
					int32_t   m_titleBarWidth = 22;
					int32_t   m_lineWidth = 22;
					int32_t   m_portLength = 14;
					String    m_titleString;
					Point     m_titleBasepoint;

					struct PortLabel
					{
						String  name;
						int     length;
						Point   basepoint;
					};
					core::ArrayList<PortLabel> m_inPortLabels;
					core::ArrayList<PortLabel> m_outPortLabels;
					Font m_inLabelFont;
					Font m_outLabelFont;
				public:

					NodeWidget_Default(NodeView* nodeView, int instanceID) : AbstractNodeWidget(nodeView, instanceID)
					{
						getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
						getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;

						m_inLabelFont.getAlignment().mHorizonMode = Alignment::eAlignmentMode_Left;
						m_outLabelFont.getAlignment().mHorizonMode = Alignment::eAlignmentMode_Right;

						auto instance = getView()->getModel()->getInstance(getInstanceID());
						m_titleString = instance.title();
						auto& structure = instance.structure();
						auto maxPort = math::max(structure.mPorts.inputs.size(), structure.mPorts.outputs.size());

						m_inPortLabels.resize(structure.mPorts.inputs.size());
						for (int i = 0; i < structure.mPorts.inputs.size(); i++)
						{
							m_inPortLabels[i].name = structure.mPorts.inputs[i].name;
							m_inPortLabels[i].length = GuiFontSystem::calcFontLineWidth(m_inPortLabels[i].name, m_inLabelFont);
						}
						m_outPortLabels.resize(structure.mPorts.outputs.size());
						for (int i = 0; i < structure.mPorts.outputs.size(); i++)
						{
							m_outPortLabels[i].name = structure.mPorts.outputs[i].name;
							m_outPortLabels[i].length = GuiFontSystem::calcFontLineWidth(m_outPortLabels[i].name, m_outLabelFont);
						}
						int max_width = 100;
						for (int i = 0; i < maxPort; i++)
						{
							int w = (i < m_inPortLabels.size() ? m_inPortLabels[i].length : 0) + (i < m_outPortLabels.size() ? m_outPortLabels[i].length : 0) + 20;
							if (w > max_width)
								max_width = w;
						}
						setSize(Size(max_width, m_titleBarWidth + m_lineWidth * maxPort));
					}
				private:
					Size getAvailableSize() const { return Point(getWidth(), getHeight() - m_titleBarWidth); }
					Size getTitleSize() const { return Point(getWidth(), m_titleBarWidth); }
					Point getOriginalPoint() const { return Point(0, m_titleBarWidth); }
					void updateDefaultNodeLayout()
					{
						for (int i = 0; i < m_inPortLabels.size(); i++)
							m_inPortLabels[i].basepoint = GuiFontSystem::calcFontBasePoint(m_inPortLabels[i].name, Rect(Point(0, m_lineWidth * i), Size(m_inPortLabels[i].length, m_lineWidth)), m_inLabelFont);
						for (int i = 0; i < m_outPortLabels.size(); i++)
							m_outPortLabels[i].basepoint = GuiFontSystem::calcFontBasePoint(m_outPortLabels[i].name, Rect(Point(getWidth() - m_outPortLabels[i].length, m_lineWidth * i), Size(m_outPortLabels[i].length, m_lineWidth)), m_outLabelFont);
						m_titleBasepoint = GuiFontSystem::calcFontBasePoint(m_titleString, Rect(Point(0, 0), getTitleSize()), getFont());
					}

				public:
					virtual Point getInputPortCenter(int portId)const override final { return Point(-m_portLength / 2, portId * m_lineWidth + m_lineWidth / 2); }
					virtual Point getOutputPortCenter(int portId)const override final { return Point(getWidth() + m_portLength / 2, portId * m_lineWidth + m_lineWidth / 2); }
					virtual Point getInputPortPos(int portId)const override final { return Point(-m_portLength, (m_lineWidth - m_portLength) / 2 + portId * m_lineWidth); }
					virtual Point getOutputPortPos(int portId)const override final { return Point(getWidth(), (m_lineWidth - m_portLength) / 2 + portId * m_lineWidth); }
					virtual Point getInputPortSize(int portId)const override final { return Size(m_portLength, m_portLength); }
					virtual Point getOutputPortSize(int portId)const override final { return Size(m_portLength, m_portLength); }

					virtual Point getChildOriginal(int groupid = 0)const override final {
						switch (groupid) {
						case 0:return getOriginalPoint();
						case GroupID::gPortWidget:return getOriginalPoint();
						default:return Point(0);
						}
					}

					virtual void onUpdateNode() override final
					{

					}
				protected:
					virtual void onResizeEvent(const ResizeEvent& resizeEvent) override {
						AbstractNodeWidget::onResizeEvent(resizeEvent);
						updateDefaultNodeLayout();
					}

					virtual void onPaintEvent() override
					{
						auto painter = getPainter();
						auto painter2 = getThisPainter();
						drawBackground();
						auto title_global_rect = Rect(Point(0), getTitleSize());
						switch (getPalette().mFillMode)
						{
						case Palette::FillMode::eFillMode_None:
							break;
						case Palette::FillMode::eFillMode_Color:
							if (isInputFocus())
								painter2.drawRect(title_global_rect, getPalette().mBackgroundFocusColor);
							else
								painter2.drawRect(title_global_rect, getPalette().mBackgroundHighLightColor);
							break;
						case Palette::FillMode::eFillMode_Image:
							painter2.drawRect(title_global_rect, getPalette().mBackgroundImage);
							break;
						default:
							break;
						}

						if (m_titleString.size() > 0)
							painter2.drawTextLine(m_titleString, m_titleBasepoint,
								getFont(), getPalette().mForegroundColor);

						for (auto it: m_inPortLabels)
							painter.drawTextLine(it.name, it.basepoint,
								m_inLabelFont, getPalette().mForegroundColor);
						for (auto it : m_outPortLabels)
							painter.drawTextLine(it.name, it.basepoint,
								m_outLabelFont, getPalette().mForegroundColor);

						drawFrame();
						onPaint_drawPortWidget();
					}

					virtual void onDrag(const MouseEvent& mouseEvent) override final
					{
						auto base_point = Widget::getGlobalPos(Point(0), 1) - getDragOffset();
						auto src_global_title_rect = Rect(base_point, getTitleSize());
						if (src_global_title_rect.inside(mouseEvent.global))
							Widget::onDrag(mouseEvent);
					}

				};

				class NodeWidget_Image : public AbstractNodeWidget
				{
				private:
					int32_t   m_lineWidth = 44;
					int32_t   m_portLength = 14;
					int32_t   m_imageLength = 44;
					HandleImage m_image;
					Rect      m_imageRect;
				public:

					NodeWidget_Image(NodeView* nodeView, int instanceID) : AbstractNodeWidget(nodeView, instanceID)
					{
						getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
						getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
						auto instance = getView()->getModel()->getInstance(getInstanceID());
						auto structure = instance.structure();
						int maxPort = math::max(structure.mPorts.inputs.size(), structure.mPorts.outputs.size());
						//m_image = instance.dataList[0].get<HandleImage>();
						setSize(Size(getWidth(), math::max(m_lineWidth * maxPort, m_imageLength)));
					}
				public:
					virtual Point getInputPortCenter(int portId)const override final { return Point(-m_portLength / 2, portId * m_lineWidth + m_lineWidth / 2); }
					virtual Point getOutputPortCenter(int portId)const override final { return Point(getWidth() + m_portLength / 2, portId * m_lineWidth + m_lineWidth / 2); }
					virtual Point getInputPortPos(int portId)const override final { return Point(-m_portLength, (m_lineWidth - m_portLength) / 2 + portId * m_lineWidth); }
					virtual Point getOutputPortPos(int portId)const override final { return Point(getWidth(), (m_lineWidth - m_portLength) / 2 + portId * m_lineWidth); }
					virtual Point getInputPortSize(int portId)const override final { return Size(m_portLength, m_portLength); }
					virtual Point getOutputPortSize(int portId)const override final { return Size(m_portLength, m_portLength); }
				private:
					void updateDefaultNodeLayout()
					{
						m_imageRect = Rect((getWidth() - m_imageLength) / 2, (getHeight() - m_imageLength) / 2, m_imageLength, m_imageLength);
					}
				protected:
					virtual void onUpdateNode() override final
					{
						auto instance = getView()->getModel()->getInstance(getInstanceID());
						m_image = instance.data(0).get<HandleImage>();
						updateDefaultNodeLayout();
					}
					virtual void onResizeEvent(const ResizeEvent& resizeEvent) override {
						AbstractNodeWidget::onResizeEvent(resizeEvent);
						updateDefaultNodeLayout();
					}
					virtual void onPaintEvent() override
					{
						auto painter = getPainter();
						drawBackground();
						painter.drawRect(m_imageRect, m_image);
						drawFrame();
						onPaint_drawPortWidget();
					}

				};

				class NodeWidget_Title : public AbstractNodeWidget
				{
				private:
					int32_t   m_lineWidth = 22;
					int32_t   m_portLength = 14;
					String    m_titleString;
					Point     m_titleBasepoint;
				public:

					NodeWidget_Title(NodeView* nodeView, int instanceID) : AbstractNodeWidget(nodeView, instanceID)
					{
						getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
						getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
						auto instance = getView()->getModel()->getInstance(getInstanceID());
						m_titleString = instance.title();
						auto& structure = getView()->getModel()->getContext()->getStructure(instance.typeID());
						int maxPort = math::max(structure.mPorts.inputs.size(), structure.mPorts.outputs.size());
						maxPort = math::max(1, maxPort);
						setSize(Size(getWidth(), m_lineWidth * maxPort));
					}
				private:
					void updateDefaultNodeLayout()
					{
						m_titleBasepoint = GuiFontSystem::calcFontBasePoint(m_titleString, Rect(Point(0, 0), getSize()), getFont());
					}

				public:
					virtual Point getInputPortCenter(int portId)const override final { return Point(-m_portLength / 2, portId * m_lineWidth + m_lineWidth / 2); }
					virtual Point getOutputPortCenter(int portId)const override final { return Point(getWidth() + m_portLength / 2, portId * m_lineWidth + m_lineWidth / 2); }
					virtual Point getInputPortPos(int portId)const override final { return Point(-m_portLength, (m_lineWidth - m_portLength) / 2 + portId * m_lineWidth); }
					virtual Point getOutputPortPos(int portId)const override final { return Point(getWidth(), (m_lineWidth - m_portLength) / 2 + portId * m_lineWidth); }
					virtual Point getInputPortSize(int portId)const override final { return Size(m_portLength, m_portLength); }
					virtual Point getOutputPortSize(int portId)const override final { return Size(m_portLength, m_portLength); }
				protected:
					virtual void onUpdateNode() override final
					{
						auto instance = getView()->getModel()->getInstance(getInstanceID());
						m_titleString = instance.title();
						updateDefaultNodeLayout();
					}

					virtual void onResizeEvent(const ResizeEvent& resizeEvent) override {
						AbstractNodeWidget::onResizeEvent(resizeEvent);
						updateDefaultNodeLayout();
					}

					virtual void onPaintEvent() override
					{
						auto painter = getPainter();
						drawBackground();
						if (m_titleString.size() > 0)
							painter.drawTextLine(m_titleString, m_titleBasepoint, getFont(), getPalette().mForegroundColor);
						drawFrame();
						onPaint_drawPortWidget();
					}
				};


				struct NodeInstanceViewData
				{
					AbstractNodeWidget* nodeWidget;
					std::unordered_set<int> linkKeyList;
				};

				struct LinkData
				{
					// index data
					NodePortIndex beginPort;
					NodePortIndex endPort;
					// render data
					core::ArrayList<math::vec2> linkPolygon;
					Color linkColor;
					NodePortStyle style;
				};
				std::unordered_map<int, NodeInstanceViewData*> m_nodeInstanceViewData;
				core::ArrayList<AbstractNodeWidget*>           m_nodeInstanceOrder;
				std::unordered_map<int, LinkData*>             m_linkData;
				int m_nextLinkKey = 0;
				Color m_linkColor = GuiColorStyle::getForegroundColor();
				int m_curentInstanceID = -1;
				int m_curentLinkID = -1;
				bool m_autoMoveToFront = false;
			public:
				NodeView(Widget* parent) : ScrollWidget(parent)
				{
					setScrollArea(Rect(-1600, -1600, 3600, 3600));
					// Set mouse event catch flags
					setMouseEventCatchFlags(MouseEvent::eAll);
				}

				~NodeView()
				{
					setModel(nullptr);
				}

				void setModel(NodeModel* model)
				{
					if (m_model != nullptr)
					{
						craft_engine_gui_disconnect(m_model, instanceCreated, m_signalKey);
						craft_engine_gui_disconnect(m_model, instanceDestroyed, m_signalKey);
						craft_engine_gui_disconnect(m_model, linkAdded, m_signalKey);
						craft_engine_gui_disconnect(m_model, linkRemoved, m_signalKey);
						craft_engine_gui_disconnect(m_model, modelDestroyed, m_signalKey);
						craft_engine_gui_disconnect(m_model, instanceUpdated, m_signalKey);
						craft_engine_gui_disconnect(m_model, linksAllRemoved, m_signalKey);
						craft_engine_gui_disconnect(m_model, instancesAllRemoved, m_signalKey);
						m_model = nullptr;
						m_signalKey = -1;
					}
					_Clear_All_Node_Data();
					m_model = model;
					if (m_model != nullptr)
					{
						_Reflash_All_Node_Data();
						m_signalKey =
							craft_engine_gui_connect(m_model, instanceCreated, this, _On_Instance_Created);
						craft_engine_gui_connect(m_model, instanceDestroyed, this, _On_Instance_Destroyed);
						craft_engine_gui_connect(m_model, linkAdded, this, _On_Link_Add);
						craft_engine_gui_connect(m_model, linkRemoved, this, _On_Link_Removed);
						craft_engine_gui_connect(m_model, modelDestroyed, this, _On_Model_Destroyed);
						craft_engine_gui_connect(m_model, instanceUpdated, this, _On_Instance_Updated);
						craft_engine_gui_connect(m_model, linksAllRemoved, this, _On_Links_All_Removed);
						craft_engine_gui_connect(m_model, instancesAllRemoved, this, _On_Instances_All_Removed);
					}
				}


				AbstractNodeWidget* getNodeWidget(int id) { return m_nodeInstanceViewData[id]->nodeWidget; }
				NodeModel* getModel() { return m_model; }
				void setCurrentInstanceID(int id) {
					if (m_curentInstanceID > 0)
						getNodeWidget(getCurrentInstanceID())->setSelected(false);
					if (id >= 0)
					{
						m_curentInstanceID = id;
						auto widget = getNodeWidget(getCurrentInstanceID());
						widget->setSelected(true);
						if (m_autoMoveToFront)
						{
							for (int i = 0; i < m_nodeInstanceOrder.size(); i++)
								if (m_nodeInstanceOrder[i] == widget)
								{
									m_nodeInstanceOrder.swap(m_nodeInstanceOrder.size() - 1, i);
									break;
								}
						}
					}
					else
						m_curentInstanceID = -1;
				}
				int getCurrentInstanceID() const { return m_curentInstanceID; }
				int getCurrentLinkID() const { return m_curentLinkID; }
				void moveNodeToTop(int instanceID)
				{
					auto widget = getNodeWidget(instanceID);
					for (int i = 0; i < m_nodeInstanceOrder.size(); i++)
						if (m_nodeInstanceOrder[i] == widget)
						{
							m_nodeInstanceOrder.swap(m_nodeInstanceOrder.size() - 1, i);
							break;
						}
				}
				void moveNodeToBottom(int instanceID)
				{
					auto widget = getNodeWidget(instanceID);
					for (int i = 0; i < m_nodeInstanceOrder.size(); i++)
						if (m_nodeInstanceOrder[i] == widget)
						{
							m_nodeInstanceOrder.swap(0, i);
							break;
						}
				}
			private:
				AbstractNodeWidget* createNodeWidget(int instanceID, NodeStructure::NodeType type)
				{
					switch (type)
					{
					case CraftEngine::gui::model_view::NodeStructure::eNodeDefault:
						return new NodeWidget_Default(this, instanceID);
						break;
					case CraftEngine::gui::model_view::NodeStructure::eNodeImage:
						return new NodeWidget_Image(this, instanceID);
						break;
					case CraftEngine::gui::model_view::NodeStructure::eNodeTitle:
						return new NodeWidget_Title(this, instanceID);
						break;
					default:
						return new NodeWidget_Default(this, instanceID);
						break;
					}
				}

				void _On_Instance_Updated(int instanceID)
				{
					m_nodeInstanceViewData[instanceID]->nodeWidget->onUpdateNode();
				}

				void _On_Model_Destroyed()
				{
					setModel(nullptr);
				}

				void _Create_Port_Widget_For_Node_Widget(AbstractNodeWidget* widget);

				void _On_Instance_Created(int typeKey, int instanceKey);
				void _On_Instance_Destroyed(int instanceKey);
				void _On_Link_Add(const NodePortIndex& from, const NodePortIndex& to);
				void _On_Link_Removed(const NodePortIndex& from, const NodePortIndex& to);
				void _On_Links_All_Removed();
				void _On_Instances_All_Removed();
				void _Clear_All_Node_Data();
				void _Reflash_All_Node_Data();
				core::ArrayList<math::vec2> _Gen_Link_Polygon(const NodePortIndex& from, const NodePortIndex& to, const NodePortStyle::LinkType& type);
				void _On_Node_Widget_Drag(AbstractNodeWidget* widget);

				void _Handle_Port_Widget_Drag(NodeIOPortWidget* widget, const Point& globalBase, const Point& offset);
				void _Handle_Port_Widget_Drag_Apply(NodeIOPortWidget* widget, const Point& globalBase, const Point& offset);

				virtual bool onChildFocusTransitive() override { return false; }
			public:

				/*
				 0: scroll widget space
				 1: srcoll bar
				 3: node widget
				*/
				virtual Point getChildOriginal(int groupid = GroupID::gDefaultWidget) const override final
				{
					switch (groupid)
					{
					case GroupID::gNodeWidget:
						return ScrollWidget::getChildOriginal(GroupID::gDefaultWidget);
					default:
						return ScrollWidget::getChildOriginal(groupid);
					}
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
					Point relative_point = mouseEvent.local - getPos();
					relative_point -= getChildOriginal(GroupID::gNodeWidget);
					subEvent.local = relative_point;

					for (int i = m_nodeInstanceOrder.size() - 1; i >= 0; i--)
					{
						hit = m_nodeInstanceOrder[i]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}

					return this;
				}

				void onPaintEvent() override final
				{
					auto painter = getPainter();
					drawBackground();

					// draw links
					PolygonInfo info{};
					//info.mode = info.ePolygonMode_LineStrip;
					using vec2 = math::vec2;
					for (auto it : m_linkData)
					{
						info.vertexCount = it.second->linkPolygon.size();
						info.posBuffer = it.second->linkPolygon.data();
						info.scale = vec2(1.0f);
						info.translate = vec2(0);
						info.rotation = 0;
						info.lineWidth = 2;
						painter.drawPolyline(it.second->linkPolygon.data(), it.second->linkPolygon.size(), false, it.second->linkColor);
					}

					// draw current node frame
					if (getCurrentInstanceID() >= 0)
					{
						auto widget = getNodeWidget(getCurrentInstanceID());
						Rect frame_rect = widget->getRect();
						frame_rect.mOffset -= Offset(1);
						frame_rect.mSize += Size(2);
						painter.drawRectFrame(frame_rect, getPalette().mForegroundColor);
					}

					// draw nodes
					Rect child_rect;
					Rect this_rect = Rect(Offset(0), getSize());
					for (auto it : m_nodeInstanceOrder)
					{
						Widget* w = it; // .second->nodeWidget;
						if (w->isHide() || w->isPopupable())
							continue;
						child_rect = w->getRect();
						child_rect.mOffset += getChildOriginal(w->getGroupid());
						if (Rect::haveIntersection(this_rect, child_rect))
							drawChild(w);
					}

					// draw current link
					if (m_clickedPort != nullptr)
					{
						info.vertexCount = 20;
						info.posBuffer = m_tempLinkVertexList;
						info.scale = vec2(1.0f);
						info.translate = vec2(0);
						info.rotation = 0;
						info.lineWidth = 3;
						painter.drawPolyline(m_tempLinkVertexList, 20, false, m_linkColor);
					}

					onPaint_drawScrollBar();
					drawFrame();
				}


				NodeIOPortWidget* m_clickedPort = nullptr;
				math::vec2 m_tempLinkVertexList[20];


				NodeIOPortWidget* _Get_Port_Widget(const Point& globalBase)
				{
					auto localPos = getLocalPos(globalBase, 0);
					for (auto it : m_nodeInstanceViewData)
					{
						auto w = it.second->nodeWidget->getPortWidget(localPos);
						if (w != nullptr)
							return (NodeIOPortWidget*)w;
					}
					return nullptr;
				}


				virtual void bindChildWidget(Widget* child) override final
				{
					// do nothing, only accept group 0
					Widget::bindChildWidget(child);
				}

				virtual bool onMouseEventCaught(const MouseEvent& mouseEvent, Widget* widget) override final
				{
					auto node_widget = (AbstractNodeWidget*)widget;
					if (widget->getGroupid() == GroupID::gNodeWidget && node_widget->getView() == this)
						return onHandleNodeMouseEvent(mouseEvent, node_widget);
					return true;
				}

				virtual bool onHandleNodeMouseEvent(const MouseEvent& mouseEvent, AbstractNodeWidget* widget) final
				{
					switch (mouseEvent.type)
					{
					case MouseEvent::eClicked:
						return onNodeClicked(widget, mouseEvent);
						break;
					case MouseEvent::eDoubleClicked:
						return onNodeDoubleClicked(widget, mouseEvent);
						break;
					case MouseEvent::ePressed:
						setCurrentInstanceID(widget->getInstanceID());
						return onNodePressed(widget, mouseEvent);
						break;
					case MouseEvent::eReleased:
						return onNodeReleased(widget, mouseEvent);
						break;
					case MouseEvent::eCallMenu:
						setCurrentInstanceID(widget->getInstanceID());
						return onNodeCallMenu(widget, mouseEvent);
						break;
					case MouseEvent::eWheel:
						return onNodeWheel(widget, mouseEvent);
						break;

					case MouseEvent::eMoveIn:
						return onNodeMoveIn(widget, mouseEvent);
						break;
					case MouseEvent::eMoveOut:
						return onNodeMoveOut(widget, mouseEvent);
						break;
					case MouseEvent::eFocusIn:
						return onNodeFocusIn(widget, mouseEvent);
						break;
					case MouseEvent::eFocusOut:
						return onNodeFocusOut(widget, mouseEvent);
						break;
					case MouseEvent::eDrag:
						return onNodeDrag(widget, mouseEvent);
						break;
					case MouseEvent::eDragApply:
						return onNodeDragApply(widget, mouseEvent);
						break;
					}
					return true;
				}
			protected:
				virtual bool onNodeDoubleClicked(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeClicked(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeCallMenu(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeWheel(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeMoveIn(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeMoveOut(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeFocusIn(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
				virtual bool onNodeFocusOut(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) { return true; }
			private:
				virtual bool onNodeDrag(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) final
				{
					widget->onDrag(mouseEvent);
					_On_Node_Widget_Drag(widget);
					return false;
				}
				virtual bool onNodeDragApply(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) final
				{
					return true;
				}
				virtual bool onNodePressed(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) final
				{
					widget->setSelected(true);
					return false;
				}
				virtual bool onNodeReleased(AbstractNodeWidget* widget, const MouseEvent& mouseEvent) final
				{
					if (getCurrentInstanceID() != widget->getInstanceID())
						widget->setSelected(false);
					return false;
				}
			protected:
				virtual void onPressed(const MouseEvent& mouseEvent) override
				{
					m_clickedPort = nullptr;
					setCurrentInstanceID(-1);
				}
			private:
				NodeModel* m_model = nullptr;
				int m_signalKey = -1;
			};




			void AbstractNodeWidget::_On_Create()
			{
				getView()->_Create_Port_Widget_For_Node_Widget(this);
			}

			void NodeView::_Create_Port_Widget_For_Node_Widget(AbstractNodeWidget* widget)
			{
				auto instance = getModel()->getInstance(widget->getInstanceID());
				auto& structure = instance.structure();
				for (int i = 0; i < structure.mPorts.inputs.size(); i++)
				{
					auto new_node_widget = new NodeIOPortWidget(i, true, this, widget);
					new_node_widget->setGroupid(GroupID::gPortWidget);
					new_node_widget->bindParentWidget(widget);
					widget->m_input.push_back(new_node_widget);
				}

				for (int i = 0; i < structure.mPorts.outputs.size(); i++)
				{
					auto new_node_widget = new NodeIOPortWidget(i, false, this, widget);
					new_node_widget->setGroupid(GroupID::gPortWidget);
					new_node_widget->bindParentWidget(widget);
					widget->m_output.push_back(new_node_widget);
				}
			}


			void NodeView::_On_Instance_Created(int typeKey, int instanceKey)
			{
				NodeInstanceViewData* data = new NodeInstanceViewData;
				auto instance = getModel()->getInstance(instanceKey);
				auto& structure = instance.structure();
				m_nodeInstanceViewData.emplace(std::make_pair(instance.instanceID(), data));
				//for (int i = 0; i < instance.beginLinkList.size(); i++)
				//{
				//	auto linkIdx = m_nextLinkKey++;
				//	auto linkData = new LinkData;
				//	linkData->beginPort = instance.beginLinkList[i].from;
				//	linkData->endPort = instance.beginLinkList[i].to;
				//	linkData->
				//	m_linkData.emplace(std::make_pair(linkIdx, linkData));
				//	data->linkKeyList.insert(linkIdx);
				//}
				data->nodeWidget = createNodeWidget(instance.instanceID(), structure.mType);
				data->nodeWidget->setGroupid(GroupID::gNodeWidget);
				data->nodeWidget->bindParentWidget(this);
				m_nodeInstanceOrder.push_back(data->nodeWidget);
			}
			void NodeView::_On_Instance_Destroyed(int instanceKey)
			{
				setCurrentInstanceID(-1);
				auto it = m_nodeInstanceViewData.find(instanceKey);
				if (it == m_nodeInstanceViewData.end())
					throw std::exception("error");
				for (auto i : it->second->linkKeyList)
				{
					auto linkIt = m_linkData.find(i);
					int other_instance_idx = linkIt->second->beginPort.mInstanceID == instanceKey ? linkIt->second->endPort.mInstanceID : linkIt->second->beginPort.mInstanceID;
					auto& other_instance = m_nodeInstanceViewData[other_instance_idx];
					other_instance->linkKeyList.erase(i);
					delete linkIt->second;
					m_linkData.erase(linkIt);
				}
				for (int i = 0; i < m_nodeInstanceOrder.size(); i++)
					if (m_nodeInstanceOrder[i] == it->second->nodeWidget)
					{
						m_nodeInstanceOrder.erase(i);
						break;
					}
				delete it->second->nodeWidget;
				delete it->second;
				m_nodeInstanceViewData.erase(it);
				this->m_curentLinkID = -1;
			}
			void NodeView::_On_Link_Add(const NodePortIndex& from, const NodePortIndex& to)
			{
				auto linkIdx = m_nextLinkKey++;
				auto linkData = new LinkData;
				linkData->beginPort = from;
				linkData->endPort = to;
				linkData->style = getModel()->getContext()->getPortStyle(getModel()->getInstance(from.mInstanceID).structure().getOutPortStyle(from.mPortID));
				m_linkData.emplace(std::make_pair(linkIdx, linkData));
				auto nodeData0 = m_nodeInstanceViewData[from.mInstanceID];
				auto nodeData1 = m_nodeInstanceViewData[to.mInstanceID];
				nodeData0->linkKeyList.emplace(linkIdx);
				nodeData1->linkKeyList.emplace(linkIdx);
				auto&& vertexData = _Gen_Link_Polygon(from, to, linkData->style.mLink);
				linkData->linkPolygon.swap(vertexData);
				linkData->linkColor = m_linkColor; // 
			}
			void NodeView::_On_Link_Removed(const NodePortIndex& from, const NodePortIndex& to)
			{
				auto fromNodeData = m_nodeInstanceViewData[from.mInstanceID];
				auto toNodeData = m_nodeInstanceViewData[to.mInstanceID];
				int linkId = -1;
				for (auto it : fromNodeData->linkKeyList)
				{
					auto linkDataIt = m_linkData.find(it);
					if (linkDataIt == m_linkData.end())
						throw std::exception("error");
					if (linkDataIt->second->beginPort.mPortID == from.mPortID && linkDataIt->second->endPort.mPortID == to.mPortID)
					{
						linkId = linkDataIt->first;
						delete linkDataIt->second;
						m_linkData.erase(linkDataIt);
						break;
					}
				}
				fromNodeData->linkKeyList.erase(linkId);
				toNodeData->linkKeyList.erase(linkId);
			}
			void NodeView::_On_Links_All_Removed()
			{
				for (auto it : m_nodeInstanceViewData)
					it.second->linkKeyList.clear();
				for (auto it : m_linkData)
					delete it.second;
				m_linkData.clear();
			}
			void NodeView::_On_Instances_All_Removed()
			{
				setCurrentInstanceID(-1);
				for (auto it : m_nodeInstanceViewData)
				{
					delete it.second->nodeWidget;
					delete it.second;
				}
				m_nodeInstanceViewData.clear();
				for (auto it : m_linkData)
					delete it.second;
				m_linkData.clear();
			}
			void NodeView::_Clear_All_Node_Data()
			{
				setCurrentInstanceID(-1);
				for (auto it : m_nodeInstanceViewData)
				{
					delete it.second->nodeWidget;
					delete it.second;
				}
				m_nodeInstanceViewData.clear();
				m_nodeInstanceOrder.clear();
				for (auto it : m_linkData)
					delete it.second;
				m_linkData.clear();
			}
			void NodeView::_Reflash_All_Node_Data()
			{
				NodeView::_Clear_All_Node_Data();
				auto&& instanceList = getModel()->getInstanceKeyList();
				for (int i = 0; i < instanceList.size(); i++)
				{
					NodeInstanceViewData* data = new NodeInstanceViewData;
					auto instance = getModel()->getInstance(instanceList[i]);
					auto& nodeStructure = instance.structure();
					m_nodeInstanceViewData.emplace(std::make_pair(instance.instanceID(), data));
					for (int i = 0; i < instance.beginLinkCount(); i++)
					{
						auto linkIdx = m_nextLinkKey++;
						auto linkData = new LinkData;
						linkData->beginPort = instance.beginLink(i).beginIndex();
						linkData->endPort = instance.beginLink(i).endIndex();
						auto portStyleIndex = nodeStructure.getOutPortStyle(linkData->beginPort.mPortID);
						if (portStyleIndex >= 0)
						{
							auto& portStyle = getModel()->getContext()->getPortStyle(portStyleIndex);
							linkData->style = portStyle;
						}
						else
						{

						}
						linkData->linkColor = linkData->style.mColor;
						m_linkData.emplace(std::make_pair(linkIdx, linkData));
					}
					data->nodeWidget = createNodeWidget(instance.instanceID(), nodeStructure.mType); //
					data->nodeWidget->setGroupid(GroupID::gNodeWidget); //
					data->nodeWidget->bindParentWidget(this); //
					m_nodeInstanceOrder.push_back(data->nodeWidget);
				}
				for (auto& it : m_linkData)
				{
					auto nodeData0 = m_nodeInstanceViewData[it.second->beginPort.mInstanceID];
					auto nodeData1 = m_nodeInstanceViewData[it.second->endPort.mInstanceID];
					nodeData0->linkKeyList.emplace(it.first);
					nodeData1->linkKeyList.emplace(it.first);
					auto&& vertexData = _Gen_Link_Polygon(it.second->beginPort, it.second->endPort, it.second->style.mLink);
					it.second->linkPolygon.swap(vertexData);
				}
				for (auto it : m_nodeInstanceViewData)
				{
					_On_Instance_Updated(it.first);
				}

				this->m_curentLinkID = -1;
			}
			core::ArrayList<math::vec2> NodeView::_Gen_Link_Polygon(const NodePortIndex& from, const NodePortIndex& to, const NodePortStyle::LinkType& type)
			{
				core::ArrayList<math::vec2> vertexList;
				auto fromNodeData = m_nodeInstanceViewData[from.mInstanceID];
				auto fromNodeWidget = fromNodeData->nodeWidget;
				auto fromNodePos = fromNodeWidget->getPos() + fromNodeWidget->getChildOriginal(GroupID::gPortWidget) + fromNodeWidget->getOutputPortCenter(from.mPortID);
				auto toNodeData = m_nodeInstanceViewData[to.mInstanceID];
				auto toNodeWidget = toNodeData->nodeWidget;
				auto toNodePos = toNodeWidget->getPos() + toNodeWidget->getChildOriginal(GroupID::gPortWidget) + toNodeWidget->getInputPortCenter(to.mPortID);
				float distance = (toNodePos.x - fromNodePos.x) / 2.0f; // math::max((toNodePos.x - fromNodePos.x) / 2.0f, 20.0f);
				using vec2 = math::vec2;
				vec2 controlPoint[2] = { vec2(fromNodePos) + vec2(distance, 0), vec2(toNodePos) + vec2(-distance, 0) };
				vec2 controlPoints[4] = { vec2(fromNodePos), vec2(controlPoint[0]), vec2(controlPoint[1]), vec2(toNodePos) };

				switch (type)
				{
				case NodePortStyle::LinkType::eLinkBspline:
				{
					math::BSplineCurve<4> curve(controlPoints);
					vertexList.resize(20);
					for (int i = 0; i < 20; i++)
						vertexList[i] = curve.pointAt(i / (19.0f));
					break;
				}
				case NodePortStyle::LinkType::eLinkBezier:
				{
					math::BezierCurve<4> curve(controlPoints);
					vertexList.resize(20);
					for (int i = 0; i < 20; i++)
						vertexList[i] = curve.pointAt(i / (19.0f));
					break;
				}
				case NodePortStyle::LinkType::eLinkLine:
				{
					vertexList.resize(4);
					vertexList[0] = vec2(fromNodePos);
					vertexList[1] = vec2(controlPoint[0]);
					vertexList[2] = vec2(controlPoint[1]);
					vertexList[3] = vec2(toNodePos);
					break;
				}
				case NodePortStyle::LinkType::eLinkStraight:
				{
					vertexList.resize(2);
					vertexList[0] = vec2(fromNodePos);
					vertexList[3] = vec2(toNodePos);
					break;
				}
				case NodePortStyle::LinkType::eLinkArrow:
				case NodePortStyle::LinkType::eLinkDefault:
				default:
				{
					math::BSplineCurve<4> curve(controlPoints);
					vertexList.resize(20);
					for (int i = 0; i < 20; i++)
						vertexList[i] = curve.pointAt(i / (19.0f));
					break;
				}
				}
				return vertexList;
			}
			void NodeView::_On_Node_Widget_Drag(AbstractNodeWidget* widget)
			{
				auto idx = widget->getInstanceID();
				auto& instanceData = m_nodeInstanceViewData[idx];
				for (auto it : instanceData->linkKeyList)
				{
					auto& linkData = m_linkData[it];
					auto&& vertexData = _Gen_Link_Polygon(linkData->beginPort, linkData->endPort, linkData->style.mLink);
					linkData->linkPolygon.swap(vertexData);
				}
			}


			void NodeView::_Handle_Port_Widget_Drag(NodeIOPortWidget* widget, const Point& globalBase, const Point& offset)
			{
				m_clickedPort = widget;
				using vec2 = math::vec2;
				vec2 fromPos;
				vec2 toPos;
				if (m_clickedPort->isInputPort())
				{
					toPos = vec2(m_clickedPort->getNodeWidget()->getPos() + m_clickedPort->getNodeWidget()->getChildOriginal(GroupID::gPortWidget) + m_clickedPort->getNodeWidget()->getInputPortCenter(m_clickedPort->getPort()));
					fromPos = vec2(getLocalPos(globalBase + offset));
				}
				else
				{
					fromPos = vec2(m_clickedPort->getNodeWidget()->getPos() + m_clickedPort->getNodeWidget()->getChildOriginal(GroupID::gPortWidget) + m_clickedPort->getNodeWidget()->getOutputPortCenter(m_clickedPort->getPort()));
					toPos = vec2(getLocalPos(globalBase + offset));
				}
				float distance = (toPos.x - fromPos.x) / 2.0f; // math::max((toPos.x - fromPos.x) / 2.0f, 20.0f);
				using vec2 = math::vec2;
				vec2 controlPoint[2] = { vec2(fromPos) + vec2(distance, 0), vec2(toPos) + vec2(-distance, 0) };
				vec2 controlPoints[4] = { vec2(fromPos), vec2(controlPoint[0]), vec2(controlPoint[1]), vec2(toPos) };
				math::BSplineCurve<4> curve(controlPoints);
				for (int i = 0; i < 20; i++)
					m_tempLinkVertexList[i] = curve.pointAt(i / (19.0f));
			}

			void NodeView::_Handle_Port_Widget_Drag_Apply(NodeIOPortWidget* widget, const Point& globalBase, const Point& offset)
			{
				auto nextPort = _Get_Port_Widget(globalBase + offset);

				if (m_clickedPort == nullptr || nextPort == nullptr || nextPort->isInputPort() == m_clickedPort->isInputPort())
				{

				}
				else if (m_clickedPort->getNodeWidget()->getInstanceID() == nextPort->getNodeWidget()->getInstanceID())
				{

				}
				else if (m_clickedPort->isInputPort())
				{
					int fromNodeIdx = nextPort->getNodeWidget()->getInstanceID();
					int toNodeIdx = m_clickedPort->getNodeWidget()->getInstanceID();
					int fromPortIdx = nextPort->getPort();
					int toPortIdx = m_clickedPort->getPort();
					if (getModel()->isLinkExist({ fromNodeIdx, fromPortIdx }, { toNodeIdx, toPortIdx }))
						getModel()->removeLink({ fromNodeIdx, fromPortIdx }, { toNodeIdx, toPortIdx });
					else
						getModel()->addLink({ fromNodeIdx, fromPortIdx }, { toNodeIdx, toPortIdx });
				}
				else
				{
					int fromNodeIdx = m_clickedPort->getNodeWidget()->getInstanceID();
					int toNodeIdx = nextPort->getNodeWidget()->getInstanceID();
					int fromPortIdx = m_clickedPort->getPort();
					int toPortIdx = nextPort->getPort();
					if (getModel()->isLinkExist({ fromNodeIdx, fromPortIdx }, { toNodeIdx, toPortIdx }))
						getModel()->removeLink({ fromNodeIdx, fromPortIdx }, { toNodeIdx, toPortIdx });
					else
						getModel()->addLink({ fromNodeIdx, fromPortIdx }, { toNodeIdx, toPortIdx });
				}
				m_clickedPort = nullptr;
			}




		}
	}
}