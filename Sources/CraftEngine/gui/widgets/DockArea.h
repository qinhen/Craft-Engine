#pragma once
#include "./FrameWidget.h"
#include "./Slider.h"
#include "./Label.h"
namespace CraftEngine
{
	namespace gui
	{

		class DockArea : public Widget
		{
		private:
			class DockTitleWidget : public Label
			{
			public:
				bool m_labelEnable = false;

				DockTitleWidget(DockArea* parent) : Label(parent)
				{
					setClickable(true);

				}
				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					drawBackground();
					if (m_labelEnable)
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
					else if (isHighlight())
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
					Label::onPaint_drawIcon();
					Label::onPaint_drawText();
					drawFrame();
				}
				virtual void onClicked(const MouseEvent& mouseEvent) override
				{
					((DockArea*)getParentWidget())->_On_Label_Clicked(this);
				}
			};

			void _On_Label_Clicked(DockTitleWidget* w)
			{
				for (int i = 0; i < m_dockTitleWidgets.size(); i++)
				{
					m_dockTitleWidgets[i]->m_labelEnable = m_dockTitleWidgets[i] == w;
					if (m_dockTitleWidgets[i]->m_labelEnable == true)
						m_enableLabelIndex = i;
				}
			}

		public:
			struct DockResult
			{
				DockArea* mSrc;
				DockArea* mFirst;
				DockArea* mSecond;

				DockResult(DockArea* src)
				{
					mSrc = src;
					mFirst = src;
					mSecond = src;
				}
				DockResult(DockArea* src, DockArea* first)
				{
					mSrc = src;
					mFirst = first;
					mSecond = first;
				}
				DockResult(DockArea* src, DockArea* first, DockArea* second)
				{
					mSrc = src;
					mFirst = first;
					mSecond = second;
				}
			};

			enum DockMode
			{
				eDockMode_Auto,
				eDockMode_Fixed,
				eDockMode_Ratio,
			};

		private:

			enum DockDir
			{
				eDockDir_NoDir,
				eDockDir_X,
				eDockDir_Y,
			};
			DockArea*                  m_rootDockArea;
			DockArea*                  m_parentDockArea;
			DockDir                    m_childrenDockDir;
			core::ArrayList<DockArea*> m_childrenDockArea;
			core::ArrayList<Slider*>   m_childrenSeparators;
			core::ArrayList<float>     m_childrenSplitRates;

			core::ArrayList<Widget*>   m_dockWidgets;
			core::ArrayList<DockTitleWidget*>   m_dockTitleWidgets;
			int m_enableLabelIndex = -1;
			DockMode                   m_dockMode = eDockMode_Auto;
			float                      m_dockValue = 0.0f;
			DockArea(DockArea* rootDockArea, DockArea* parentDockArea) : Widget(nullptr)
			{
				m_rootDockArea = rootDockArea;
				m_parentDockArea = parentDockArea;
				m_childrenDockDir = eDockDir_NoDir;
				bindParentWidget(parentDockArea);
			}

			bool IsRootDockArea() const { return m_parentDockArea == nullptr; }
			bool IsLeafDockArea() const { return m_childrenDockArea.size() == 0; }
		public:
			DockArea(Widget* parent) : Widget(parent)
			{
				m_rootDockArea = this;
				m_parentDockArea = nullptr;
				m_childrenDockDir = eDockDir_NoDir;
			}

			~DockArea()
			{
				for (auto it : m_childrenDockArea)
				{
					it->unbindParentWidget();
					delete it;
				}
				for (auto it : m_dockWidgets)
				{
					it->unbindParentWidget();
					delete it;
				}
			}
		public:
			void enableLabel(int index)
			{
				for (auto it : m_dockTitleWidgets)
					it->m_labelEnable = false;
				m_enableLabelIndex = index;
				m_dockTitleWidgets[index]->m_labelEnable = true;
			}

		public:

			void setTitle(int index, const String& title)
			{
				if (index < m_dockWidgets.size())
				{
					m_dockTitleWidgets[index]->setText(title);
					updateTitleLayout();
				}
			}

			DockArea* dockCenter(Widget* widget)
			{
				if (widget->getParentWidget() != nullptr)
				{
					widget->getParentWidget()->unbindChildWidget(widget);
					widget->unbindParentWidget();
				}
				widget->bindParentWidget(this);
				m_dockWidgets.push_back(widget);
				m_dockTitleWidgets.push_back(new DockTitleWidget(this));
				if (m_dockWidgets.size() == 1)
					enableLabel(0);
				updateDockLayout(false, eDockDir_NoDir);
				return this;
			}

