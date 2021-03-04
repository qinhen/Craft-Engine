#pragma once
#include "./Widget.h"
#include "./PushButton.h"
namespace CraftEngine
{
	namespace gui
	{

		class FrameWidget : public Widget
		{
		private:
			int32_t     m_titleBarWidth = 25;
			PushButton* m_miniButton;
			PushButton* m_maxiButton;
			PushButton* m_closeButton;
			String      m_title;
			Point       m_basepoint;

			bool        m_isTough = false;
			bool        m_isResizable = true;
			int         m_resizeState = 0;
			
			bool        m_minimized = false;
			bool        m_maximized = false;
			Rect        m_tempRect;

			bool        m_isDraging = false;
			bool        m_isResizing = false;
			bool        m_titleBarHide = false;
		protected:

			virtual Point getChildOriginal(int groupid = GroupID::gDefaultWidget)const override
			{
				switch (groupid)
				{
				case GroupID::gDefaultWidget:
					return getOriginalPoint();
				case GroupID::gFrameButton:
					return Point(0);
				default:
					return Point(0);
				}
			}
		public:


			FrameWidget(const String& title, const Rect& rect, Widget* parent) : Widget(rect, parent)
			{
				setMinSizeLimit(Size(120, m_titleBarWidth));
				getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
				getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
				setDragable(true);

				m_miniButton = new PushButton(nullptr);
				m_maxiButton = new PushButton(nullptr);
				m_closeButton = new PushButton(nullptr);
				m_miniButton->setIcon(GuiAssetsManager::loadImage("Minimize"));
				m_maxiButton->setIcon(GuiAssetsManager::loadImage("Maximize"));
				m_closeButton->setIcon(GuiAssetsManager::loadImage("Exit"));
				PushButton* buttons[3] = { m_miniButton,m_maxiButton,m_closeButton };
				for (int i = 0; i < 3; i++)
				{
					buttons[i]->setGroupid(GroupID::gFrameButton);
					buttons[i]->bindParentWidget(this);
					buttons[i]->getPalette().mFillMode = Palette::eFillMode_None;
				}

				craft_engine_gui_connect_v2(m_miniButton, clicked, [=]() {
					setMinimize(!m_minimized);
				});

				craft_engine_gui_connect_v2(m_maxiButton, clicked, [=]() {
					setMaximize(!m_maximized);
				});

				craft_engine_gui_connect_v2(m_closeButton, clicked, [=]() {

				});

				setTitle(title);
			}

			FrameWidget(Widget* parent) : FrameWidget(L"", Rect(0, 0, 400, 400), parent)
			{

			}

			~FrameWidget()
			{
				delete m_closeButton;
				delete m_maxiButton;
				delete m_miniButton;
			}

			void setMinimize(bool enable)
			{
				if (m_maximized)
				{

				}
				else if (enable && !m_minimized)
				{
					m_minimized = true;
					m_tempRect = getRect();
					setSize(Size(getWidth(), getTitleSize().y));
				}
				else if (!enable && m_minimized)
				{
					m_minimized = false;
					setSize(Size(getWidth(), m_tempRect.mSize.y));
				}
			}

			void setMaximize(bool enable)
			{
				if (enable && !m_maximized)
				{
					setMinimize(false);
					m_maximized = true;
					m_tempRect = getRect();
					setRect(Rect(Point(0), getParentWidget()->getSize()));
				}
				else if (!enable && m_maximized)
				{
					m_maximized = false;
					setRect(m_tempRect);
				}
			}

			void setTitle(const String& text)
			{
				m_title = text;
				_Reset_Title_Text_Layout();
			}
			const String& getTitle() const { return m_title; }
			void setResizable(bool enable) { m_isResizable = enable; }
			bool isResizable() const { return m_isResizable; }
			void setTough(bool enable) { m_isTough = enable; }
			bool isTough() const { return m_isTough; }
			bool isDraging() const { return m_isDraging; }
			bool isResizing() const { return m_isResizing; }

			Point getOriginalPoint() const { return Point(0, m_titleBarWidth); }
			Size getAvailableSize() const { return Point(getWidth(), getHeight() - m_titleBarWidth); }
			Size getTitleSize() const { return Point(getWidth(), m_titleBarWidth); }
			Rect getAvailableRect() const { return Rect(getPos() - getOriginalPoint(), getAvailableSize()); }


