#pragma once
#ifndef CRAFT_ENGINE_GUI_WIDGET_H_
#define CRAFT_ENGINE_GUI_WIDGET_H_
#include "../Common.h"

namespace CraftEngine
{
	namespace gui
	{

		
		class GroupID
		{
		public:
			enum WidgetGroupIDEnum { 
				GroupIDmask = 0xffff
			};

			static constexpr WidgetGroupIDEnum gDefaultWidget = static_cast<WidgetGroupIDEnum>(0x0000);
			static constexpr WidgetGroupIDEnum gIndependentWidget = static_cast<WidgetGroupIDEnum>(0x0001);
			static constexpr WidgetGroupIDEnum gScrollBar     = static_cast<WidgetGroupIDEnum>(0x0002);
			static constexpr WidgetGroupIDEnum gFrameButton   = static_cast<WidgetGroupIDEnum>(0x0003);
			static constexpr WidgetGroupIDEnum gItemWidget    = static_cast<WidgetGroupIDEnum>(0x0004);
			static constexpr WidgetGroupIDEnum gNodeWidget    = static_cast<WidgetGroupIDEnum>(0x0005);
			static constexpr WidgetGroupIDEnum gPortWidget    = static_cast<WidgetGroupIDEnum>(0x0006);
		};


		int widget_instances_count = 0;

		class RootWidget;

		/*
		 Widget
		*/
		class Widget : CraftEngine::core::NonCopyable
		{
		public:
			struct PaintEvent
			{
				AbstractGuiRenderer* pRenderer = nullptr;
				Point globalRelativePoint;
				Rect  globalLimitRect;
				int   globalAlpha;
			};

			struct ResizeEvent
			{
				enum ResizePhase
				{
					eCalcSize,
					eCalcPos,
				};
				Size        size;
				ResizePhase phase;
			};

			struct MouseEvent
			{
				enum : int
				{
					eNoButton = -1,
					eLeftButton = 0,
					eRightButton = 1,
					eMiddleButton = 2,
				};

				enum : int
				{
					eDetect = 0x1,
					eClicked = 0x2,
					ePressed = 0x4,
					eReleased = 0x8,
					eMoveIn = 0x10,
					eMoveOut = 0x20,
					eFocusIn = 0x40,
					eFocusOut = 0x80,
					eCallMenu = 0x100,
					eDrag = 0x200,
					eDragApply = 0x400,
					eWheel = 0x800,
					eMoving = 0x1000,
					eDoubleClicked = 0x2000,
					eDrop = 0x4000, // drop a widget
					eDetect2 = 0x8000, // detect second widget
					eAll = 0xFFFF,
				};
				Point   global;
				Point   local;
				Point   offset;
				int     type;
				int     button;
				int     duration;
				Widget* widget;
			};

			struct KeyboardEvent
			{
				KeyBoard::KeyVal key;
				bool down;
			};

		private:
			friend class RootWidget;
			friend class CanvasWidget;
			Rect    m_rect;
			Size    m_minSizeLimit = Size(0);
			Size    m_maxSizeLimit = Size(65535);
			Palette m_palette;
			Font    m_font;
			int     m_alpha = 255;
			AbstractCursor* m_cursor = nullptr;
			AbstractCursor::SystemCursorType m_cursorType = AbstractCursor::SystemCursorType::eArrowCursor;

			Offset m_dragOffset = Offset(0, 0);

			// layout
			AbstractCustomLayout* m_customLayout = nullptr;
			Layout m_layout;

			// attributes
			bool m_isHide = false;
			bool m_isTransparent = false;

			bool m_isClickable = false;
			bool m_isDragable = false;
			bool m_isLeftButtonDrag = true;
			bool m_isMouseWheelable = false;

			bool m_isInputFocus = false;
			bool m_isFocus = false;
			bool m_isHighlight = false;

			bool m_isCharAcceptable = false;
			bool m_isFileAcceptable = false;

			// private attribute
			bool m_isNowPopup = false;
			bool m_isPopupable = false;
			bool m_isRootWidget = false;

			// private
			bool m_isDropable = false;
			bool m_isAcceptDrop = false;
			int  m_maxDropDepth = 0;

			int m_group = GroupID::gDefaultWidget;
			Widget* m_parent;
			RootWidget* m_rootWidget = nullptr;
			core::ArrayList<Widget*> m_children;
			int m_mouseEventCatchFlags = 0;

		private:
			// render info.
			AbstractGuiRenderer* m_tempRenderer;
			Point        m_tempGlobalRelativePoint;
			Rect         m_tempGlobalLimitRect;
			Rect         m_tempChildGlobalLimitRect;
			int          m_tempAlpha;
		public:
			Widget* getParentWidget() const { return m_parent; }
			auto&   getChildrenWidgets() { return m_children; }
			RootWidget* getRootWidget()
			{
				Widget* root = getParentWidget();
				if (root == nullptr)
					if (m_isRootWidget) return (RootWidget*)this;
					else return nullptr;
				else
					return root->getRootWidget();
			}
			bool haveParentWidget(Widget* object)
			{
				Widget* w = this;
				while (w != nullptr)
				{
					w = w->getParentWidget();
					if (w == object)
						return true;
				}return false;
			}
		public:

			/*
			 Add a child widget to children list.
			 *Called when constructs a widget.
			*/
			virtual void bindChildWidget(Widget* child)
			{
				if (child->getGroupid() == GroupID::gDefaultWidget)
					m_children.push_back(child);
				else
					craft_engine_error("GUI SYSTEM", -1, -1, -1, "child widget's group id will not be accepted!");
			}
			/*
			  Remove a child widget from children list.
			  *Called when disstructs a widget.
			*/
			virtual void unbindChildWidget(Widget* widget)
			{
				if (widget->getGroupid() == GroupID::gDefaultWidget)
				{
					for (int i = 0; i < m_children.size(); i++)
						if (m_children[i] == widget)
						{
							m_children.erase(i);
							//widget->m_parent = nullptr;
							return;
						}
					throw std::runtime_error("param widget is not a child widget of its parent widget!");
				}
			}
			void bindParentWidget(Widget* parent)
			{
				if (m_parent == nullptr)
					if (parent != nullptr)
						m_parent = parent;
					else
						throw std::runtime_error("param parent is nullptr!");
				else
					throw std::runtime_error("current parent is not null!");
			}
			void unbindParentWidget()
			{
				if (m_parent != nullptr)
					m_parent = nullptr;
				else
					throw std::runtime_error("current parent is null!");
			}

		private:
			void setNowPopup(bool enable) { m_isNowPopup = enable; }
		public:

			void setPopupable(bool enable) { m_isPopupable = enable; }
			bool isPopupable()const { return m_isPopupable; }
			bool isNowPopup()const { return m_isNowPopup; }

			virtual bool shouldPopupCancel() 
			{
				if (isInputFocus())
					return false;
				for (auto c : m_children)
					if (!c->shouldPopupCancel())
						return false;
				return true; 
			}