			DockResult dockLeft(Widget* widget, DockMode mode = eDockMode_Auto, float value = 0.0f)
			{
				if (IsRootDockArea() && IsLeafDockArea() && m_dockWidgets.size() == 0)
					return DockResult(dockCenter(widget));
				return dockHelp(widget, true, eDockDir_X, mode, value);
			}
			DockResult dockRight(Widget* widget, DockMode mode = eDockMode_Auto, float value = 0.0f)
			{
				if (IsRootDockArea() && IsLeafDockArea() && m_dockWidgets.size() == 0)
					return DockResult(dockCenter(widget));
				return dockHelp(widget, false, eDockDir_X, mode, value);
			}
			DockResult dockTop(Widget* widget, DockMode mode = eDockMode_Auto, float value = 0.0f)
			{
				if (IsRootDockArea() && IsLeafDockArea() && m_dockWidgets.size() == 0)
					return DockResult(dockCenter(widget));
				return dockHelp(widget, true, eDockDir_Y, mode, value);
			}
			DockResult dockBottom(Widget* widget, DockMode mode = eDockMode_Auto, float value = 0.0f)
			{
				if (IsRootDockArea() && IsLeafDockArea() && m_dockWidgets.size() == 0)
					return DockResult(dockCenter(widget));
				return dockHelp(widget, false, eDockDir_Y, mode, value);
			}

			void undock(Widget* widget)
			{

			}
		protected:
			DockResult dockHelp(Widget* widget, bool isFirst, DockDir dir, DockMode mode, float value);

			void updateSeperatorsLimit();

			void updateDockLayout(bool change_rate, DockDir dir);

			void updateTitleLayout()
			{
				if (m_dockTitleWidgets.size() == 1)
				{
					m_dockTitleWidgets[0]->setRect(Rect(0, 0, getWidth(), 22));
				}
				else
				{
					int x = 0;
					for (auto it : m_dockTitleWidgets)
					{
						it->setRect(Rect(x, 0, it->getMinSizeLimit().x + 10, 22));
						x += it->getMinSizeLimit().x + 10;
					}
				}
			}
		protected:
			virtual Widget* onDetect(const MouseEvent& mouseEvent) override
			{
				if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
					return nullptr;

				Widget* hit = nullptr;
				MouseEvent subEvent = mouseEvent;
				subEvent.local = mouseEvent.local - getPos() - getChildOriginal(GroupID::gDefaultWidget);
				if (!IsLeafDockArea())
				{
					for (auto it : m_childrenSeparators)
					{
						hit = it->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
					for (auto it : m_childrenDockArea)
					{
						hit = it->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
				}
				else
				{
					for (auto it : m_dockTitleWidgets)
					{
						hit = it->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
					if (m_enableLabelIndex >= 0)
					{
						hit = m_dockWidgets[m_enableLabelIndex]->onMouseEvent(subEvent);
						if (hit != nullptr)
							return hit;
					}
				}
				return this;
			}

			virtual void onPaintEvent() override
			{
				if (!IsLeafDockArea())
				{
					for (auto it : m_childrenSeparators)
						drawChild2(it);
					for (auto it : m_childrenDockArea)
						drawChild2(it);
				}
				else
				{
					for (auto it : m_dockTitleWidgets)
						drawChild(it);
					if (m_enableLabelIndex >= 0)
						drawChild2(m_dockWidgets[m_enableLabelIndex]);
				}
			}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				if (IsRootDockArea())
					updateDockLayout(false, eDockDir_NoDir);
			}
		};


	}
}



namespace CraftEngine
{
	namespace gui
	{