			void setTitleBarHide(bool enable)
			{
				m_titleBarHide = enable;
				if (enable)
					m_titleBarWidth = 0;
				else
					m_titleBarWidth = 25;
				_Reset_Title_Text_Layout();
			}

		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { _Reset_Title_Text_Layout(); }

			virtual Widget* onDetect(const MouseEvent& mouseEvent) override final
			{
				Widget* hit;
				if (isHide() || isTransparent())
					return nullptr;

				if (m_isResizable && !m_isTough && !m_maximized)
				{
					if (isFocus())
						return this;
					m_resizeState = _Detect_Frame(mouseEvent.global);
					if (m_resizeState > 0)
					{
						switch (m_resizeState)
						{
						case 0x1:
						case 0x2:
							setCursorType(AbstractCursor::SystemCursorType::eSizeHorCursor);
							break;
						case 0x4:
						case 0x8:
							setCursorType(AbstractCursor::SystemCursorType::eSizeVerCursor);
							break;
						case 0x1 | 0x4:
						case 0x2 | 0x8:
							setCursorType(AbstractCursor::SystemCursorType::eSizeDiaCursor1);
							break;
						case 0x1 | 0x8:
						case 0x2 | 0x4:
							setCursorType(AbstractCursor::SystemCursorType::eSizeDiaCursor2);
							break;
						}

						return this;
					}
					setCursorType(AbstractCursor::SystemCursorType::eArrowCursor);
				}

				if (!getRect().inside(mouseEvent.local))
					return nullptr;

				MouseEvent subEvent = mouseEvent;
				Point relative_point = mouseEvent.local - getPos();
				if (!m_titleBarHide)
				{
					subEvent.local = relative_point - getChildOriginal(GroupID::gFrameButton);
					hit = m_miniButton->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
					hit = m_maxiButton->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
					hit = m_closeButton->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}

				subEvent.local = relative_point - getChildOriginal(GroupID::gDefaultWidget);
				for (int i = getChildrenWidgets().size() - 1; i >= 0; i--) //
				{
					hit = getChildrenWidgets()[i]->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}
				return this;
			}

			virtual void onPaintEvent() override
			{
				auto painter = getThisPainter();
				drawBackground();

				if (!m_minimized)
					drawAllChildren();

				if (!m_titleBarHide)
				{
					switch (getPalette().mFillMode)
					{
					case Palette::FillMode::eFillMode_None:
						break;
					case Palette::FillMode::eFillMode_Color:
						//if (isInputFocus())
						//	painter.drawRect(Rect(Point(0), getTitleSize()), getPalette().mBackgroundFocusColor);
						//else
						painter.drawRect(Rect(Point(0), getTitleSize()), getPalette().mBackgroundHighLightColor);
						break;
					case Palette::FillMode::eFillMode_Image:
						painter.drawRect(Rect(Point(0), getTitleSize()), getPalette().mBackgroundImage);
						break;
					default:
						break;
					}

					painter.drawTextLine(m_title, m_basepoint, getFont(), getPalette().mForegroundColor);
					drawChild(m_miniButton);
					drawChild(m_maxiButton);
					drawChild(m_closeButton);
				}

				drawFrame();

				if (m_resizeState > 0)
				{
					auto color = getPalette().mForegroundHighLightColor;
					if (m_resizeState & 0x1)
						painter.drawRect(Rect(getWidth() + 1, 0, 1, getHeight()), color);
					else if(m_resizeState & 0x2)
						painter.drawRect(Rect(-1, 0, 1, getHeight()), color);
					if (m_resizeState & 0x4)
						painter.drawRect(Rect(0, getHeight() + 1, getWidth(), 1), color);
					else if (m_resizeState & 0x8)
						painter.drawRect(Rect(0, -1, getWidth(), 1), color);
				}

			}

			virtual void onDoubleClicked(const MouseEvent& mouseEvent) override
			{
				if (!m_titleBarHide)
				{
					auto base_point = Widget::getGlobalPos(Point(0), 1) - getDragOffset();
					auto src_global_title_rect = Rect(base_point, getTitleSize());
					if (!src_global_title_rect.inside(mouseEvent.global))
						return;
					setMaximize(!m_maximized);
				}
				//setTitleBarHide(!m_titleBarHide);
			}