		protected:
			CRAFT_ENGINE_EXPLICIT Widget() :m_parent(nullptr), m_rect(Rect(0, 0, 200, 50))
			{
				widget_instances_count++;
				m_isRootWidget = true;
				sizeof(Widget);
			}

		private:
			virtual void _On_Destroy();
			virtual void _Destroy_Children_Widget();
		public:
			virtual ~Widget()
			{
				widget_instances_count--;
				if (isNowPopup() || isFocus() || isHighlight() || isInputFocus())
				{
					_On_Destroy();
				}
		
				_Destroy_Children_Widget();

				// if size of children list of parent is zero, than widget do not remove itself from parent widget
				if (getParentWidget() != nullptr && getParentWidget()->getChildrenWidgets().size() > 0)
					getParentWidget()->unbindChildWidget(this);
			}

			Widget(const Rect & rect, Widget *p) :m_rect(rect), m_parent(nullptr)
			{
				widget_instances_count++;
				if (p != nullptr)
				{
					p->bindChildWidget(this);
					bindParentWidget(p);
				}
				// throw std::runtime_error("param p could not be nullptr!");
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eWidget));
			}

			Widget(Widget *p) :Widget(Rect(0, 0, 200, 50), p)
			{

			}

			/*
			 Convert a point from global space to THIS local space.
			 * The result space is the same as getRect().
			 * The result must be wrong if one of parent widget set to nullptr.
			*/
			virtual Point getBaseLocalPos(const Point& globalPos)const final
			{
				if (getParentWidget() == nullptr)
					return globalPos - getPos();
				else
					return getParentWidget()->getBaseLocalPos(globalPos - getParentWidget()->getChildOriginal(getGroupid())) - getPos();
			}

			/*
			 Convert a point from THIS local space to global space.
			 * The result space is the same as screen space.
			 * The result must be wrong if one of parent widget set to nullptr.
			*/
			virtual Point getBaseGlobalPos(const Point& localPos)const final
			{
				if (getParentWidget() == nullptr)
					return localPos + getPos();
				else
					return getParentWidget()->getBaseGlobalPos(localPos + getParentWidget()->getChildOriginal(getGroupid())) + getPos();
				//return getParentWidget()->getGlobalPos(localPos + getChildOriginal(groupid) + getPos(), getGroupid()) ;
			}


			/*
			 Convert a point from global space to CHILD'S local space.
			 * The result space is the same as child's getRect().
			 * The result must be wrong if one of parent widget set to nullptr.
			 * getLocalPos(globalPos, groupid) is equal to getBaseLocalPos(globalPos) - getChildOriginal(groupid)
			*/
			virtual Point getLocalPos(const Point& globalPos, int groupid = 0)const final
			{				
				// return getBaseLocalPos(globalPos) - getChildOriginal(groupid);
				if (getParentWidget() == nullptr)
					return globalPos - getChildOriginal(groupid) - getPos();
				else
					return getParentWidget()->getLocalPos(globalPos, getGroupid()) - getChildOriginal(groupid) - getPos();
			}

			/*
			 Convert a point from child's local space to global space.
			 * The result space is the same as screen space.
			 * The result must be wrong if one of parent widget set to nullptr.
			 * getGlobalPos(globalPos, groupid) is equal to getBaseGlobalPos(globalPos) + getChildOriginal(groupid)
			*/
			virtual Point getGlobalPos(const Point& localPos, int groupid = 0)const final
			{
				// return getBaseGlobalPos(localPos) + getChildOriginal(groupid);
				if (getParentWidget() == nullptr)
					return localPos + getPos();
				else
					return getParentWidget()->getGlobalPos(localPos + getChildOriginal(groupid) + getPos(), getGroupid());
			}

			/*
			 Convert a point from child's local space to global space.
			 * The result space is the same as screen space.
			 * The result must be wrong if one of parent widget set to nullptr.
			*/
			virtual Point getChildOriginal(int groupid)const
			{
				return Point(0);
			}


			bool isHide()const { return m_isHide; }
			bool isTransparent()const { return m_isTransparent; }
			bool isClickable()const { return m_isClickable; }
			bool isDragable()const { return m_isDragable; }
			bool isLeftButtonDrag()const { return m_isLeftButtonDrag; }
			bool isMouseWheelable()const { return m_isMouseWheelable; }
			bool isCharAcceptable()const { return m_isCharAcceptable; }
			bool isFileAcceptable()const { return m_isFileAcceptable; }
			bool isInputFocus()const { return m_isInputFocus; }
			bool isFocus()const { return m_isFocus; }
			bool isHighlight()const { return m_isHighlight; }

			void setHide(bool enable) { m_isHide = enable; }
			void setTransparent(bool enable) { m_isTransparent = enable; }
			void setClickable(bool enable) { m_isClickable = enable; }
			void setDragable(bool enable) { m_isDragable = enable; }
			void setLeftButtonDrag(bool enable) { m_isLeftButtonDrag = enable; }
			void setMouseWheelable(bool enable) { m_isMouseWheelable = enable; }
			void setCharAcceptable(bool enable) { m_isCharAcceptable = enable; }
			void setFileAcceptable(bool enable) { m_isFileAcceptable = enable; }
		private:
			void setInputFocus(bool enable) { m_isInputFocus = enable; }
			void setFocus(bool enable) { m_isFocus = enable; }
			void setHighlight(bool enable) { m_isHighlight = enable; }
		public:
			void forceInputFocus(bool enable);
		public:
			void setGroupid(int id) { m_group = id; }
			int  getGroupid()const { return m_group; }
			int  getMouseEventCatchFlags() { return m_mouseEventCatchFlags; }
			void setMouseEventCatchFlags(int flags) { m_mouseEventCatchFlags = flags; }
			void enableMouseEventCatchFlags(int flagbits) { m_mouseEventCatchFlags |= flagbits; }
			void disableMouseEventCatchFlags(int flagbits) { m_mouseEventCatchFlags &= (m_mouseEventCatchFlags ^ (m_mouseEventCatchFlags & flagbits)); }

			void    setSize(const Size& size) { m_rect.mSize = size; onResizeEvent({ getSize() }); }
			Size    getSize()const { return m_rect.mSize; }
			void    setPos(const Point& point) { m_rect.mOffset = point; }
			Point   getPos()const { return m_rect.mOffset; }
			void    setRect(const Rect& rect) { m_rect = rect; onResizeEvent({ getSize() }); }
			Rect    getRect()const { return m_rect; }
			int32_t getX()const { return m_rect.mX; }
			int32_t getY()const { return m_rect.mY; }
			int32_t getWidth()const { return m_rect.mWidth; }
			int32_t getHeight()const { return m_rect.mHeight; }