		DockArea::DockResult DockArea::dockHelp(Widget* widget, bool isFirst, DockDir dir, DockMode mode, float value)
		{
			if (widget->getParentWidget() != nullptr)
			{
				widget->getParentWidget()->unbindChildWidget(widget);
				widget->unbindParentWidget();
			}

			if (m_childrenDockArea.size() == 1 || m_childrenDockDir == dir)
			{
				m_childrenDockDir = dir;
				auto area = new DockArea(m_rootDockArea, this);
				area->m_dockMode = mode;
				area->m_dockValue = value;

				widget->bindParentWidget(area);
				area->m_dockWidgets.push_back(widget);
				area->m_dockTitleWidgets.push_back(new DockTitleWidget(area));

				auto sep = new Slider(this);
				sep->setCursorType(dir == eDockDir_X ? AbstractCursor::eSizeHorCursor : AbstractCursor::eSizeVerCursor);
				sep->setSlideCallBack(
					core::Callback<void(void)>([=]() {
					updateDockLayout(true, dir);
				})
				);

				if (m_childrenSeparators.size() == 0)
				{
					m_childrenDockArea.push_back(area);
					m_childrenSeparators.push_back(sep);
					m_childrenSplitRates.push_back(0.0f);
					if (dir == eDockDir_X)
					{
						int x = (getWidth() - 5) / 2;
						if (mode == eDockMode_Ratio)
							x = getWidth() * value - 5;
						sep->setRect(Rect(x, 0, 5, getHeight()));
						m_childrenSplitRates[m_childrenSplitRates.size() - 1] = (getWidth() - sep->getRect().right()) / float(getWidth());
					}
					else
					{
						int y = (getHeight() - 5) / 2;
						if (mode == eDockMode_Ratio)
							y = getHeight() * value - 5;
						sep->setRect(Rect(0, y, getWidth(), 5));
						m_childrenSplitRates[m_childrenSplitRates.size() - 1] = (getHeight() - sep->getRect().bottom()) / float(getHeight());
					}
				}
				else
				{
					if (isFirst)
					{
						m_childrenDockArea.insert(0, area);
						m_childrenSeparators.insert(0, sep);
						m_childrenSplitRates.insert(0, 0.0f);

						if (dir == eDockDir_X)
						{
							int x = (m_childrenSeparators[1]->getRect().left() - 5) / 2;
							if (mode == eDockMode_Ratio)
								x = getWidth() * value - 5;
							sep->setRect(Rect(x, 0, 5, getHeight()));
							m_childrenSplitRates[0] = sep->getRect().right() / float(getWidth());
							m_childrenSplitRates[1] = m_childrenSplitRates[1] - m_childrenSplitRates[0];
						}
						else
						{
							int y = (m_childrenSeparators[1]->getRect().top() - 5) / 2;
							if (mode == eDockMode_Ratio)
								y = getHeight() * value - 5;
							sep->setRect(Rect(0, y, getWidth(), 5));
							m_childrenSplitRates[0] = sep->getRect().bottom() / float(getHeight());
							m_childrenSplitRates[1] = m_childrenSplitRates[1] - m_childrenSplitRates[0];
						}
					}
					else
					{
						m_childrenDockArea.push_back(area);
						m_childrenSeparators.push_back(sep);
						m_childrenSplitRates.push_back(0.0f);
						if (dir == eDockDir_X)
						{
							int x = m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().right() + 
								(getWidth() - m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().right()) / 2;
							if (mode == eDockMode_Ratio)
								x = m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().right() + getWidth() * value - 5;
							sep->setRect(Rect(x, 0, 5, getHeight()));
							m_childrenSplitRates[m_childrenSplitRates.size() - 1] = (sep->getRect().right() - 
								m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().right()) / float(getWidth());
						}
						else
						{
							int y = m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().bottom() +
								(getHeight() - m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().bottom()) / 2;
							if (mode == eDockMode_Ratio)
								y = m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().bottom() + getHeight() * value - 5;
							sep->setRect(Rect(0, y, getWidth(), 5));
							m_childrenSplitRates[m_childrenSplitRates.size() - 1] = (sep->getRect().bottom() - 
								m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().bottom()) / float(getHeight());
						}
					}
				}

				updateDockLayout(false, dir);
				return { this, area };
			}
			else
			{
				auto first = new DockArea(m_rootDockArea, this);
				auto second = new DockArea(m_rootDockArea, this);
				DockArea* docks[2] = { first, second };
				if (!isFirst)
				{
					docks[0] = second;
					docks[1] = first;
				}

				docks[0]->m_dockMode = mode;
				docks[0]->m_dockValue = value;
				docks[1]->m_dockMode = m_dockMode;
				docks[1]->m_dockValue = m_dockValue;


				widget->bindParentWidget(docks[0]);

				if (!IsLeafDockArea())
				{
					docks[0]->m_dockWidgets.push_back(widget);
					docks[0]->m_dockTitleWidgets.push_back(new DockTitleWidget(docks[0]));
					for (auto it : m_childrenDockArea)
					{
						it->unbindParentWidget();
						it->bindParentWidget(docks[1]);
					}
					docks[1]->m_childrenDockArea.swap(m_childrenDockArea);

					for (auto it : m_childrenSeparators)
					{
						unbindChildWidget(it); // 
						it->unbindParentWidget();
						it->bindParentWidget(docks[1]);
						docks[1]->bindChildWidget(it);
						it->setSlideCallBack(
							core::Callback<void(void)>([=]() {
							docks[1]->updateDockLayout(true, dir);
						})
						);
					}
					docks[1]->m_childrenSeparators.swap(m_childrenSeparators);
					docks[1]->m_childrenSplitRates.swap(m_childrenSplitRates);
					docks[1]->m_childrenDockDir = m_childrenDockDir;
				}
				else
				{
					for (auto it : m_dockWidgets)
					{
						it->unbindParentWidget();
						it->bindParentWidget(docks[1]);
					}
					for (auto it : m_dockTitleWidgets)
					{
						it->unbindParentWidget();
						it->bindParentWidget(docks[1]);
					}
									
					docks[1]->m_dockWidgets.swap(m_dockWidgets);
					docks[0]->m_dockWidgets.push_back(widget);
					docks[1]->m_dockTitleWidgets.swap(m_dockTitleWidgets);
					docks[0]->m_dockTitleWidgets.push_back(new DockTitleWidget(docks[0]));
				}


				m_childrenDockDir = dir;
				m_childrenDockArea.resize(2);
				m_childrenDockArea[0] = first;
				m_childrenDockArea[1] = second;


				auto sep = new Slider(this);
				m_childrenSeparators.push_back(sep);
				sep->setCursorType(dir == eDockDir_X ? AbstractCursor::eSizeHorCursor : AbstractCursor::eSizeVerCursor);
				sep->setSlideCallBack(
					core::Callback<void(void)>([=]() {
					updateDockLayout(true, dir);
				})
				);

				if (dir == eDockDir_X)
				{
					int x = (getWidth() - 5) / 2;
					if (mode == eDockMode_Ratio)
						x = getWidth() * value - 5;
					sep->setRect(Rect(x, 0, 5, getHeight()));
					m_childrenSplitRates.push_back(sep->getRect().right() / float(getWidth()));
				}
				else
				{
					int y = (getHeight() - 5) / 2;
					if (mode == eDockMode_Ratio)
						y = getHeight() * value - 5;
					sep->setRect(Rect(0, y, getWidth(), 5));
					m_childrenSplitRates.push_back(sep->getRect().bottom() / float(getHeight()));
				}

				docks[0]->enableLabel(0);
				docks[1]->enableLabel(0);
				this->m_enableLabelIndex = -1;

				updateDockLayout(false, dir);
				return { this, docks[0], docks[1] };
			}

		}