			virtual void onDragApply(const MouseEvent& mouseEvent) override
			{
				m_isResizing = false;
				m_isDraging = false;
				Widget::onDragApply(mouseEvent);
			}

			virtual void onDrag(const MouseEvent& mouseEvent) override
			{
				if (m_isTough || m_maximized)
					return;

				if (m_resizeState > 0)
				{
					auto rect = getRect();
					auto offset = mouseEvent.offset - getDragOffset();
					auto min_size = getMinSizeLimit();
					auto max_size = getMaxSizeLimit();
					auto real_offset = mouseEvent.offset;
					auto temp_size = getSize();
					auto temp_offset = Offset(0);
					auto temp_point = 0;

					if (m_resizeState & 0x1)
					{
						rect.mWidth += offset.x;
						rect.mWidth = math::clamp(rect.mWidth, min_size.x, max_size.x);
						temp_offset.x = rect.mWidth - temp_size.x;
					}
					else if (m_resizeState & 0x2)
					{
						temp_point = rect.right();
						rect.mWidth -= offset.x;
						rect.mWidth = math::clamp(rect.mWidth, min_size.x, max_size.x);
						rect.mX = temp_point - rect.mWidth;
						temp_offset.x = temp_size.x - rect.mWidth;
					}
					if (m_resizeState & 0x4)
					{
						rect.mHeight += offset.y;
						rect.mHeight = math::clamp(rect.mHeight, min_size.y, max_size.y);		
						temp_offset.y = rect.mHeight - temp_size.y;
					}
					else if (m_resizeState & 0x8)
					{
						temp_point = rect.bottom();
						rect.mHeight -= offset.y;
						rect.mHeight = math::clamp(rect.mHeight, min_size.y, max_size.y);
						rect.mY = temp_point - rect.mHeight;
						temp_offset.y = temp_size.y - rect.mHeight;
					}
					real_offset = getDragOffset() + temp_offset;
					setDragOffset(real_offset);
					setRect(rect);
					m_isResizing = true;
				}
				else
				{
					auto base_point = Widget::getGlobalPos(Point(0), 1) - getDragOffset();
					auto src_global_title_rect = Rect(base_point, getTitleSize());
					if (src_global_title_rect.inside(mouseEvent.global))
						Widget::onDrag(mouseEvent);
					m_isDraging = true;
				}
			}

			virtual bool onChildFocusTransitive() override { return false; }

		private:

			int _Detect_Frame(const Point& global, int frame_size = 3)
			{
				auto local_pos = getLocalPos(global, 0xFFFF);
				auto limit = Rect(Point(0), getSize());
				limit.mOffset -= frame_size * 2;
				limit.mSize += frame_size * 4;
				if (!limit.inside(local_pos))
					return 0;
				auto center = getSize() / 2;
				auto distance = local_pos - center;
				auto radius_min = center - frame_size;
				auto radius_max = center + frame_size * 2;
				int result = 0;
				if (distance.x > radius_min.x && distance.x < radius_max.x)
					result |= 0x1;
				else if (distance.x < -radius_min.x && distance.x > -radius_max.x)
					result |= 0x2;
				if (!m_minimized && distance.y > radius_min.y && distance.y < radius_max.y)
					result |= 0x4;
				else if (!m_minimized && distance.y < -radius_min.y && distance.y > -radius_max.y)
					result |= 0x8;
				return result;
			}

		protected:
			void _Reset_Title_Text_Layout()
			{
				auto button_height = m_titleBarWidth - 3;
				m_basepoint = GuiFontSystem::calcFontBasePoint(m_title, Rect(Point(0), getTitleSize()), getFont());
				auto y = (getTitleSize().y - button_height) / 2;
				m_closeButton->setRect(Rect(getWidth() - 30, y, 30, button_height));
				m_maxiButton->setRect(Rect(getWidth() - 60, y, 30, button_height));
				m_miniButton->setRect(Rect(getWidth() - 90, y, 30, button_height));
			}
		};



	}
}