			virtual void move(const Offset& offset) { m_rect.mOffset += offset; }
		protected:
			const Offset& getDragOffset()const { return m_dragOffset; }
			void          setDragOffset(const Offset& offset) { m_dragOffset = offset; }
		public:
			const Font& getFont()const { return m_font; }
			Font&       getFont() { return m_font; }
			void        setFont(const Font& font) { m_font = font; }
		public:
			const Palette& getPalette()const { return m_palette; }
			Palette&       getPalette() { return m_palette; }
			void           setPalette(const Palette& brash) { m_palette = brash; }
		public:
			AbstractCursor* getCursor() { return m_cursor; }
			void            setCursor(AbstractCursor* cursor) { m_cursor = cursor; }
			AbstractCursor::SystemCursorType getCursorType() { return m_cursorType; }
			void                             setCursorType(AbstractCursor::SystemCursorType cursorType) { m_cursorType = cursorType; }
		public:
			void         setMinSizeLimit(const Size& size) { m_minSizeLimit = size; }
			void         setMaxSizeLimit(const Size& size) { m_maxSizeLimit = size; }
			virtual Size getMinSizeLimit()const { return m_minSizeLimit; }
			virtual Size getMaxSizeLimit()const { return m_maxSizeLimit; }
		public:
			const Layout& getLayout()const { return m_layout; }
			Layout&       getLayout() { return m_layout; }
			void          setLayout(const Layout& layout) { m_layout = layout; }
			void          setLayoutMode(Layout::LayoutMode mode) { m_layout.setMode(mode); }
			void          applyLaoyut()
			{ 
				core::ArrayList<Size> sizeList;
				for (auto it : m_children)
					sizeList.push_back(it->getSize());
				auto layout = m_layout.calcLayout(sizeList, getSize());
				if (layout.size() == m_children.size())
					for (int i = 0; i < m_children.size(); i++)
						m_children[i]->setRect(layout[i]);
			}		

		protected:

			virtual void onResizeEvent(const ResizeEvent & resizeEvent)
			{
				applyLaoyut();
			}

		public:

			virtual Widget* onMouseEvent(const MouseEvent& mouseEvent)
			{
				auto parent = this->getParentWidget();
				Widget* caught_widget = nullptr;
				bool should_continue = true;
				while (parent != nullptr && parent->m_mouseEventCatchFlags & mouseEvent.type)
				{	
					auto temp = parent->onMouseEventCaught(mouseEvent, this);
					if (!temp){
						caught_widget = parent;
						should_continue = false;
					}
					parent = parent->getParentWidget();
				}
				if (caught_widget != nullptr)
					return caught_widget;
				
				switch (mouseEvent.type)
				{	
				case MouseEvent::eDetect:
					return onDetect(mouseEvent);
				case MouseEvent::eDetect2:
					return mouseEvent.widget != this ? onDetect(mouseEvent) : nullptr;
				case MouseEvent::eMoveOut:
					onMoveOut(mouseEvent); return nullptr;
				case MouseEvent::eFocusOut:
					onFocusOut(mouseEvent); return nullptr;
				case MouseEvent::eFocusIn:
					onFocusIn(mouseEvent); return nullptr;
				case MouseEvent::eReleased:
					onReleased(mouseEvent); return nullptr;
				case MouseEvent::eDrag:
					onDrag(mouseEvent); return nullptr;
				case MouseEvent::eDragApply:
					onDragApply(mouseEvent); return nullptr;
				case MouseEvent::eClicked:
					onClicked(mouseEvent); return nullptr;
				case MouseEvent::eDoubleClicked:
					onDoubleClicked(mouseEvent); return nullptr;
				case MouseEvent::eWheel:
					onWheel(mouseEvent); return nullptr;
				case MouseEvent::eMoving:
					onMoving(mouseEvent); return nullptr;
				case MouseEvent::eCallMenu:
					onCallMenu(mouseEvent); return nullptr;
				case MouseEvent::eMoveIn:
					onMoveIn(mouseEvent); return nullptr;
				case MouseEvent::ePressed:
					onPressed(mouseEvent); return nullptr;
				}
				return nullptr;
			}

		protected:

			virtual bool onMouseEventCaught(const MouseEvent& mouseEvent, Widget* widget)
			{
				return true;
			}

			/*
			 Detect a widget or forward message to child widgets
			*/
			virtual Widget* onDetect(const MouseEvent& mouseEvent) 
			{
				if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
					return nullptr;

				Widget* hit;
				MouseEvent subEvent = mouseEvent;
				for (int i = m_children.size() - 1; i >= 0; i--)
				{
					subEvent.local = mouseEvent.local - getPos() - getChildOriginal(m_children[i]->getGroupid());
					hit = m_children[i]->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}
				return this;
			}

			// These messages can be caught by rewrite [onMouseEvent] or return nullptr to forward message back to parent widget.
			virtual void onPressed(const MouseEvent& mouseEvent) { }
			virtual void onMoveIn(const MouseEvent& mouseEvent) { }
			virtual void onMoveOut(const MouseEvent& mouseEvent) { }
			virtual void onMoving(const MouseEvent& mouseEvent) { }
			virtual void onClicked(const MouseEvent& mouseEvent) { }
			virtual void onDoubleClicked(const MouseEvent& mouseEvent) { }
			virtual void onReleased(const MouseEvent& mouseEvent) { }
			virtual void onFocusIn(const MouseEvent& mouseEvent) { }
			virtual void onFocusOut(const MouseEvent& mouseEvent) { }

			// These messages can't be caught, but return nullptr to forward message back to parent widget.
			virtual void onCallMenu(const MouseEvent& mouseEvent)
			{
				if (getParentWidget() != nullptr)
					getParentWidget()->onCallMenu(mouseEvent);
			}

			virtual void onWheel(const MouseEvent& mouseEvent) 
			{ 
				if (getParentWidget() != nullptr) 
					getParentWidget()->onWheel(mouseEvent); 
			}

			virtual void onDrag(const MouseEvent& mouseEvent)
			{
				if (m_isDragable)
				{
					move(mouseEvent.offset - m_dragOffset);
					setDragOffset(mouseEvent.offset);
				}
				else
				{
					if (getParentWidget() != nullptr)
						getParentWidget()->onDrag(mouseEvent);
				}
			}

			virtual void onDragApply(const MouseEvent& mouseEvent)
			{
				if (m_isDragable)
				{
					setDragOffset(Offset(0, 0));
				}
				else
				{
					if (getParentWidget() != nullptr)
						getParentWidget()->onDragApply(mouseEvent);
				}
			}


			virtual void onChildFocusIn(Widget* child) { }
			virtual void onChildFocusOut(Widget* child) { }
			virtual bool onChildFocusTransitive() { return true; }
			virtual void onAcceptChar(Char c) {}
			virtual void onKeyBoardEvent(const KeyboardEvent& keyboardEvent)
			{
				if (m_parent != nullptr)
					m_parent->onKeyBoardEvent(keyboardEvent);
			}

			virtual void onAcceptFiles(const StringList& filelist)
			{
				if (m_parent != nullptr && !m_isFileAcceptable)
					m_parent->onAcceptFiles(filelist);
			}