		void DockArea::updateSeperatorsLimit()
		{
			if (m_childrenDockDir == eDockDir_X)
			{
				for (auto it : m_childrenSeparators)
					it->setRect(Rect(it->getX(), 0, 5, getHeight()));

				if (m_childrenSeparators.size() > 1)
					m_childrenSeparators[0]->setLimit(Rect(0, 0, m_childrenSeparators[1]->getRect().left(), getHeight()));
				else if (m_childrenSeparators.size() > 0)
					m_childrenSeparators[0]->setLimit(Rect(0, 0, getWidth(), getHeight()));

				for (int i = 1; i < m_childrenSeparators.size() - 1; i++)
					m_childrenSeparators[i]->setLimit(Rect(m_childrenSeparators[i - 1]->getRect().right(), 0, m_childrenSeparators[i + 1]->getRect().left() -
						m_childrenSeparators[i - 1]->getRect().right(), getHeight()));

				if (m_childrenSeparators.size() > 1)
					m_childrenSeparators.back()->setLimit(Rect(m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().right(), 0, getWidth() - 
						m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().right(), getHeight()));
			}
			else if (m_childrenDockDir == eDockDir_Y)
			{
				for (auto it : m_childrenSeparators)
					it->setRect(Rect(0, it->getY(), getWidth(), 5));

				if (m_childrenSeparators.size() > 1)
					m_childrenSeparators[0]->setLimit(Rect(0, 0, getWidth(), m_childrenSeparators[1]->getRect().top()));
				else if (m_childrenSeparators.size() > 0)
					m_childrenSeparators[0]->setLimit(Rect(0, 0, getWidth(), getHeight()));

				for (int i = 1; i < m_childrenSeparators.size() - 1; i++)
					m_childrenSeparators[i]->setLimit(Rect(0, m_childrenSeparators[i - 1]->getRect().bottom(), getWidth(), m_childrenSeparators[i + 1]->getRect().top() - 
						m_childrenSeparators[i - 1]->getRect().bottom()));

				if (m_childrenSeparators.size() > 1)
					m_childrenSeparators.back()->setLimit(Rect(0, m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().bottom(), getWidth(), getHeight() - 
						m_childrenSeparators[m_childrenSeparators.size() - 2]->getRect().bottom()));
			}
		}



		void DockArea::updateDockLayout(bool change_rate, DockDir dir)
		{
			switch (m_childrenDockDir)
			{
			case eDockDir_NoDir:
			{
				updateTitleLayout();
				for (auto it : m_dockWidgets)
					it->setRect(Rect(0, 22, getWidth(), getHeight() - 22));
				break;
			}
			case eDockDir_X:
				if (change_rate)
				{
					for (int i = 0; i < m_childrenSplitRates.size(); i++)
						m_childrenSplitRates[i] = (m_childrenSeparators[i]->getRect().right() - (i > 0 ? m_childrenSeparators[i - 1]->getRect().right() : 0)) / float(getWidth());
				}
				else if (dir == eDockDir_NoDir || dir == eDockDir_X)
				{
					int x = 0;
					for (int i = 0; i < m_childrenSplitRates.size(); i++)
					{
						x += m_childrenSplitRates[i] * getWidth();
						m_childrenSeparators[i]->setRect(Rect(x - 5, 0, 5, getHeight()));
					}
				}
				updateSeperatorsLimit();

				m_childrenDockArea[0]->setRect(Rect(0, 0, m_childrenSeparators[0]->getX(), getHeight()));
				for (int i = 1; i < m_childrenSeparators.size(); i++)
					m_childrenDockArea[i]->setRect(Rect(m_childrenSeparators[i - 1]->getRect().right(), 0,
						m_childrenSeparators[i]->getRect().left() - 
						m_childrenSeparators[i - 1]->getRect().right(), getHeight()));
				m_childrenDockArea.back()->setRect(Rect(m_childrenSeparators.back()->getRect().right(), 0,
					getWidth() - m_childrenSeparators.back()->getRect().right(), getHeight()));

				for (auto it : m_childrenDockArea)
					it->updateDockLayout(false, dir);
				break;
			case eDockDir_Y:
				if (change_rate)
				{
					for (int i = 0; i < m_childrenSplitRates.size(); i++)
						m_childrenSplitRates[i] = (m_childrenSeparators[i]->getRect().bottom() - (i > 0 ? m_childrenSeparators[i - 1]->getRect().bottom() : 0)) / float(getHeight());
				}
				else if (dir == eDockDir_NoDir || dir == eDockDir_Y)
				{
					int y = 0;
					for (int i = 0; i < m_childrenSplitRates.size(); i++)
					{
						y += m_childrenSplitRates[i] * getHeight();
						m_childrenSeparators[i]->setRect(Rect(0, y - 5, getWidth(), 5));
					}
				}
				updateSeperatorsLimit();

				m_childrenDockArea[0]->setRect(Rect(0, 0, getWidth(), m_childrenSeparators[0]->getY()));
				for (int i = 1; i < m_childrenSeparators.size(); i++)
					m_childrenDockArea[i]->setRect(Rect(0, m_childrenSeparators[i - 1]->getRect().bottom(), getWidth(),
						m_childrenSeparators[i]->getRect().top() - 
						m_childrenSeparators[i - 1]->getRect().bottom()));
				m_childrenDockArea.back()->setRect(Rect(0, m_childrenSeparators.back()->getRect().bottom(), getWidth(),
					getHeight() - m_childrenSeparators.back()->getRect().bottom()));

				for (auto it : m_childrenDockArea)
					it->updateDockLayout(false, dir);
				break;
			}

		}

	}
}