			virtual String onTextCopy() { return String(); }
			virtual void   onTextPause(const String& text) { }

			// render function
		public:
			void setAlpha(uint8_t alpha) { m_alpha = alpha; }
			int getAlpha() const { return m_alpha; }
		protected:
			Painter getPainter(int groupid = GroupID::gDefaultWidget)
			{
				return Painter(m_tempRenderer, m_tempGlobalRelativePoint + getChildOriginal(groupid), m_tempChildGlobalLimitRect, getSize(), m_tempAlpha);
			}
			Painter getThisPainter()
			{
				return Painter(m_tempRenderer, m_tempGlobalRelativePoint, m_tempGlobalLimitRect, getSize(), m_tempAlpha);
			}
			Painter getGlobalPainter()
			{
				return Painter(m_tempRenderer, Point(), Rect(-65535, 65535, 65535 * 2, 65535 * 2), getSize(), m_tempAlpha);
			}
		protected:
			void sendRepaintEvent();

			virtual void drawBackground()
			{
				auto painter = getThisPainter();
				auto rect = Rect(Point(0, 0), getSize());
				switch (getPalette().mFillMode)
				{
				case Palette::FillMode::eFillMode_None:
					break;
				case Palette::FillMode::eFillMode_Color:
					painter.drawRect(rect, getPalette().mBackgroundColor);
					break;
				case Palette::FillMode::eFillMode_Image:
					painter.drawRect(rect, getPalette().mBackgroundImage);
					break;
				default:
					break;
				}
			}

			virtual void drawFrame()
			{
				auto painter = getThisPainter();
				auto rect = Rect(Point(0, 0), getSize());
				switch (getPalette().mFrameMode)
				{
				case Palette::FrameMode::eFrameMode_None:
					break;
				case Palette::FrameMode::eFrameMode_Surround:
					painter.drawRectFrame(rect, getPalette().mFrameColor);
					break;
				//case Palette::FrameMode::eFrameMode_Inside:
				//{
				//	rect.mOffset += Size(1);
				//	rect.mSize -= Size(1);
				//	painter.drawRectFrame(rect, getPalette().mFrameColor);
				//	break;
				//}
				//case Palette::FrameMode::eFrameMode_Outside:
				//{
				//	rect.mSize += Size(1);
				//	painter.drawRectFrame(rect, getPalette().mFrameColor);
				//	break;
				//}
				default:
					break;
				}
			}
		private:
			void processPaintEvent(const PaintEvent& paintEvent)
			{
				auto this_global_rect = Rect(paintEvent.globalRelativePoint + getPos(), getSize());
				Rect child_global_limit_rect = paintEvent.globalLimitRect.disjunction(this_global_rect);
				m_tempRenderer = paintEvent.pRenderer;
				m_tempGlobalRelativePoint = this_global_rect.mOffset;
				m_tempGlobalLimitRect = paintEvent.globalLimitRect;
				m_tempChildGlobalLimitRect = child_global_limit_rect;
				m_tempAlpha = paintEvent.globalAlpha * getAlpha() / 255;
			}
		protected:
			virtual void drawAllChildren()
			{
				PaintEvent subEvent;
				subEvent.pRenderer = m_tempRenderer;
				subEvent.globalRelativePoint = m_tempGlobalRelativePoint + getChildOriginal(GroupID::gDefaultWidget);
				subEvent.globalLimitRect = m_tempChildGlobalLimitRect;
				subEvent.globalAlpha = m_tempAlpha;
				for (auto child : m_children)
				{
					if (child->isHide() || child->isPopupable())
						continue;
					child->processPaintEvent(subEvent);
					child->onPaintEvent();
				}
			}
			virtual void drawChild(Widget* widget)
			{
				PaintEvent subEvent;
				subEvent.pRenderer = m_tempRenderer;
				subEvent.globalRelativePoint = m_tempGlobalRelativePoint + getChildOriginal(widget->getGroupid());
				subEvent.globalLimitRect = m_tempChildGlobalLimitRect;
				subEvent.globalAlpha = m_tempAlpha;
				widget->processPaintEvent(subEvent);
				widget->onPaintEvent();
			}
			virtual void drawChild2(Widget* widget)
			{
				PaintEvent subEvent;
				subEvent.pRenderer = m_tempRenderer;
				subEvent.globalRelativePoint = m_tempGlobalRelativePoint + getChildOriginal(widget->getGroupid());
				subEvent.globalLimitRect = m_tempGlobalLimitRect;
				subEvent.globalAlpha = m_tempAlpha;
				widget->processPaintEvent(subEvent);
				widget->onPaintEvent();
			}
		public:
			virtual void onPaintEvent()
			{
				drawBackground();
				drawAllChildren();
				drawFrame();
			}

		public:

			template<typename...Args>
			core::Any execute(const std::string& command, const Args&... args)
			{
				std::enable_if<((sizeof...(Args)) > 0), void>::type;
				std::vector<core::Any> package;
				_Pack_Params(package, args...);
				return execute(command, package);
			}

			virtual core::Any execute(const std::string& command, const std::vector<core::Any>& params)
			{
				std::stringstream strbuf(command);
				std::string cmd;
				strbuf >> cmd;
				for (auto& it : cmd)
					it = tolower(it);
				if (cmd == "set")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);

					if (opt == "size")
					{
						if (params.size() == 1)
							setSize(params[0].cast<Size>());
						else if (params.size() == 2)
							setSize(Size(params[0].cast<int>(), params[1].cast<int>()));
					}
					else if (opt == "pos")
					{
						if (params.size() == 1)
							setPos(params[0].cast<Point>());
						else if (params.size() == 2)
							setPos(Point(params[0].cast<int>(), params[1].cast<int>()));
					}
					else if (opt == "rect")
					{
						if (params.size() == 1)
							setRect(params[0].cast<Rect>());
						else if (params.size() == 2)
							setRect(Rect(params[0].cast<Point>(), params[1].cast<Size>()));
						else if (params.size() == 4)
							setRect(Rect(params[0].cast<int>(), params[1].cast<int>(), params[2].cast<int>(), params[3].cast<int>()));
					}
				}
				else if (cmd == "get")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);

					if (opt == "size")
						return getSize();
					else if (opt == "pos")
						return getPos();
					else if (opt == "rect")
						return getRect();
				}
				return nullptr;
			}


			virtual core::Any execute(const std::string& command)
			{
				std::stringstream strbuf(command);
				std::string cmd;
				strbuf >> cmd;
				for (auto& it : cmd)
					it = tolower(it);
				if (cmd == "set")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);

					std::vector<std::string> param_list;
					while (!strbuf.eof())
					{
						std::string param;
						strbuf >> param;
						if (param.size() > 0)
							param_list.emplace_back(param);
					}

					if (opt == "size")
					{
						if (param_list.size() == 2)
							setSize(Size(std::atoi(param_list[0].c_str()), std::atoi(param_list[1].c_str())));
					}
					else if (opt == "pos")
					{
						if (param_list.size() == 2)
							setPos(Point(std::atoi(param_list[0].c_str()), std::atoi(param_list[1].c_str())));
					}
					else if (opt == "rect")
					{
						if (param_list.size() == 4)
							setRect(Rect(std::atoi(param_list[0].c_str()), std::atoi(param_list[1].c_str()), std::atoi(param_list[2].c_str()), std::atoi(param_list[3].c_str())));
					}
				}
				else if (cmd == "get")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);

					if (opt == "size")
						return getSize();
					else if (opt == "pos")
						return getPos();
					else if (opt == "rect")
						return getRect();
				}
				return nullptr;
			}

		private:

			template<typename Param>
			void _Pack_Params(std::vector<CraftEngine::core::Any>& package, const Param& param)
			{
				package.emplace_back(core::Any(param));
			}

			template<typename Param, typename...Params>
			void _Pack_Params(std::vector<CraftEngine::core::Any>& package, const Param& param, const Params&... params)
			{
				package.emplace_back(core::Any(param));
				_Pack_Params(package, params...);
			}

		};




		class RootWidget : public Widget
		{
		private:
			friend class Widget;

			bool m_isCursorEnable = true;

			Point m_cursorPos;
			bool m_isLButtonDown = false;
			bool m_isRButtonDown = false;
			Point m_LButtonDownPoint;
			Point m_RButtonDownPoint;
			Widget* m_cursorOnComponent = nullptr;
			Widget* m_cursorFocusComponent = nullptr;
			Widget* m_inputFocusComponent = nullptr;
			AbstractGuiRenderer* m_renderer = nullptr;
			std::vector<Widget*> m_popupWidgetsList;
			SyncTimerSystem* m_timerSystem;
			TaskManager* m_taskManager;

			AbstractCursor* m_cursor = nullptr;
			AbstractCursor* m_system_cursor_list[16];
		protected:
			int m_shouldRepaint = 1;
		protected:

			void _On_Child_Widget_Destroy(Widget* widget)
			{
				if (widget == m_cursorOnComponent)
					m_cursorOnComponent = nullptr;
				if (widget == m_cursorFocusComponent)
					m_cursorFocusComponent = nullptr;
				if (widget == m_inputFocusComponent)
					m_inputFocusComponent = nullptr;

				for(auto i = m_popupWidgetsList.begin(); i != m_popupWidgetsList.end(); i++)
					if ((*i) == widget)
					{
						m_popupWidgetsList.erase(i);
						break;
					}
			}

			void _On_Child_Widget_Force_Input_Focus(Widget* widget, bool focus)
			{
				if (widget == nullptr)
					return;

				if (m_inputFocusComponent != nullptr)
				{
					if (m_inputFocusComponent == widget)
					{
						if (focus == false)
						{
							MouseEvent mouseEvent;
							mouseEvent.type = MouseEvent::eFocusOut;
							m_inputFocusComponent->setInputFocus(false);
							m_inputFocusComponent->onMouseEvent(mouseEvent);
							m_inputFocusComponent = this;
						}
					}
					else
					{
						if (focus == true)
						{
							MouseEvent mouseEvent;
							mouseEvent.type = MouseEvent::eFocusOut;
							m_inputFocusComponent->setInputFocus(false);
							m_inputFocusComponent->onMouseEvent(mouseEvent);
							m_inputFocusComponent = widget;
							updateRootWidget(m_inputFocusComponent);
							mouseEvent.type = MouseEvent::eFocusIn;
							m_inputFocusComponent->setInputFocus(true);
							m_inputFocusComponent->onMouseEvent(mouseEvent);
						}
					}
				}
				else
				{
					if (focus == true)
					{
						m_inputFocusComponent = widget;
						updateRootWidget(m_inputFocusComponent);
						MouseEvent mouseEvent;
						mouseEvent.type = MouseEvent::eFocusIn;
						m_inputFocusComponent->setInputFocus(true);
						m_inputFocusComponent->onMouseEvent(mouseEvent);
						m_inputFocusComponent = this;
					}
				}
			}

			void _Root_Widget_Init_Future()
			{
				m_system_cursor_list[AbstractCursor::SystemCursorType::eArrowCursor] = (AbstractCursor*)GuiAssetsManager::loadCursor("Cursor_Arrow").unused;
				m_system_cursor_list[AbstractCursor::SystemCursorType::eEditCursor] = (AbstractCursor*)GuiAssetsManager::loadCursor("Cursor_Edit").unused;
				m_system_cursor_list[AbstractCursor::SystemCursorType::eSizeHorCursor] = (AbstractCursor*)GuiAssetsManager::loadCursor("Cursor_SizeHor").unused;
				m_system_cursor_list[AbstractCursor::SystemCursorType::eSizeVerCursor] = (AbstractCursor*)GuiAssetsManager::loadCursor("Cursor_SizeVer").unused;
				m_system_cursor_list[AbstractCursor::SystemCursorType::eSizeDiaCursor1] = (AbstractCursor*)GuiAssetsManager::loadCursor("Cursor_SizeDia1").unused;
				m_system_cursor_list[AbstractCursor::SystemCursorType::eSizeDiaCursor2] = (AbstractCursor*)GuiAssetsManager::loadCursor("Cursor_SizeDia2").unused;

				m_cursor = m_system_cursor_list[AbstractCursor::SystemCursorType::eArrowCursor];
			}

			virtual Widget* onDetect(const MouseEvent& mouseEvent)
			{
				if (isHide() || isTransparent() || isPopupable() || !getRect().inside(mouseEvent.local))
					return nullptr;
				MouseEvent subEvent = mouseEvent;
				Widget* popupWidget = nullptr;
				for (int i = m_popupWidgetsList.size() - 1; i >= 0; i--)
				{
					subEvent.local = m_popupWidgetsList[i]->getParentWidget()->getLocalPos(subEvent.global, m_popupWidgetsList[i]->getGroupid());
					popupWidget = m_popupWidgetsList[i]->onMouseEvent(subEvent);
					if (popupWidget != nullptr)
						return popupWidget;
				}
				return Widget::onDetect(mouseEvent);
			}
		public:

			AbstractGuiRenderer* getRenderer()
			{
				return m_renderer;
			}

			SyncTimerSystem* getSyncTimerSystem()
			{
				return m_timerSystem;
			}
			
			TaskManager* getTaskManager()
			{
				return m_taskManager;
			}

			bool addAtomicTask(std::function<void(core::Any&)>&& sync, std::function<void(core::Any&)>&& async = [](core::Any&) {})
			{
				return m_taskManager->addTask({ sync, async });
			}

			/*
			 popupWidget
			 @param widget: widget

			*/
			void popupWidget(Widget* widget)
			{
				if (!widget->isPopupable())
					throw std::runtime_error("This widget is not popupable!");
				widget->setNowPopup(true);
				m_popupWidgetsList.push_back(widget);
			}

			void unpopupWidget(Widget* widget)
			{
				for (int i = 0; i < m_popupWidgetsList.size(); i++)
				{
					if (m_popupWidgetsList[i] == widget)
					{
						m_popupWidgetsList.erase(m_popupWidgetsList.begin() + i);
						widget->setNowPopup(false);
						return;
					}
				}
				throw std::runtime_error("This widget is not popup!");
			}

			bool isCursorEnable() { return m_isCursorEnable; }
			void setCursorEnable(bool enable) { m_isCursorEnable = enable; }

		protected:

			void setGuiRenderer(AbstractGuiRenderer* renderer)
			{
				if (renderer == nullptr)
					throw std::runtime_error("renderer is nullptr!");
				m_renderer = renderer;
			}


		public:
			craft_engine_gui_signal(renderStarted, void());
			craft_engine_gui_signal(renderFinished, void());
			craft_engine_gui_signal(resized, void(const Size&));

			
			RootWidget(RootWidget* parent) :Widget()
			{
				m_timerSystem = new SyncTimerSystem;
				m_taskManager = new TaskManager;
			}
			~RootWidget()
			{
				_Destroy_Children_Widget();
				delete m_taskManager;
				delete m_timerSystem;
			}
			virtual void setWindowSize(const Size& size) = 0;
			virtual Size getWindowSize() = 0;
			virtual void exec() = 0;
			virtual void exit() = 0;

			// Optional extend function
			virtual void setSystemCursorHide(bool enable) {}
			virtual bool isSystemCursorHide() { return false; }
			void         forceUpdateFrame() { m_shouldRepaint = 2; }

		private:

			void onButtonDoubleClickedEx(int button)
			{
				MouseEvent mouseEvent;
				mouseEvent.button = button;
				mouseEvent.type = MouseEvent::eDoubleClicked;
				mouseEvent.global = m_cursorPos;
				if (m_cursorOnComponent != nullptr)
					m_cursorOnComponent->onMouseEvent(mouseEvent);

				if (m_cursorFocusComponent != nullptr)
				{
					auto offset = m_cursorPos - m_LButtonDownPoint;
					// 1.
					if ((offset.x != 0 || offset.y != 0) && m_cursorFocusComponent->isDragable() && (m_cursorFocusComponent->isLeftButtonDrag() ? 0 : 1) == button)
					{
						MouseEvent mouseEvent;
						mouseEvent.button = button;
						mouseEvent.type = MouseEvent::eDragApply;
						mouseEvent.global = m_cursorPos;
						mouseEvent.offset = offset;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
					// 2.
					{
						MouseEvent mouseEvent;
						mouseEvent.button = button;
						mouseEvent.type = MouseEvent::eReleased;
						mouseEvent.global = m_cursorPos;
						m_cursorFocusComponent->setFocus(false);
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				{
					MouseEvent mouseEvent;
					mouseEvent.button = button;
					mouseEvent.type = MouseEvent::ePressed;
					mouseEvent.global = m_cursorPos;
					m_cursorFocusComponent = m_cursorOnComponent;
					updateRootWidget(m_cursorFocusComponent);
					if (m_cursorFocusComponent != nullptr)
					{
						m_cursorFocusComponent->setFocus(true);
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
			}

		protected:

			virtual void onWindowDestroy() {}
			virtual void onWindowFocus(bool focus) final
			{
				if (!focus)
					KeyBoard::setAllKeyUp();
			}

			virtual void onRenderStarted() final
			{
				craft_engine_gui_emit(renderStarted, );
			}

			virtual void onRenderFinished() final
			{
				craft_engine_gui_emit(renderFinished, );
			}

			virtual void onPaintEvent() override final
			{
				auto duration = m_renderer->getFramDuration();

				PaintEvent paintEvent;
				paintEvent.pRenderer = m_renderer;
				paintEvent.globalRelativePoint = Offset(0, 0);
				paintEvent.globalLimitRect = getRect();
				paintEvent.globalAlpha = 255;
				Widget::processPaintEvent(paintEvent);
				Widget::onPaintEvent();
				for (int i = 0; i < m_popupWidgetsList.size(); i++)
				{
					paintEvent.pRenderer = m_renderer;
					paintEvent.globalRelativePoint = m_popupWidgetsList[i]->getParentWidget()->getGlobalPos(Point(0));
					paintEvent.globalLimitRect = getRect();
					paintEvent.globalAlpha = m_popupWidgetsList[i]->getParentWidget()->m_tempAlpha;
					m_popupWidgetsList[i]->processPaintEvent(paintEvent);
					m_popupWidgetsList[i]->onPaintEvent();
				}

				auto painter = getPainter();
				AbstractCursor::CursorPaintEvent cursorPaintEvent;
				cursorPaintEvent.painter = &painter;
				cursorPaintEvent.globalPos = m_cursorPos;
				cursorPaintEvent.scalar = 1.00f;
				if (isCursorEnable() && isSystemCursorHide())
					if (m_cursor != nullptr)
						m_cursor->onPaint(cursorPaintEvent);
					else
						m_system_cursor_list[AbstractCursor::SystemCursorType::eArrowCursor]->onPaint(cursorPaintEvent);
			}

			virtual void onWindowResize(uint32_t width, uint32_t height) {}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override 
			{
				craft_engine_gui_emit(resized, resizeEvent.size);
			}

			virtual void onKeyDown(int key) final
			{
				KeyBoard::setKeyDown(KeyBoard::KeyVal(key));
				if (m_inputFocusComponent != nullptr)
				{
					KeyboardEvent keyboardEvent;
					keyboardEvent.key = KeyBoard::KeyVal(key);
					keyboardEvent.down = true;
					m_inputFocusComponent->onKeyBoardEvent(keyboardEvent);
				}
			}

			virtual void onKeyUp(int key) final
			{
				KeyBoard::setKeyUp(KeyBoard::KeyVal(key));
				if (m_inputFocusComponent != nullptr)
				{
					KeyboardEvent keyboardEvent;
					keyboardEvent.key = KeyBoard::KeyVal(key);
					keyboardEvent.down = false;
					m_inputFocusComponent->onKeyBoardEvent(keyboardEvent);
				}
			}

			virtual void onMouseWheel(int offset) final
			{
				if (m_cursorOnComponent != nullptr)
				{
					MouseEvent mouseEvent;
					mouseEvent.button = MouseEvent::eNoButton;
					mouseEvent.type = MouseEvent::eWheel;
					mouseEvent.offset = Offset(0, offset);
					m_cursorOnComponent->onWheel(mouseEvent);
				}
			}

			virtual void onMouseMove(int mouseX, int mouseY) final
			{
				auto offset = Point(mouseX, mouseY) - m_cursorPos;
				//bool should_update_cursor = true;
				m_cursorPos.x = mouseX;
				m_cursorPos.y = mouseY;

				updateMouseOnWidget();
				if (m_cursorFocusComponent != nullptr && m_isLButtonDown && m_cursorFocusComponent->isLeftButtonDrag())
				{
					auto offset = m_cursorPos - m_LButtonDownPoint;
					if ((offset.x != 0 || offset.y != 0) && m_cursorFocusComponent->isDragable())
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eLeftButton;
						mouseEvent.type = MouseEvent::eDrag;
						mouseEvent.global = m_LButtonDownPoint;
						mouseEvent.offset = offset;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				if (m_cursorFocusComponent != nullptr && m_isRButtonDown && !m_cursorFocusComponent->isLeftButtonDrag())
				{
					auto offset = m_cursorPos - m_RButtonDownPoint;
					if ((offset.x != 0 || offset.y != 0) && m_cursorFocusComponent->isDragable())
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eRightButton;
						mouseEvent.type = MouseEvent::eDrag;
						mouseEvent.global = m_RButtonDownPoint;
						mouseEvent.offset = offset;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				updateCursor();
			}


			virtual void onLButtonDoubleClicked()
			{
				onButtonDoubleClickedEx(MouseEvent::eLeftButton);
				m_isLButtonDown = true;
			}

			virtual void onRButtonDoubleClicked()
			{
				onButtonDoubleClickedEx(MouseEvent::eRightButton);
				m_isRButtonDown = true;
			}


			void updateRootWidget(Widget* widget)
			{
				if (widget != nullptr)
					widget->m_rootWidget = this;
			}

			void updateMouseOnWidget() 
			{
				auto offset = Point(0, 0);
				Widget* cur_component = nullptr;
				{
					MouseEvent mouseEvent;
					mouseEvent.button = MouseEvent::eNoButton;
					mouseEvent.type = MouseEvent::eDetect;
					mouseEvent.global = m_cursorPos;
					mouseEvent.local = m_cursorPos;
					cur_component = this->onMouseEvent(mouseEvent);
				}
				if (cur_component != nullptr && m_cursorOnComponent == cur_component)
				{
					MouseEvent mouseEvent;
					mouseEvent.button = MouseEvent::eNoButton;
					mouseEvent.type = MouseEvent::eMoving;
					mouseEvent.global = m_cursorPos;
					mouseEvent.offset = offset;
					m_cursorOnComponent->onMouseEvent(mouseEvent);
				}
				else
				{
					// ²»Í¬
					if (m_cursorOnComponent != nullptr)
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eNoButton;
						mouseEvent.type = MouseEvent::eMoveOut;
						mouseEvent.global = m_cursorPos;
						m_cursorOnComponent->setHighlight(false);
						m_cursorOnComponent->onMouseEvent(mouseEvent);
					}
					m_cursorOnComponent = cur_component;
					updateRootWidget(m_cursorOnComponent);
					if (m_cursorOnComponent != nullptr)
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eNoButton;
						mouseEvent.type = MouseEvent::eMoveIn;
						mouseEvent.global = m_cursorPos;
						m_cursorOnComponent->setHighlight(true);
						m_cursorOnComponent->onMouseEvent(mouseEvent);
					}
				}
				updateCursor();
			}

			void updateMouseFocusWidget(int button)
			{
				if (m_cursorFocusComponent != nullptr)
				{
					auto offset = m_cursorPos - m_LButtonDownPoint;
					// 1.
					if ((offset.x != 0 || offset.y != 0) && m_cursorFocusComponent->isDragable() && (m_cursorFocusComponent->isLeftButtonDrag() ? 0 : 1) == button)
					{
						MouseEvent mouseEvent;
						mouseEvent.button = button;
						mouseEvent.type = MouseEvent::eDragApply;
						mouseEvent.global = m_cursorPos;
						mouseEvent.offset = offset;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
					// 2.
					{
						MouseEvent mouseEvent;
						mouseEvent.button = button;
						mouseEvent.type = MouseEvent::eReleased;
						mouseEvent.global = m_cursorPos;
						m_cursorFocusComponent->setFocus(false);
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}

				{
					MouseEvent mouseEvent;
					mouseEvent.button = button;
					mouseEvent.type = MouseEvent::ePressed;
					mouseEvent.global = m_cursorPos;
					m_cursorFocusComponent = m_cursorOnComponent;
					updateRootWidget(m_cursorFocusComponent);
					if (m_cursorFocusComponent != nullptr)
					{
						m_cursorFocusComponent->setFocus(true);
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
			}

			void updateMouseInputFocusWidget()
			{
				if (m_cursorFocusComponent != m_inputFocusComponent)
				{
					if (m_inputFocusComponent != nullptr)
					{
						{
							MouseEvent mouseEvent;
							mouseEvent.button = MouseEvent::eNoButton;
							mouseEvent.type = MouseEvent::eFocusOut;
							m_inputFocusComponent->setInputFocus(false);
							m_inputFocusComponent->onMouseEvent(mouseEvent);
						}
						auto w = m_inputFocusComponent;
						auto p = m_inputFocusComponent->getParentWidget();
						while (p != nullptr)
						{
							p->onChildFocusOut(w);
							if (!p->onChildFocusTransitive())
								break;
							w = p;
							p = p->getParentWidget();
						}
					}
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eNoButton;
						mouseEvent.type = MouseEvent::eFocusIn;
						mouseEvent.global = m_cursorPos;
						mouseEvent.local = m_cursorPos;
						m_inputFocusComponent = m_cursorOnComponent;
						updateRootWidget(m_inputFocusComponent);
						if (m_inputFocusComponent != nullptr)
						{
							m_inputFocusComponent->setInputFocus(true);
							m_inputFocusComponent->onMouseEvent(mouseEvent);
						}
					}
					if (m_inputFocusComponent != nullptr)
					{
						auto w = m_inputFocusComponent;
						auto p = m_inputFocusComponent->getParentWidget();
						while (p != nullptr)
						{
							p->onChildFocusIn(w);
							if (!p->onChildFocusTransitive())
								break;
							w = p;
							p = p->getParentWidget();
						}
					}
				}
			}

			void updatePopupWidgets()
			{
				for (auto pop : m_popupWidgetsList)
				{
					if ((m_cursorFocusComponent != nullptr && !m_cursorFocusComponent->haveParentWidget(pop))
						&& pop->shouldPopupCancel())
						unpopupWidget(pop);
				}
			}

			void updateCursor()
			{
				if (m_isLButtonDown == true || m_cursorOnComponent == nullptr)
					return;
				m_cursor = m_cursorOnComponent->getCursor();
				if (m_cursor == nullptr)
					m_cursor = m_system_cursor_list[m_cursorOnComponent->getCursorType()];
			}

			virtual void onLButtonDown() final
			{
				updateMouseOnWidget();
				updateMouseFocusWidget(MouseEvent::eLeftButton);
				updateMouseInputFocusWidget();
				updatePopupWidgets();
				updateCursor();
				m_isLButtonDown = true;
				m_LButtonDownPoint = m_cursorPos;
			}

			virtual void onLButtonUp() final
			{
				updateMouseOnWidget();
				if (m_cursorFocusComponent != nullptr)
				{
					MouseEvent mouseEvent;
					mouseEvent.button = MouseEvent::eLeftButton;
					mouseEvent.type = MouseEvent::eReleased;
					mouseEvent.global = m_cursorPos;
					m_cursorFocusComponent->setFocus(false);
					m_cursorFocusComponent->onMouseEvent(mouseEvent);
				}

				if (m_cursorOnComponent != nullptr)
				{
					{ // ???
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eLeftButton;
						mouseEvent.type = MouseEvent::eReleased;
						mouseEvent.global = m_cursorPos;
						m_cursorOnComponent->setFocus(false);
						m_cursorOnComponent->onMouseEvent(mouseEvent);
					}
					if (m_cursorFocusComponent == m_cursorOnComponent && m_cursorFocusComponent->isClickable())
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eLeftButton;
						mouseEvent.type = MouseEvent::eClicked;
						mouseEvent.global = m_cursorPos;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				if (m_cursorFocusComponent != nullptr && m_isLButtonDown)
				{
					auto offset = m_cursorPos - m_LButtonDownPoint;
					if ((offset.x != 0 || offset.y != 0) && m_cursorFocusComponent->isDragable() && m_cursorFocusComponent->isLeftButtonDrag())
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eLeftButton;
						mouseEvent.type = MouseEvent::eDragApply;
						mouseEvent.global = m_LButtonDownPoint;
						mouseEvent.offset = offset;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				m_LButtonDownPoint = m_cursorPos;
				m_isLButtonDown = false;
				m_cursorFocusComponent = nullptr;
				updateMouseOnWidget();
				updateCursor();
			}

			virtual void onRButtonDown() final
			{
				updateMouseOnWidget();
				updateMouseFocusWidget(MouseEvent::eRightButton);
				updateMouseInputFocusWidget();
				updatePopupWidgets();
				updateCursor();
				m_isRButtonDown = true;
				m_RButtonDownPoint = m_cursorPos;
			}

			virtual void onRButtonUp() final
			{
				updateMouseOnWidget();
				if (m_cursorFocusComponent != nullptr)
				{
					MouseEvent mouseEvent;
					mouseEvent.button = MouseEvent::eRightButton;
					mouseEvent.type = MouseEvent::eReleased;
					mouseEvent.global = m_cursorPos;
					m_cursorFocusComponent->setFocus(false);
					m_cursorFocusComponent->onMouseEvent(mouseEvent);
				}
				if (m_cursorOnComponent != nullptr)
				{
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eRightButton;
						mouseEvent.type = MouseEvent::eReleased;
						mouseEvent.global = m_cursorPos;
						m_cursorOnComponent->setFocus(false);
						m_cursorOnComponent->onMouseEvent(mouseEvent);
					}
					if (m_cursorFocusComponent == m_cursorOnComponent) // && m_cursorFocusComponent->isClickable())
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eRightButton;
						mouseEvent.type = MouseEvent::eCallMenu;
						mouseEvent.global = m_cursorPos;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				if (m_cursorFocusComponent != nullptr && m_isLButtonDown && !m_cursorFocusComponent->isLeftButtonDrag())
				{
					auto offset = m_cursorPos - m_LButtonDownPoint;
					if ((offset.x != 0 || offset.y != 0) && m_cursorFocusComponent->isDragable())
					{
						MouseEvent mouseEvent;
						mouseEvent.button = MouseEvent::eRightButton;
						mouseEvent.type = MouseEvent::eDragApply;
						mouseEvent.offset = offset;
						m_cursorFocusComponent->onMouseEvent(mouseEvent);
					}
				}
				m_RButtonDownPoint = m_cursorPos;
				m_isRButtonDown = false;
				//m_cursorFocusComponent = nullptr;
				updateMouseOnWidget();
				updateCursor();
			}


			virtual void onMouseLeave() final
			{
				if (m_cursorOnComponent != nullptr)
				{
					MouseEvent mouseEvent;
					mouseEvent.button = MouseEvent::eNoButton;
					mouseEvent.type = MouseEvent::eMoveOut;
					m_cursorOnComponent->setHighlight(false);
					m_cursorOnComponent->onMouseEvent(mouseEvent);
				}
				m_cursorOnComponent = nullptr;
			}

			virtual void onCharInput(wchar_t c) final
			{
				if (m_inputFocusComponent != nullptr && m_inputFocusComponent->isCharAcceptable())
					m_inputFocusComponent->onAcceptChar(c);
			}

			virtual void onDropFiles(const StringList& filelist) final
			{
				if (m_cursorOnComponent != nullptr)
					m_cursorOnComponent->onAcceptFiles(filelist);
			}

		protected:

			void _Resize_No_Event(const Size& size)
			{
				m_rect.mSize = size;
			}

			virtual core::Any execute(const std::string& command, const std::vector<core::Any>& params) override
			{
				std::stringstream strbuf(command);
				std::string cmd;
				strbuf >> cmd;
				for (auto& it : cmd)
					it = tolower(it);
				if (cmd == "set")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);

					if (opt == "size")
					{
						if (params.size() == 1)
							setWindowSize(params[0].cast<Size>());
						else if (params.size() == 2)
							setWindowSize(Size(params[0].cast<int>(), params[1].cast<int>()));
					}

				}
				else if (cmd == "get")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);
					if (opt == "size")
						return getSize();
				}
				return nullptr;
			}

			virtual core::Any execute(const std::string& command) override
			{
				std::stringstream strbuf(command);
				std::string cmd;
				strbuf >> cmd;
				for (auto& it : cmd)
					it = tolower(it);
				if (cmd == "set")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);

					std::vector<std::string> param_list;
					while (!strbuf.eof())
					{
						std::string param;
						strbuf >> param;
						if (param.size() > 0)
							param_list.emplace_back(param);
					}

					if (opt == "size")
					{
						if (param_list.size() == 2)
							setWindowSize(Size(std::atoi(param_list[0].c_str()), std::atoi(param_list[1].c_str())));
					}
				}
				else if (cmd == "get")
				{
					std::string opt;
					strbuf >> opt;
					for (auto& it : opt)
						it = tolower(it);
					if (opt == "size")
						return getSize();
				}
				return nullptr;
			}


		};


		void Widget::_On_Destroy()
		{
			if (m_rootWidget != nullptr)
				m_rootWidget->_On_Child_Widget_Destroy(this);
		}
		void Widget::_Destroy_Children_Widget()
		{
			core::ArrayList<Widget*> temp_children_list(0);
			m_children.swap(temp_children_list);
			for (int i = 0; i < temp_children_list.size(); i++)
				delete temp_children_list[i];
		}
		void Widget::forceInputFocus(bool enable)
		{
			auto root = getRootWidget();
			root->_On_Child_Widget_Force_Input_Focus(this, enable);
		}
		void Widget::sendRepaintEvent()
		{
			auto root = getRootWidget();
			if (root != nullptr)
				root->forceUpdateFrame();
		}



	}

}



#endif // !CRAFT_ENGINE_GUI_WIDGET_H_


