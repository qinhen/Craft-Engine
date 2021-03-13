#pragma once
#ifndef CRAFT_ENGINE_GUI_COMMON_H_
#define CRAFT_ENGINE_GUI_COMMON_H_


// ref lib
#include "../core/Core.h"
#include "../core/Bitmap.h"
#include "../math/LinearMath.h"


#define CRAFT_ENGINE_GUI_API CRAFT_ENGINE_INLINE

namespace CraftEngine
{

	namespace gui
	{

		typedef math::ivec2              Point;
		typedef math::vec2               PointF;
		typedef math::ivec2              Offset;
		typedef math::ivec2              Size;
		typedef math::Vector<4, uint8_t> Color;

		typedef core::Char       Char;
		typedef core::String     String;
		typedef core::StringRef  StringRef;
		typedef core::StringTool StringTool;
		typedef core::LinkedList<String> StringList;
		//typedef std::vector<String> StringList;

		namespace _Gui_Api_Detail
		{
			CRAFT_ENGINE_STATIC void _Load_Default_Assets();
			CRAFT_ENGINE_STATIC void _Load_Default_Font_File();
			CRAFT_ENGINE_STATIC void _Load_Fixed_Width_Font_File();
		}




		craft_engine_make_handle(HandleImage);
		craft_engine_make_handle(HandleCursor);

		struct ImageInfo
		{
			int mWidth;
			int mHeight;
			int mChannels;
			int mMipLevels;
		};

		namespace _Gui_Api_Detail
		{
			using PFN_GuiSystemCallCopyStringToClipboard = void(*)(const String& str);
			using PFN_GuiSystemCallCopyStringFromClipboard = String(*)();

			struct GuiSystemCalls
			{
				PFN_GuiSystemCallCopyStringToClipboard func_CopyStringToClipboard;
				PFN_GuiSystemCallCopyStringFromClipboard func_CopyStringFromClipboard;
			}guiSystemCallFunctions;
		}

	

		class AbstractGuiRenderer;
		class AbstractGuiCanvas;
		class Painter;


		/*
		 rectangle struct
		 mOffset = Point(mX, mY)
		 mSize   = Size(mWidth, mHeight)
		*/
		union Rect
		{
			struct { int32_t mX, mY, mWidth, mHeight; };
			struct { Point mOffset; Size mSize; };

			Rect(const Rect& rect) :mX(rect.mX), mY(rect.mY), mWidth(rect.mWidth), mHeight(rect.mHeight) {}
			Rect(const Point& off, const Size& size) :mOffset(off), mSize(size) {}
			Rect() :mX(0U), mY(0U), mWidth(0U), mHeight(0U) {}
			Rect(int32_t Xx, int32_t Yy, int32_t Ww, int32_t Hh) :mX(Xx), mY(Yy), mWidth(Ww), mHeight(Hh) {};
			Rect& operator=(const Rect& rect) { mOffset = rect.mOffset; mSize = rect.mSize; return *this; }

			inline int32_t left()const { return mX; }
			inline int32_t right()const { return mX + mWidth; }
			inline int32_t top()const { return mY; }
			inline int32_t bottom()const { return mY + mHeight; }

			/* 交集 */
			inline Rect disjunction(const Rect& other)const {
				Rect result;
				result.mX = math::max(this->left(), other.left());
				result.mWidth = math::max(0, math::min(this->right(), other.right()) - result.mX);
				if (result.mWidth == 0)
					return Rect();
				result.mY = math::max(this->top(), other.top());
				result.mHeight = math::max(0, math::min(this->bottom(), other.bottom()) - result.mY);
				if (result.mHeight == 0)
					return Rect();
				return result;
			}

			/* 并集 */
			inline Rect conjunction(const Rect& other)const {
				Rect result;
				result.mX = math::min(this->left(), other.left());
				result.mWidth = math::max(this->right(), other.right()) - result.mX;
				result.mY = math::min(this->top(), other.top());
				result.mHeight = math::max(this->bottom(), other.bottom()) - result.mY;
				return result;
			}

			inline bool isNull()const
			{
				return this->mWidth == 0 || this->mHeight == 0;
			}
			inline bool equals(const Rect& rect)const
			{
				return rect.mX == mX && rect.mY == mY && rect.mWidth == mWidth && rect.mHeight == mHeight;
			}
			
			/* 交集 */
			static bool haveIntersection(const Rect& r0, const Rect& r1)
			{
				if (r0.left() > r1.right())
					return false;
				if (r0.right() < r1.left())
					return false;
				if (r0.top() > r1.bottom())
					return false;
				if (r0.bottom() < r1.top())
					return false;
				return true;
			}

			inline bool inside(const Rect& rect)const {
				return rect.left() >= this->left() && rect.top() >= this->top() && rect.right() <= this->right() && rect.bottom() <= this->bottom();
			}
			inline bool inside(const Point& point)const {
				return point.x >= this->left() && point.y >= this->top() && point.x < this->right() && point.y < this->bottom();
			}

			inline Rect padding(int x, int y)const {
				return Rect(mX + x, mY + y, mWidth - 2 * x, mHeight - 2 * y);
			}

			inline void print()
			{
				std::cout << "x, y, w, h = " << mX << ", " << mY << ", " << mWidth << ", " << mHeight << std::endl;
			}
		};


		class KeyBoard
		{
		public:
			enum KeyVal
			{
				eKey_Add = 0x6B, // VK_ADD,
				eKey_Subtract = 0x6D, // VK_SUBTRACT,
				eKey_Multiply = 0x6A, // VK_MULTIPLY,
				eKey_Divide = 0x6F, // VK_DIVIDE,
				eKey_Up = 0x26, // VK_UP,
				eKey_Down = 0x28, // VK_DOWN,
				eKey_Left = 0x25, // VK_LEFT,
				eKey_Right = 0x27, // VK_RIGHT,
				eKey_Escape = 0x1B, // VK_ESCAPE,
				eKey_Tab = 0x09, // VK_TAB,
				eKey_Space = 0x20, // VK_SPACE,
				eKey_Return = 0x0D, // VK_RETURN,//回车
				eKey_Shift = 0x10, // VK_SHIFT,
				//eKey_Shift = VK_, // VK_SHIFT,
				eKey_Control = 0x11, // VK_CONTROL,
				eKey_Back = 0x08, //VK_BACK,
				eKey_Home = 0x24, // VK_HOME
				eKey_End = 0x23, // VK_END

				eKey_Insert = 0x2D, // VK_DELETE
				eKey_Delete = 0x2E, // VK_DELETE

			};
		private:
			static bool m_keyIsDown[256];
		public:
			static bool isKeyDown(int key) { return m_keyIsDown[key]; }
			static bool isKeyDown(KeyVal key) { return m_keyIsDown[key]; }
			static void setKeyDown(KeyVal key) { m_keyIsDown[key] = true; }
			static void setKeyUp(KeyVal key) { m_keyIsDown[key] = false; }
			static void setAllKeyUp() {
				for (int i = 0; i < 256; i++)
					m_keyIsDown[i] = false;
			}
		};
		bool KeyBoard::m_keyIsDown[256] = { };


		class SyncTimerSystem
		{
		private:
			class TimerInfo
			{
			private:
				friend class SyncTimerSystem;
				float m_ms;
				float m_acc_ms;
				uint32_t m_execution_times;
				uint32_t m_max_execution_times;
				TimerInfo** m_ppThis;
				CraftEngine::core::Callback<void(float, float)> m_callback;
				bool m_enable;
			public:
				TimerInfo(float ms, uint32_t max_times, CraftEngine::core::Callback<void(float, float)> callback, TimerInfo** ppDest)
					:m_ms(ms), m_acc_ms(0), m_max_execution_times(max_times), m_execution_times(0), m_ppThis(ppDest), m_callback(callback), m_enable(false) {}

				void start() { m_acc_ms = 0; m_execution_times = 0; m_enable = true; }
				void pause() { m_enable = false; }
				void restart() { m_acc_ms = 0; m_execution_times = 0; m_enable = true; }
			};
		public:
			craft_engine_make_handle(TimerHandle);


			void getOne(float ms, uint32_t times, CraftEngine::core::Callback<void(float, float)> callback, TimerHandle* ppDest)
			{
				assert(ppDest != nullptr);
				auto pTimer = new TimerInfo(ms, times, callback, (TimerInfo**)ppDest);
				m_timerList.push_back(pTimer);
				*((TimerInfo**)(ppDest)) = pTimer;
			}
			void startOne(TimerHandle timer)
			{
				if (timer != nullptr)
					((TimerInfo*)(void*)timer)->start();
			}
			void restartOne(TimerHandle timer)
			{
				if (timer != nullptr)
					((TimerInfo*)(void*)timer)->restart();
			}
			void pauseOne(TimerHandle timer)
			{
				if (timer != nullptr)
					((TimerInfo*)(void*)timer)->pause();
			}
			void removeOne(TimerHandle pDest)
			{
				TimerInfo* pTimer = (TimerInfo*)(void*)pDest;
				TimerInfo** ppTimer = pTimer->m_ppThis;
				m_timerList.remove(pTimer);
				*ppTimer = nullptr;
				delete pTimer;
			}

			void step()
			{
				if (m_mutex.try_lock())
				{
					auto system_cur_time_point = std::chrono::system_clock::now();
					long long duration_us = std::chrono::duration_cast<std::chrono::nanoseconds>(system_cur_time_point - m_curTimePoint).count();
					m_curTimePoint = system_cur_time_point;
					float duration_ms = duration_us / 1000000.0;

					std::list<TimerInfo*> removeList;
					for (auto timer : m_timerList)
					{
						if (timer->m_enable)
						{
							timer->m_acc_ms += duration_ms;
							float dest_time_point = timer->m_ms * (1 + timer->m_execution_times) / timer->m_max_execution_times;
							if (timer->m_acc_ms > dest_time_point)
							{
								timer->m_callback.call(timer->m_acc_ms, timer->m_ms);
								removeList.push_back(timer);
							}
						}
					}
					for (auto timer : removeList)
					{
						removeOne(TimerHandle(timer));
					}
					m_mutex.unlock();
				}
			}
		private:
			std::mutex m_mutex;
			std::chrono::system_clock::time_point m_curTimePoint;
			std::list<TimerInfo*> m_timerList;
		};


		class Timer :public CraftEngine::core::NonCopyable
		{
		private:
			SyncTimerSystem::TimerHandle m_handle = SyncTimerSystem::TimerHandle(nullptr);
			CraftEngine::core::Callback<void(float, float)> m_callback;
			float m_duration = 0.0f;
			uint32_t m_maxExecution = 1;
			SyncTimerSystem* m_timer_sys = nullptr;
		public:
			void setCallback(CraftEngine::core::Callback<void(float, float)> callback)
			{
				m_callback = callback;
			}
			void setDuration(float duration)
			{
				m_duration = duration;
			}
			void setMaxExecution(uint32_t times)
			{
				m_maxExecution = times;
			}

			bool startTimer(SyncTimerSystem* timer_sys)
			{
				m_timer_sys = timer_sys;
				if (isValid())
				{
					timer_sys->startOne(m_handle);
					return true;
				}
				else
				{
					timer_sys->getOne(m_duration, m_maxExecution, m_callback, &m_handle);
					timer_sys->startOne(m_handle);
					return false;
				}
			}

			bool restartTimer()
			{
				if (isValid())
				{
					m_timer_sys->restartOne(m_handle);
					return true;
				}
			}

			bool stopTimer(SyncTimerSystem* timer_sys)
			{
				if (isValid())
				{
					timer_sys->removeOne(m_handle);
					return true;
				}
				else
					return false;
			}

			bool pauseTimer(SyncTimerSystem* timer_sys)
			{
				if (isValid())
				{
					timer_sys->pauseOne(m_handle);
					return true;
				}
				else
					return false;
			}

			bool isValid()
			{
				return m_handle != nullptr;
			}
		};


		class TaskManager
		{
		public:
			struct AtomicTask
			{
				std::function<void(core::Any& data)> mAsyncTask;
				std::function<void(core::Any& data)> mSyncTask;
				core::Any mResult;
			};
			
			bool addTask(AtomicTask&& task)
			{
				std::lock_guard<std::mutex> lock(m_asyncMutex);
				if (m_asyncTasks.size() > 1000)
					return false;
				m_asyncTasks.emplace(task);
				return true;
			}

			void solvingAsync(int32_t maxTask = -1)
			{
				const int maxTaskCount = maxTask;
				int curTaskCount = 0;
				while (1)
				{
					if (maxTaskCount > 0 && curTaskCount >= maxTaskCount)
						return;
					AtomicTask task;
					{
						std::lock_guard<std::mutex> lock(m_asyncMutex);
						if (m_asyncTasks.size() == 0)
							return;
						std::swap(task.mAsyncTask, m_asyncTasks.front().mAsyncTask);
						std::swap(task.mSyncTask, m_asyncTasks.front().mSyncTask);
						m_asyncTasks.pop();
					}
					task.mAsyncTask(task.mResult);
					{
						std::lock_guard<std::mutex> lock(m_syncMutex);
						m_syncTasks.emplace(task);
					}
				}
			}


			void solvingSync(int32_t maxTask = -1)
			{
				const int maxTaskCount = maxTask;
				int curTaskCount = 0;
				std::queue<AtomicTask> syncTasks;
				if (maxTask < 0)
				{
					std::lock_guard<std::mutex> lock(m_syncMutex);
					syncTasks.swap(m_syncTasks);
				}
				else
				{
					std::lock_guard<std::mutex> lock(m_syncMutex);
					for (int i = 0; i < maxTaskCount && m_syncTasks.size() > 0; i++)
					{
						syncTasks.emplace(m_syncTasks.front());
						m_syncTasks.pop();
					}
				}
				for (int i = 0; i < syncTasks.size(); i++)
				{
					syncTasks.front().mSyncTask(syncTasks.front().mResult);
					syncTasks.pop();
				}
			}


			void clearAllTask()
			{
				{
					std::lock_guard<std::mutex> lock(m_asyncMutex);
					while (m_asyncTasks.size() > 0)
						m_asyncTasks.pop();
				}
				{
					std::lock_guard<std::mutex> lock(m_syncMutex);
					while (m_syncTasks.size() > 0)
						m_syncTasks.pop();
				}
			}
		
		private:
			std::mutex m_asyncMutex;
			std::mutex m_syncMutex;
			std::queue<AtomicTask> m_asyncTasks;
			std::queue<AtomicTask> m_syncTasks;
		};




		struct Alignment
		{
			enum AlignmentMode
			{
				eAlignmentMode_Center,
				eAlignmentMode_Left,
				eAlignmentMode_Right,
				eAlignmentMode_Top,
				eAlignmentMode_Bottom,
			};

			AlignmentMode mHorizonMode;
			AlignmentMode mVerticalMode;
			int mHorizonOffset;
			int mVerticalOffset;

			Alignment(AlignmentMode horizon = AlignmentMode::eAlignmentMode_Left, AlignmentMode vertical = AlignmentMode::eAlignmentMode_Center, int hoffset = 2, int voffset = 0)
				:mHorizonMode(horizon), mVerticalMode(vertical), mHorizonOffset(hoffset), mVerticalOffset(voffset) {
			}
			void setHorizonAlignment(AlignmentMode horizon = AlignmentMode::eAlignmentMode_Left, int hoffset = 1) { mHorizonMode = horizon; mHorizonOffset = hoffset; }
			void setVerticalAlignment(AlignmentMode vertical = AlignmentMode::eAlignmentMode_Center, int voffset = 1) { mVerticalMode = vertical; mVerticalOffset = voffset; }
			bool operator==(const Alignment& right)const {
				return mHorizonMode == right.mHorizonMode &&
					mVerticalMode == right.mVerticalMode &&
					mHorizonOffset == right.mHorizonOffset &&
					mVerticalOffset == right.mVerticalOffset;
			}
			bool operator!=(const Alignment& right)const {
				return !(*this == right);
			}
		};




		class FontFile
		{
		public:

			union FileHeader
			{
				struct
				{
					char  title[16];
					char  name[32];
					int   fontCount;
					int   maxId;
					float yOffset;
					float yHeight;
					float yLineHeight;
					float yMaxOffset;
					float yMaxHeight;
					int fontDescriptorStart;
					int fontDescriptorLen;
					int fontImagePngStart;
					int fontImagePngLen;
					int fontImageWidth;
					int fontImageHeight;
				};
				struct
				{
					char fix128[128];
				};
			};
			struct FontUnit
			{
				int id;
				int xcoord;
				int ycoord;
				int width;
				int height;
				float xoffset;
				float yoffset;
				float xadvance;
			};
		public:

			~FontFile()
			{

			}

			FontFile()
			{

			}

			FontFile(const char* filename)
			{
				loadFromFile(filename);
			}

			bool loadFromFile(const char* filename)
			{
				std::ifstream ifs(filename, std::ios::binary);
				if (ifs.is_open())
				{
					ifs.read((char*)&this->header, sizeof(this->header));

					m_fontUnits.resize(header.fontCount);
					std::vector<unsigned char> fontimage(header.fontImagePngLen);

					ifs.seekg(header.fontDescriptorStart, std::ios::beg);
					ifs.read((char*)&m_fontUnits[0], header.fontDescriptorLen);

					ifs.seekg(header.fontImagePngStart, std::ios::beg);
					ifs.read((char*)&fontimage[0], header.fontImagePngLen);

					m_fontBitmap.loadFromMemory(fontimage.data(), fontimage.size(), true);
					m_isLoaded = true;
					return true;
				}
				m_isLoaded = false;
				return false;
			}

			bool loadFromMemory(const uint8_t* data, uint32_t size)
			{
				const uint8_t* ptr = data;
				memcpy(&this->header, ptr, sizeof(this->header));

				m_fontUnits.resize(header.fontCount);
				std::vector<unsigned char> fontimage(header.fontImagePngLen);
				memcpy(&m_fontUnits[0], data + header.fontDescriptorStart, header.fontDescriptorLen);
				memcpy(&fontimage[0], data + header.fontImagePngStart, header.fontImagePngLen);

				m_fontBitmap.loadFromMemory(fontimage.data(), fontimage.size(), true);
				m_isLoaded = true;		
				return true;
			}


			const core::Bitmap& getSdfImage()const
			{
				return m_fontBitmap;
			}

			float fontCount()const { return header.fontCount; }
			float fontYOffset()const { return header.yOffset; }
			float fontYHeight()const { return header.yHeight; }
			float fontYMaxOffset()const { return header.yMaxOffset; }
			float fontYMaxHeight()const { return header.yMaxHeight; }
			float fontYLineHeight()const { return header.yLineHeight; }
			bool  isLoaded()const { return m_isLoaded; }
			const core::ArrayList<FontUnit>& fontUnits()const { return m_fontUnits; }

		private:
			bool m_isLoaded = false;
			FileHeader header;
			core::ArrayList<FontUnit> m_fontUnits;
			core::Bitmap m_fontBitmap;
		};



		class Font
		{
		private:
			Alignment m_fontAlignment;
			float     m_fontSize;
			float     m_fontLineHeight;
			uint32_t  m_fontInterval;
			uint32_t  m_fontID;
			static uint32_t m_defaultFontID;
		public:
			Font(const Font& font) 
				:m_fontID(font.m_fontID), m_fontSize(font.m_fontSize), m_fontAlignment(font.m_fontAlignment), m_fontInterval(font.m_fontInterval), m_fontLineHeight(font.m_fontLineHeight)
			{

			}

			Font(float fontSize = 11.0f, int wordInterval = 0, const Alignment& alignment = Alignment(), float lineHeight = 1.0f, uint32_t id = 0)
				:m_fontID(m_defaultFontID), m_fontSize(fontSize), m_fontAlignment(alignment), m_fontInterval(wordInterval), m_fontLineHeight(lineHeight) {
			}
			//void setFontID(FontSystem::FontIndex id) { m_fontID = (int)id; }
			void setFontID(uint32_t id) { m_fontID = id; }
			float getFontID()const { return m_fontID; }
			void setSize(float size) { m_fontSize = size; }
			float getSize()const { return m_fontSize; }
			void setAlignment(const Alignment& alignment) { m_fontAlignment = alignment; }
			const Alignment& getAlignment()const { return m_fontAlignment; }
			Alignment& getAlignment() { return m_fontAlignment; }
			void setInterval(uint32_t i) { m_fontInterval = i; }
			uint32_t getInterval()const { return m_fontInterval; }
			void setLineHeight(float h) { m_fontLineHeight = h; }
			float getLineHeight()const { return m_fontLineHeight; }

			static void setDefaultFontID(uint32_t idx) { m_defaultFontID = idx; }

			bool operator==(const Font& right)const {
				return m_fontAlignment == right.m_fontAlignment &&
					m_fontSize == right.m_fontSize &&
					m_fontLineHeight == right.m_fontLineHeight &&
					m_fontInterval == right.m_fontInterval &&
					m_fontID == right.m_fontID;
			}
			bool operator!=(const Font& right)const {
				return !(*this == right);
			}
		};
		uint32_t Font::m_defaultFontID = 0;


		struct RichTextFormat
		{
		public:
			enum EdgeMode
			{
				eEdgeMode_Fill,
				eEdgeMode_FadeOut,
				eEdgeMode_FadeIn,
			};
			enum FontMode
			{
				eFontMode_Standard,
				eFontMode_Bold,
				eFontMode_Light,
			};
		private:
			// rich text attributes
			FontMode m_fontMode = eFontMode_Standard;
			EdgeMode m_fontEdgeMode = eEdgeMode_Fill;
			Color m_fontColor = Color(245, 245, 245, 255);
			Color m_fontEdgeColor = Color(0, 0, 0, 255);
			Color m_fontShadowColor = Color(0, 0, 0, 255);
			float m_fontEdgeSize = 0.75f;
			float m_fontSlope = 0.0f;
			float m_fontShadowSize = 0.5f;

		public:
			void setFontMode(FontMode mode) { m_fontMode = mode; }
			FontMode getFontMode()const { return m_fontMode; }
			void setEdgeMode(EdgeMode mode) { m_fontEdgeMode = mode; }
			EdgeMode getEdgeMode()const { return m_fontEdgeMode; }
			void setEdgeSize(float size) { m_fontEdgeSize = size; }
			float getEdgeSize()const { return m_fontEdgeSize; }
			void setEdgeColor(const Color& color) { m_fontEdgeColor = color; }
			const Color& getEdgeColor()const { return m_fontEdgeColor; }
			void setColor(const Color& color) { m_fontColor = color; }
			const Color& getColor()const { return m_fontColor; }
			void setSlope(float slope) { m_fontSlope = slope; }
			float getSlope()const { return m_fontSlope; }

			bool operator==(const RichTextFormat& right)const {
				return 	m_fontMode == right.m_fontMode &&
					m_fontEdgeMode == right.m_fontEdgeMode &&
					m_fontColor == right.m_fontColor &&
					m_fontEdgeColor == right.m_fontEdgeColor &&
					m_fontShadowColor == right.m_fontShadowColor &&
					m_fontEdgeSize == right.m_fontEdgeSize &&
					m_fontShadowSize == right.m_fontShadowSize &&
					m_fontSlope == right.m_fontSlope;
			}
			bool operator!=(const RichTextFormat& right)const {
				return !(*this == right);
			}
		};



		class GuiFontSystem
		{
		public:
			struct FontUnit
			{
				int id;
				int xcoord;
				int ycoord;
				int width;
				int height;
				float xoffset;
				float yoffset;
				float xadvance;
			};

			enum class FontIndex
			{
				eDefault = 0,			
				eFixedWidth = 31,
			};
		private:
			struct FontInstance
			{
				struct FontInfo
				{
					float yOffset;
					float yHeight;
					float yLineHeight;
					float yMaxOffset;
					float yMaxHeight;
					float scalar;
				};
				core::Array<FontUnit, 65535> m_fontUnits;
				FontInfo    m_fontInfo;
				HandleImage m_fontImage;
				String      m_name;
			};
			static FontInstance* m_instanceList[32];
		public:
			static void clearAllFont() { for (int i = 0; i < 32; i++) freeFont(i); }
			static void freeFont(uint32_t id);
			static void loadFont(const FontFile* font, uint32_t id);

			static core::ArrayList<int> getGlobalFontIDList() { 
				core::ArrayList<int> list;
				for (int i = 0; i < 32; i++)
					if (m_instanceList[i] != nullptr)
						list.push_back(i);
				return list;
			}

			static inline float getGlobalFontScalar(uint32_t id) { return m_instanceList[id]->m_fontInfo.scalar; }
			static inline float getGlobalFontYOffset(uint32_t id) { return m_instanceList[id]->m_fontInfo.yOffset; }
			static inline float getGlobalFontYHeight(uint32_t id) { return m_instanceList[id]->m_fontInfo.yHeight; }
			static inline float getGlobalFontYStandardLineHeight(uint32_t id) { return m_instanceList[id]->m_fontInfo.yLineHeight; }
			static inline float getGlobalFontYMaxOffset(uint32_t id) { return m_instanceList[id]->m_fontInfo.yMaxOffset; }
			static inline float getGlobalFontYMaxHeight(uint32_t id) { return m_instanceList[id]->m_fontInfo.yMaxHeight; }

			static inline const FontUnit& getGlobalFontUnit(Char idx, int id);
			static inline HandleImage getGlobalFontImage(int id);
			static inline float getGlobalFontCharWidth(Char c, int id);

			static inline float calcFontLineOffset(const Font& font)
			{
				return (getGlobalFontYOffset(font.getFontID()) + 0.5 *
					(getGlobalFontYHeight(font.getFontID()) - font.getLineHeight() * getGlobalFontYStandardLineHeight(font.getFontID()))) *
					getGlobalFontScalar(font.getFontID()) * font.getSize();
			}
			static inline float calcFontLineHeight(const Font& font)
			{
				return font.getLineHeight() * getGlobalFontYStandardLineHeight(font.getFontID()) * 
					getGlobalFontScalar(font.getFontID()) * font.getSize();
			}

			static inline int calcFontLineWidth(const Char* pStr, int32_t count, const Font& font);
			static inline int calcFontLineWidth(const String& str, const Font& font);
			static inline int calcFontLineWidth(const StringRef& str, const Font& font);
			static inline Point calcFontBasePoint(const Char* pStr, int32_t count, const Rect& rect, const Font& font);
			static inline Point calcFontBasePoint(const String& str, const Rect& rect, const Font& font);
			static inline Point calcFontBasePoint(const StringRef& str, const Rect& rect, const Font& font);
			static inline int calcFontPointerIndex(const Char* pStr, int32_t count, int xcooed, const Font& font);
			static inline int calcFontPointerIndex(const String& str, int xcoord, const Font& font);
			static inline int calcFontPointerIndex(const StringRef& str, int xcoord, const Font& font);
			static inline Rect calcFontCursorRect(const Char* pStr, int32_t count, Point basepoint, const Font& font);
			static inline Rect calcFontCursorRect(const String& str, Point basepoint, const Font& font);
			static inline Rect calcFontCursorRect(const StringRef& str, Point basepoint, const Font& font);

			static bool loadFromFile(const char* file, int id) 
			{ 
				FontFile f(file);
				if (f.isLoaded()) loadFont(&f, id);
				else return false; return true; 
			}
			static bool loadFromMemory(const void* data, uint32_t size, int id) 
			{
				FontFile f;
				f.loadFromMemory((uint8_t*)data, size);
				if (f.isLoaded())loadFont(&f, id);
				else return false; return true;
			}

			static inline int step_forward(float xadvance, float scalar, int interval)
			{
				return step_advance(xadvance, scalar) + interval;
			}
			static inline int step_advance(float xadvance, float scalar)
			{
				return roundf(xadvance * scalar);
				// return int(xadvance * scalar);
			}
		};
		GuiFontSystem::FontInstance* GuiFontSystem::m_instanceList[32];



		class RichTextBlock
		{
		public:
			Font           font;
			RichTextFormat format;
			String         string;
		};

		class RichText
		{
		private:
			core::LinkedList<RichTextBlock> m_blockList;
		public:
			String getString()const
			{
				String str;
				for (auto it : m_blockList)
					str += it.string;
				return str;
			}

			struct Iterator
			{
			private:
				RichText* m_text;
				core::LinkedList<RichTextBlock>::Iterator m_blockIt;
				uint32_t m_blockCharIdx;
			public:
				Iterator() : m_text(nullptr), m_blockIt(core::LinkedList<RichTextBlock>::Iterator()), m_blockCharIdx(0) {}
				Iterator(const Iterator& other) : m_text(other.m_text), m_blockIt(other.m_blockIt), m_blockCharIdx(other.m_blockCharIdx) {}
				Iterator& operator=(const Iterator& other) { m_text = (other.m_text); m_blockIt = (other.m_blockIt); m_blockCharIdx = (other.m_blockCharIdx); return *this; }
				Iterator(RichText* pText, const core::LinkedList<RichTextBlock>::Iterator& blockIt, uint32_t charIdx) :m_text(pText), m_blockIt(blockIt), m_blockCharIdx(charIdx) { }
				Iterator& operator++() { if (m_blockCharIdx < m_blockIt->string.size() - 1) m_blockCharIdx++; else { ++m_blockIt; m_blockCharIdx = 0; }return *this; }
				Iterator& operator--() { if (m_blockCharIdx > 0) m_blockCharIdx--; else { --m_blockIt; m_blockCharIdx = m_blockIt->string.size() - 1; }return *this; }
				Iterator operator+(int count) {
					auto it = *this;
					auto temp = count;
					while (count != 0)
					{
						if (count >= it.blockIt()->string.size() - it.charIdx())
						{
							count -= (it.blockIt()->string.size() - it.charIdx());
							auto bit = it.blockIt();
							++bit;
							it = Iterator(this->m_text, bit, 0);
						}
						else
						{
							return Iterator(this->m_text, this->m_blockIt, it.charIdx() + count);
						}
					}
				}

				bool operator==(const Iterator& right) const { return m_text == right.m_text && m_blockIt == right.m_blockIt && m_blockCharIdx == right.m_blockCharIdx; }
				bool operator!=(const Iterator& right) const { return !(*this == right); }
				const Char& operator*() const { return m_blockIt->string[m_blockCharIdx]; }
				Char& operator*() { return m_blockIt->string[m_blockCharIdx]; }
				const RichTextBlock& block()const { return *m_blockIt; }
				RichTextBlock& block() { return *m_blockIt; }
				core::LinkedList<RichTextBlock>::Iterator blockIt()const { return m_blockIt; }
				Char value()const { return m_blockIt->string[m_blockCharIdx]; }
				uint32_t charIdx()const { return m_blockCharIdx; }
			};
			struct ConstIterator
			{
			private:
				const RichText* m_text;
				core::LinkedList<RichTextBlock>::ConstIterator m_blockIt;
				uint32_t m_blockCharIdx;
			public:
				ConstIterator(const RichText* pText, const core::LinkedList<RichTextBlock>::ConstIterator& blockIt, uint32_t charIdx) :m_text(pText), m_blockIt(blockIt), m_blockCharIdx(charIdx) { }
				ConstIterator& operator++() { if (m_blockCharIdx < m_blockIt->string.size() - 1) m_blockCharIdx++; else { ++m_blockIt; m_blockCharIdx = 0; }return *this; }
				ConstIterator& operator--() { if (m_blockCharIdx > 0) m_blockCharIdx--; else { --m_blockIt; m_blockCharIdx = m_blockIt->string.size() - 1; }return *this; }
				bool operator==(const ConstIterator& right) const { return m_text == right.m_text && m_blockIt == right.m_blockIt && m_blockCharIdx == right.m_blockCharIdx; }
				bool operator!=(const ConstIterator& right) const { return !(*this == right); }
				const Char& operator*() const { return m_blockIt->string[m_blockCharIdx]; }
				const RichTextBlock& block()const { return *m_blockIt; }
				core::LinkedList<RichTextBlock>::ConstIterator blockIt()const { return m_blockIt; }
				Char value()const { return m_blockIt->string[m_blockCharIdx]; }
				uint32_t charIdx()const { return m_blockCharIdx; }
			};

			//const Char& front() const { return m_front->data; }
			//const Type& back()  const { return m_back->data; }
			//Type& front() { return m_front->data; }
			//Type& back() { return m_back->data; }


			void push_back(Char c, const Font& font, const RichTextFormat& format)
			{
				insert(end(), c, font, format);
			}

			void push_back(const RichTextBlock& textBlock)
			{
				insert(end(), textBlock);
			}

			void insert(const Iterator& pos, Char c, const Font& font, const RichTextFormat& format)
			{
				RichTextBlock textBlock;
				textBlock.font = font;
				textBlock.format = format;
				textBlock.string = String() + c;
				m_blockList.push_back(textBlock);
				insert(pos, textBlock);
			}

			Iterator insert(const Iterator& pos, const RichTextBlock& textBlock)
			{
				if (pos == end())
				{
					if (m_blockList.size() > 0)
						if (m_blockList.back().font == textBlock.font && m_blockList.back().format == textBlock.format)
							m_blockList.back().string += textBlock.string;
						else if (m_blockList.back().string.size() == 0)
							m_blockList.back() = textBlock;
						else
							m_blockList.push_back(textBlock);
					else
						m_blockList.push_back(textBlock);
					return end();
				}
				else if (textBlock.string.size() > 0)
				{
					if (pos.block().font == textBlock.font && pos.block().format == textBlock.format)
					{
						pos.blockIt()->string = pos.blockIt()->string.substr(0, pos.charIdx()) + textBlock.string + pos.blockIt()->string.substr(pos.charIdx());
						auto c_idx = pos.charIdx() + textBlock.string.size();
						if (c_idx == pos.blockIt()->string.size())
						{
							auto next_it = pos.blockIt();
							++next_it;
							return Iterator(this, next_it, 0);
						}
						else
						{
							return Iterator(this, pos.blockIt(), c_idx);
						}
					}
					else
					{
						if (pos.charIdx() > 0)
						{
							auto it = pos.blockIt();
							RichTextBlock front_part;
							front_part.font = it->font;
							front_part.string = it->string.substr(0, pos.charIdx());
							m_blockList.insert(it, front_part);
							++it;
							RichTextBlock back_part;
							back_part.font = it->font;
							back_part.string = it->string.substr(pos.charIdx());
							m_blockList.insert(it, front_part);
							*pos.blockIt() = textBlock;
							return Iterator(this, it, 0);
						}
						else
						{
							auto temp = pos.blockIt();
							if (temp != m_blockList.begin())
							{
								--temp;
								if (temp->font == textBlock.font && temp->format == textBlock.format)
								{
									temp->string += textBlock.string;
									return pos;
								}
							}
							m_blockList.insert(pos.blockIt(), textBlock);
							--temp;
							return Iterator(this, temp, 0);
						}
					}
				}
			}


			void insert(const Iterator& pos, const RichText& text)
			{
				auto temp = pos;
				for (auto& it : text.m_blockList)
					temp = insert(temp, it);
			}

			void erase(const Iterator& pos, uint32_t count)
			{
				auto it = pos.blockIt();
				auto left = count;
				auto offset = pos.charIdx();
				while (left > 0 && it != m_blockList.end())
				{
					if (offset != 0)
					{
						if (offset + left < it->string.size())
						{
							it->string = it->string.substr(0, offset) + it->string.substr(offset + left);
							left = 0;
							offset = 0;
						}
						else
						{
							left -= it->string.size() - offset;
							it->string.resize(offset);
							offset = 0;
						}
					}
					else
					{
						if (left < it->string.size())
						{
							it->string = it->string.substr(left);
							left = 0;
						}
						else
						{
							left -= it->string.size();
							auto temp = it;
							++it;
							m_blockList.erase(temp);
						}
					}
					++it;
				}
				it = m_blockList.begin();
				auto it_next = it;
				if (it_next != m_blockList.end())
				{
					++it_next;
					while (it_next != m_blockList.end())
					{
						if (it->font == it_next->font && it->format == it_next->format)
						{
							it_next->string = it->string + it_next->string;
							m_blockList.erase(it);
						}
						it = it_next;
						++it_next;
					}
				}
			}
			int32_t size() const {
				uint32_t size = 0;
				for (auto it : m_blockList)
					size += it.string.size();
				return size;
			}
			Iterator begin() { return Iterator(this, m_blockList.begin(), 0); }
			ConstIterator begin() const { return ConstIterator(this, m_blockList.begin(), 0); }
			Iterator end() { return Iterator(this, m_blockList.end(), 0); }
			ConstIterator end() const { return ConstIterator(this, m_blockList.end(), 0); }

			struct PaintInfo
			{
				struct LinePaintInfo
				{
					struct BlockPaintInfo
					{
						Iterator begin;
						Iterator end;
						Rect     rect;
						Point    basepoint;
						StringRef strRef;
						BlockPaintInfo() {}
						BlockPaintInfo(const Iterator& beg, const Iterator& ed, const Rect& r, const Point bp, const StringRef& ref) :
							begin(beg), end(ed), rect(r), basepoint(bp), strRef(ref)
						{

						}
					};
					LinePaintInfo() {};
					core::LinkedList<BlockPaintInfo> blocks;
				};
				core::LinkedList<LinePaintInfo> lines;
				int totalHeight;
				int totalWidth;
			};

			PaintInfo getPaintInfo(const Rect& area, const Alignment& alignment)
			{
				auto cur = begin();
				int x = 0;
				int y = 0;
				PaintInfo info;
				info.totalHeight = 0;
				PaintInfo::LinePaintInfo new_line;
				while (cur != end())
				{
					auto bIt = cur.blockIt();
					auto str_ref = StringRef(bIt->string.data() + cur.charIdx(), bIt->string.size() - cur.charIdx());
					auto width = GuiFontSystem::calcFontLineWidth(str_ref, bIt->font);
					if (width <= area.mWidth - x)
					{
						auto rect = Rect(x, y, width, GuiFontSystem::calcFontLineHeight(bIt->font));
						auto base_point = GuiFontSystem::calcFontBasePoint(str_ref, rect, bIt->font);
						++bIt;
						auto next_pos = Iterator(this, bIt, 0);
						new_line.blocks.push_back(PaintInfo::LinePaintInfo::BlockPaintInfo(cur, next_pos, rect, base_point, str_ref));
						x += width;
						cur = next_pos;
					}
					else
					{
						auto last_char_idx = GuiFontSystem::calcFontPointerIndex(str_ref, area.mWidth - x, bIt->font);
						auto sub_str_ref = str_ref.substr_ref(last_char_idx, 0);
						auto sub_str_width = GuiFontSystem::calcFontLineWidth(sub_str_ref, bIt->font);
						auto rect = Rect(x, y, sub_str_width, GuiFontSystem::calcFontLineHeight(bIt->font));
						auto base_point = GuiFontSystem::calcFontBasePoint(sub_str_ref, rect, bIt->font);
						auto next_pos = Iterator(this, bIt, cur.charIdx() + last_char_idx);
						new_line.blocks.push_back(PaintInfo::LinePaintInfo::BlockPaintInfo(cur, next_pos, rect, base_point, sub_str_ref));
						x = 0;
						cur = next_pos;
						int max_h = 0;
						for (auto& it : new_line.blocks)
							if (it.rect.mHeight > max_h)
							{
								max_h = it.rect.mHeight;
							}
						y += max_h;
						info.lines.push_back(new_line);
						new_line.blocks.clear();
					}
				}
				if (new_line.blocks.size() > 0)
				{
					int max_h = 0;
					for (auto& it : new_line.blocks)
						if (it.rect.mHeight > max_h)
						{
							max_h = it.rect.mHeight;
						}
					y += max_h;
					info.lines.push_back(new_line);
				}
				info.totalHeight = y;
				return info;
			}

		};



		struct Palette
		{
			enum FillMode
			{
				eFillMode_None = 0,
				eFillMode_Color = eFillMode_None + 1,
				eFillMode_Image = eFillMode_Color + 1,
			};

			enum FrameMode
			{
				eFrameMode_None = 0,
				eFrameMode_Surround = eFrameMode_None + 1,
				//eFrameMode_Inside = eFrameMode_Surround + 1,
				//eFrameMode_Outside = eFrameMode_Inside + 1,
				eFrameMode_Sunken = eFrameMode_Surround + 1,
				eFrameMode_Convex = eFrameMode_Sunken + 1,
			};

			FillMode    mFillMode;
			FrameMode   mFrameMode;
			HandleImage mBackgroundImage;
			Color       mBackgroundColor;
			Color       mBackgroundDisableColor;
			Color       mBackgroundHighLightColor;
			Color       mBackgroundFocusColor;
			Color       mForegroundColor;
			Color       mForegroundDisableColor;
			Color       mForegroundHighLightColor;
			Color       mForegroundFocusColor;
			Color       mFrameColor;

			Palette()
			{
				sizeof(Palette);
				mFillMode = eFillMode_None;
				mFrameMode = eFrameMode_None;
				mBackgroundImage = HandleImage(nullptr);
				mBackgroundColor = Color(245, 245, 245, 150);
				mBackgroundDisableColor = Color(200, 200, 200, 225); //
				mBackgroundHighLightColor = Color(150, 150, 150, 175);
				mBackgroundFocusColor = Color(100, 100, 100, 200);
				mForegroundColor = Color(0, 0, 0, 255);
				mForegroundHighLightColor = Color(0, 0, 0, 255);
				mForegroundFocusColor = Color(255, 255, 255, 255);
				mForegroundDisableColor = Color(100, 100, 100, 225); //
				mFrameColor = Color(120, 120, 120, 255);
			}

			void setBackgroundColor(const Color& color)
			{
				if (color.a != 0)
					mFillMode = eFillMode_Color;
				else
					mFillMode = eFillMode_None;
				mBackgroundColor = color;
			}

			void setBackgroundImage(HandleImage image)
			{
				mFillMode = eFillMode_Image;
				mBackgroundImage = image;
			}

			void setFrameColor(const Color& color)
			{
				if (color.a != 0 && mFrameMode == eFrameMode_None)
					mFrameMode = eFrameMode_Surround;
				else
					mFrameMode = eFrameMode_None;
				mFrameColor = color;
			}

		};




		class GuiColorStyle
		{
		public:
			enum WidgetType
			{
				eWidgetTypeBegin = 0,
				eWidget = eWidgetTypeBegin,
				eLabel,
				ePushButton,
				eTextInput,
				eSlider,
				eScorllBar,
				eProgressBar,
				eSliderBar,
				eSeekBar,
				eItemView,
				eMenu,
				eWidgetTypeEnd,	
				eWidgetTypeCount = eWidgetTypeEnd - eWidgetTypeBegin + 1,
			};
			enum ColorStyle
			{
				eLight,
				eDark,
				eRed,
			};
		private:
			static ColorStyle cur_color_style;
			static Color foreground_color;
			static Color background_color;
			static Palette color_style[(int)WidgetType::eWidgetTypeCount];
		public:
			static const GuiColorStyle::ColorStyle& getCurColorSytle() { return cur_color_style; }
			static const Palette& getSytle(WidgetType type) { return color_style[type]; }
			static const Color& getForegroundColor() { return foreground_color; }
			static const Color& getBackgroundColor() { return  background_color; }

			struct StyleData
			{
				Color colors[16];
				const Color& operator[](int index)const { return colors[index]; }
			};

			static void init(ColorStyle color_style = ColorStyle::eDark);
		private:
			static void _Init(const StyleData& style);
		};
		GuiColorStyle::ColorStyle GuiColorStyle::cur_color_style;
		Palette GuiColorStyle::color_style[WidgetType::eWidgetTypeCount];
		Color GuiColorStyle::foreground_color = Color(0, 0, 0, 255);
		Color GuiColorStyle::background_color = Color(255, 255, 255, 255);


		class AbstractCursor
		{
		private:
			Rect m_rect;
			float m_scalar = 1.0f;
		public:
			struct CursorPaintEvent
			{
				Painter* painter = nullptr;
				Point globalPos = Point(0, 0);
				float scalar = 1.0f;
			};

			enum SystemCursorType
			{
				eArrowCursor,  // 箭头（默认）
				eEditCursor,   // 文本编辑
				ePickUpCiursor,// 拾取
				eSizeVerCursor,// 竖直大小调整
				eSizeHorCursor,// 水平大小调整
				eSizeDiaCursor1, // 对角线大小调整1
				eSizeDiaCursor2, // 对角线大小调整2
			};

			virtual ~AbstractCursor() {}
			void  setScalar(float scalar) { m_scalar = scalar; }
			float getScalar()const { return m_scalar; }
			void  setRect(const Rect& rect) { m_rect = rect; }
			const Rect& getRect()const { return m_rect; }
			virtual void onPaint(const CursorPaintEvent& paintEvent) = 0;
		};


		enum class GuiIcon
		{

			eIcon_Mosaic,
			eIcon_TriangleLeft,
			eIcon_TriangleRight,
			eIcon_TriangleDown,
			eIcon_TriangleUp,
			eIcon_Cross,
			eIcon_Tick,
		};

		class GuiAssetsManager
		{
		public:
			static std::unordered_map<std::string, HandleImage> m_images;
			static std::unordered_map<std::string, AbstractCursor*>     m_cursors;
		public:
			static HandleImage loadImage(GuiIcon icon)
			{
				switch (icon)
				{
				case GuiIcon::eIcon_Mosaic:return loadImage("Mosaic"); break;
				case GuiIcon::eIcon_TriangleLeft:return loadImage("TriangleLeft"); break;
				case GuiIcon::eIcon_TriangleRight:return loadImage("TriangleRight"); break;
				case GuiIcon::eIcon_TriangleDown:return loadImage("TriangleDown"); break;
				case GuiIcon::eIcon_TriangleUp:return loadImage("TriangleUp"); break;
				case GuiIcon::eIcon_Cross:return loadImage("Cross"); break;
				case GuiIcon::eIcon_Tick:return loadImage("Tick"); break;
				default:return HandleImage();
				}
			}

			static HandleImage        loadImage(const std::string& name);
			static void               storeImage(const char* name, HandleImage image);
			static HandleCursor       loadCursor(const std::string& name);
			static void               storeCursor(const char* name, AbstractCursor* cursor);
			static void               clear();
		};
		std::unordered_map<std::string, HandleImage> GuiAssetsManager::m_images;
		std::unordered_map<std::string, AbstractCursor*> GuiAssetsManager::m_cursors;



		class AbstractGuiRenderSystem
		{
		public:

			AbstractGuiRenderSystem() {};
			virtual ~AbstractGuiRenderSystem() {};

			virtual HandleImage createImage(const void* data, uint32_t size, uint32_t width, uint32_t height) = 0;
			virtual void deleteImage(HandleImage image) = 0;
			virtual void updateImage(HandleImage image, const void* data, uint32_t size, uint32_t width, uint32_t height) = 0;
			virtual ImageInfo getImageInfo(HandleImage image) = 0;
			virtual AbstractGuiCanvas* createGuiCanvas(const Size& size) = 0;
			virtual void deleteGuiCanvas(AbstractGuiCanvas* canvas) = 0;
		};

		class GuiRenderSystem
		{
		private:
			static AbstractGuiRenderSystem* m_renderSystem;
			static bool m_isInitialized;
		public:
			static void initSystem(AbstractGuiRenderSystem* renderSystem);
			static void destroySystem();
			static bool isInitialized();

			static HandleImage        createImage(const void* data, uint32_t size, uint32_t width, uint32_t height);
			static HandleImage        createImage(const core::Bitmap& bitmap) { return createImage(bitmap.data(), bitmap.size(), bitmap.width(), bitmap.height()); };
			static HandleImage        createImage(const String& path) { return createImage(core::Bitmap(path.c_str())); };
			static HandleImage        createImage(const char* path) { return createImage(core::Bitmap(path)); };

			static void               deleteImage(HandleImage image);
			static void               updateImage(HandleImage image, const void* data, uint32_t size, uint32_t width, uint32_t height);
			static ImageInfo          getImageInfo(HandleImage image);
			static AbstractGuiCanvas* createGuiCanvas(const Size& size);
			static void               deleteGuiCanvas(AbstractGuiCanvas* canvas);
		};
		AbstractGuiRenderSystem* GuiRenderSystem::m_renderSystem = nullptr;
		bool GuiRenderSystem::m_isInitialized = false;



		struct PolygonInfo
		{
			using vec2 = math::vec2;
			struct VertexData
			{
				vec2  pos;
				vec2  tex;
				Color col;
			};
			VertexData* mixBuffer = nullptr;
			vec2*       posBuffer = nullptr;
			vec2*       texBuffer = nullptr;
			Color*      colBuffer = nullptr;
			uint32_t    vertexCount = 0;
			uint32_t*   indexList = nullptr;
			uint32_t    indexCount = 0;
			float       lineWidth = 1;
			bool        close = false;
			int         pointSize = 1;
			vec2        translate = vec2(0);
			vec2        scale = vec2(1);
			float       rotation = 0.0f;
			HandleImage image = HandleImage(nullptr);
		};

		class AbstractGuiRenderer
		{
		protected:
			using vec4 = math::vec4;
			using vec3 = math::vec3;
			using vec2 = math::vec2;
			using mat4 = math::mat4;
		public:
			struct Vertex
			{
				vec2  pos;
				vec2  tex;
				Color col;
			};
		protected:
			friend class Painter;
			float m_frame_duration;
			float m_frame_rate;

			struct DrawCommand
			{
				math::mat4  mTransform;
				Rect        mScissor;
				HandleImage mImage;
				uint32_t    mPipeline;
				uint32_t    mVertexOffset;
				uint32_t    mVertexCount;
				uint32_t    mIndexOffset;
				uint32_t    mIndexCount;
			};

			struct DrawList
			{
				std::vector<Vertex>     mVerticesData;
				std::vector<uint32_t>   mIndicesData;
				std::vector<DrawCommand> mDrawCommand;
			};

		protected:
			DrawList m_drawList;
			HandleImage m_defaultImage;

			struct RenderData
			{
				vec2 mCircleVertices[13];
			}m_renderData;

		public:
			AbstractGuiRenderer()
			{
				uint32_t whiteImageData[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
				m_defaultImage = GuiRenderSystem::createImage(whiteImageData, 2 * 2 * 4, 2, 2);
				for (int i = 0; i < 13; i++)
					m_renderData.mCircleVertices[i] = vec2(math::cos(math::radians(float(i) * 30.0f)), math::sin(math::radians(float(i) * 30.0f)));
			}
			virtual ~AbstractGuiRenderer() 
			{
				GuiRenderSystem::deleteImage(m_defaultImage);
			};

			void  setFrameDuration(float duration) { m_frame_duration = duration; }
			float getFramDuration() { return m_frame_duration; }
			void  setFrameRate(float rate) { m_frame_rate = rate; }
			float getFramRate() { return m_frame_rate; }

			virtual void clear() {}
			virtual void begin(void* userData) {}
			virtual void end(void* userData) {}
		protected:

			void checkState(const HandleImage& image, const Rect& scissor, int pipeline)
			{
				assert(image != nullptr);
				if (m_drawList.mDrawCommand.size() <= 0)
				{
					newDrawCommand(mat4(1.0f));
					m_drawList.mDrawCommand.back().mImage = image;
					m_drawList.mDrawCommand.back().mScissor = scissor;
					m_drawList.mDrawCommand.back().mPipeline = pipeline;
				}
				else if (m_drawList.mDrawCommand.back().mIndexCount <= 0)
				{
					m_drawList.mDrawCommand.back().mImage = image;
					m_drawList.mDrawCommand.back().mScissor = scissor;
					m_drawList.mDrawCommand.back().mPipeline = pipeline;
				}
				else if (m_drawList.mDrawCommand.back().mImage != image ||
					!m_drawList.mDrawCommand.back().mScissor.equals(scissor) ||
					m_drawList.mDrawCommand.back().mPipeline != pipeline)
				{
					newDrawCommand(mat4(1.0f));
					m_drawList.mDrawCommand.back().mImage = image;
					m_drawList.mDrawCommand.back().mScissor = scissor;
					m_drawList.mDrawCommand.back().mPipeline = pipeline;
				}		
			}
			void newDrawCommand(const mat4& transform)
			{
				if (m_drawList.mDrawCommand.size() <= 0 || m_drawList.mDrawCommand.back().mIndexCount > 0)
				{
					m_drawList.mDrawCommand.push_back(DrawCommand());
					m_drawList.mDrawCommand.back().mVertexOffset = m_drawList.mVerticesData.size();
					m_drawList.mDrawCommand.back().mVertexCount = 0;
					m_drawList.mDrawCommand.back().mIndexOffset = m_drawList.mIndicesData.size();
					m_drawList.mDrawCommand.back().mIndexCount = 0;
				}
				m_drawList.mDrawCommand.back().mTransform = transform;
			}
			void pushData(Vertex* vertexData, int vertexCount, uint32_t* indexData, int indexCount)
			{
				auto index_offset = m_drawList.mDrawCommand.back().mVertexCount;
				m_drawList.mDrawCommand.back().mVertexCount += vertexCount;
				m_drawList.mDrawCommand.back().mIndexCount += indexCount;
				for (int i = 0; i < vertexCount; i++)
					m_drawList.mVerticesData.push_back(vertexData[i]);
				for (int i = 0; i < indexCount; i++)
					m_drawList.mIndicesData.push_back(indexData[i] + index_offset);
			}
			struct DrawDataBuffer
			{
				Vertex* mVertexBuffer;
				uint32_t* mIndexBuffer;
				uint32_t mIndexCount;
				uint32_t mIndexOffset;
			};
			DrawDataBuffer applyData(int vertexCount, int indexCount)
			{
				DrawDataBuffer data_buffer;
				auto vertex_count = m_drawList.mVerticesData.size();
				auto index_count = m_drawList.mIndicesData.size();
				auto index_offset = m_drawList.mDrawCommand.back().mVertexCount;
				m_drawList.mDrawCommand.back().mVertexCount += vertexCount;
				m_drawList.mDrawCommand.back().mIndexCount += indexCount;
				for (int i = 0; i < vertexCount; i++)
					m_drawList.mVerticesData.push_back(Vertex());
				for (int i = 0; i < indexCount; i++)
					m_drawList.mIndicesData.push_back(index_offset);
				data_buffer.mVertexBuffer = &m_drawList.mVerticesData[vertex_count];
				data_buffer.mIndexBuffer = &m_drawList.mIndicesData[index_count];
				data_buffer.mIndexCount = indexCount;
				data_buffer.mIndexOffset = index_offset;
				return data_buffer;
			}
			void submitData(const DrawDataBuffer& buffer)
			{
				for (int i = 0; i < buffer.mIndexCount; i++)
					buffer.mIndexBuffer[i] += buffer.mIndexOffset;
			}

		};



		class Painter
		{
		private:
			friend class Widget;
			using vec2 = math::vec2;
			using mat4 = math::mat4;
			using vec4 = math::vec4;
			using vec3 = math::vec3;
			using Vertex = AbstractGuiRenderer::Vertex;
			Rect         m_limitRect;
			Rect         m_scissorRect;
			Rect         m_thisRect;
			Point        m_basepoint;
			int          m_baseAlpha;
			int          m_currentAlpha;
			AbstractGuiRenderer* m_renderer;
			Painter(AbstractGuiRenderer* renderer, const Point& basepoint, const Rect& limit, const Size& size, int alpha)
				: m_renderer(renderer), m_basepoint(basepoint), m_limitRect(limit), m_scissorRect(limit), m_thisRect(basepoint, size)
			{
				m_baseAlpha = math::clamp(alpha, 0, 255);
				m_currentAlpha = m_baseAlpha;
			}

			void checkState(const HandleImage& image, const Rect& scissor, int pipeline)
			{
				m_renderer->checkState(image, scissor, pipeline);
			}
			void newDrawCommand(const mat4& transform)
			{
				m_renderer->newDrawCommand(transform);
			}
			void pushData(Vertex* vertexData, int vertexCount, uint32_t* indexData, int indexCount)
			{
				m_renderer->pushData(vertexData, vertexCount, indexData, indexCount);
			}
			AbstractGuiRenderer::DrawDataBuffer applyData(int vertexCount, int indexCount)
			{
				return m_renderer->applyData(vertexCount, indexCount);
			}
			void submitData(const AbstractGuiRenderer::DrawDataBuffer& buffer)
			{
				m_renderer->submitData(buffer);
			}
			const HandleImage& getDefaultImage() const
			{
				return m_renderer->m_defaultImage;
			}
		public:
			Painter() {}

			float getFramDuration() { return m_renderer->getFramDuration(); }
			float getFramRate() { return m_renderer->getFramRate(); }

			const Point& getGlobalBasepoint() const { return m_basepoint; }
			const Rect&  getGlobalLimit()     const { return m_limitRect; }
			const Rect&  getGlobalRect()      const { return m_thisRect; }
			const Rect&  getGlobalScissor()     const { return m_scissorRect; }
			const int& getAlpha() const { return m_currentAlpha; }
			const int& getBaseAlpha() const { return m_baseAlpha; }

			void setScissor()
			{
				m_scissorRect = getGlobalLimit();
			}
			void setScissor(const Rect& scissor)
			{
				Rect globalScissorRect = Rect(getGlobalBasepoint() + scissor.mOffset, scissor.mSize);
				m_scissorRect = getGlobalLimit().disjunction(globalScissorRect);
			}
			void setAlpha()
			{
				m_currentAlpha = math::clamp(getBaseAlpha(), 0, 255);
			}
			void setAlpha(int alpha)
			{
				m_currentAlpha = math::clamp(getBaseAlpha() * alpha / 255, 0, 255);
			}

			// line
			void drawLine(const Point& p1, const Point& p2, const Color& color);
			void drawLine(const PointF& p1, const PointF& p2, const Color& color);
			void drawLine(const Point& p1, const Point& p2, const Color& color1, const Color color2);
			void drawLine(const PointF& p1, const PointF& p2, const Color& color1, const Color color2);
			void drawLineNoCheck(const Point& p1, const Point& p2, const Color& color1, const Color color2);
			void drawLineNoCheck(const PointF& p1, const PointF& p2, const Color& color1, const Color color2);
			// rect
			void drawRect(const Rect& rect, const Color& color);
			void drawRect(const Rect& rect, const HandleImage& image);
			void drawRectFrame(const Rect& rect, const Color& color);
			// Signed Distance Field Image
			void drawSDF(const Rect& rect, const Color& color, const HandleImage image);
			// text C-Style-String
			void drawTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color& color);
			void drawColorTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color* colorList);
			void drawPaletteTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color* colorList, const uint16_t* colorIdxList);
			void drawRichTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const RichTextFormat& format);
			// text - String
			void drawTextLine(const String& str, const Point& point, Font& font, const Color& color) { return drawTextLine(str.c_str(), str.size(), point, font, color); }
			void drawColorTextLine(const String& str, const Point& point, Font& font, const Color* colorList) { return drawColorTextLine(str.c_str(), str.size(), point, font, colorList); }
			void drawPaletteTextLine(const String& str, const Point& point, Font& font, const Color* colorList, const uint16_t* colorIdxList) { return drawPaletteTextLine(str.c_str(), str.size(), point, font, colorList, colorIdxList); }
			void drawRichTextLine(const String& str, const Point& point, Font& font, const RichTextFormat& format) { return drawRichTextLine(str.c_str(), str.size(), point, font, format); }
			// triangle
			void drawTriangle(const Point& p1, const Point& p2, const Point& p3, const Color& color);
			void drawTriangle(const PointF& p1, const PointF& p2, const PointF& p3, const Color& color);
			void drawTriangleFrame(const Point& p1, const Point& p2, const Point& p3, const Color& color);
			void drawTriangleFrame(const PointF& p1, const PointF& p2, const PointF& p3, const Color& color);
			// circle
			void drawCircle(const Point& center, float radius, float delta, const Color& color);
			void drawCircle(const PointF& center, float radius, float delta, const Color& color);
			void drawCircleFrame(const Point& center, float radius, float delta, const Color& color);
			void drawCircleFrame(const PointF& center, float radius, float delta, const Color& color);
			// ellipse
			void drawEllipse(const Point& center, float rx, float ry, float delta, const Color& color);
			void drawEllipse(const PointF& center, float rx, float ry, float delta, const Color& color);
			void drawEllipseFrame(const Point& center, float rx, float ry, float delta, const Color& color);
			void drawEllipseFrame(const PointF& center, float rx, float ry, float delta, const Color& color);
			// round rect
			void drawRoundRect(const Rect& rect, float radius, float delta, const Color& color);
			void drawRoundRect(const Rect& rect, float radius, float delta, const HandleImage& image);
			void drawRoundRectFrame(const Rect& rect, float radius, float delta, const Color& color);
			// polyline
			void drawPolyline(const Point* vertices, uint32_t count, bool close, const Color& color);
			void drawPolyline(const PointF* vertices, uint32_t count, bool close, const Color& color);
			void drawPolyline(const Point* vertices, uint32_t count, bool close, const Color* colors);
			void drawPolyline(const PointF* vertices, uint32_t count, bool close, const Color* colors);
			// polygon
			void drawPolygon(const Point* vertices, uint32_t count, const Color& color);
			void drawPolygon(const PointF* vertices, uint32_t count, const Color& color);
			void drawPolygon(const Point* vertices, uint32_t count, const Color* colors);
			void drawPolygon(const PointF* vertices, uint32_t count, const Color* colors);
			// wide line
			void drawWideLine(const Point& p1, const Point& p2, float width, const Color& color1, const Color color2);
			void drawWideLine(const PointF& p1, const PointF& p2, float width, const Color& color1, const Color color2);
			// wide polyline
			void drawWidePolyline(const Point* vertices, uint32_t count, float width, bool close, const Color& color);
			void drawWidePolyline(const PointF* vertices, uint32_t count, float width, bool close, const Color& color);
			void drawWidePolyline(const Point* vertices, uint32_t count, float width, bool close, const Color* colors);
			void drawWidePolyline(const PointF* vertices, uint32_t count, float width, bool close, const Color* colors);
			// wide round polyline
			void drawWideRoundPolyline(const Point* vertices, uint32_t count, float width, bool close, const Color& color);
			void drawWideRoundPolyline(const PointF* vertices, uint32_t count, float width, bool close, const Color& color);
			// polygon ext
			void drawPolygonEx(const PolygonInfo& info, const Color& color);
			void drawPolygonEx(const PolygonInfo& info);
			// polyline ext
			void drawPolylineEx(const PolygonInfo& info, const Color& color);
			void drawPolylineEx(const PolygonInfo& info);


		};



		class AbstractGuiCanvas
		{
		public:
			virtual Size         getCanvasSize() = 0;
			virtual void         setCanvasSize(const Size& size) = 0;
			virtual void         setCurrentFramebufferIndex(int32_t idx) = 0;
			virtual uint32_t     getCurrentFramebufferIndex() const = 0;
			virtual HandleImage  getFramebufferImage(uint32_t attachmentIdx = 0, uint32_t framebufferIdx = 0) = 0;
			virtual uint32_t     getFramebufferCount() const = 0;
			virtual uint32_t     getAttachmentCount() const = 0;
			virtual AbstractGuiRenderer* getGuiRenderer() = 0;
		};



		class CursorSDF : public AbstractCursor
		{
		private:
			using vec2 = math::vec2;
			HandleImage m_sdf = HandleImage(nullptr);
		public:
			CursorSDF(HandleImage image) {
				m_sdf = image; 
			}
			virtual ~CursorSDF() { GuiRenderSystem::deleteImage(m_sdf); }
			virtual void onPaint(const CursorPaintEvent& paintEvent)override
			{
				Rect rect = getRect();
				auto scalar = math::clamp(paintEvent.scalar, 0.001f, 16.0f);
				rect.mOffset = Offset(vec2(rect.mOffset) * scalar);
				rect.mSize = Size(vec2(rect.mSize) * scalar);
				rect.mOffset += paintEvent.globalPos;
				Rect shadow_rect = rect;
				shadow_rect.mOffset += Offset(1, 1);
				paintEvent.painter->drawSDF(shadow_rect, GuiColorStyle::getBackgroundColor(), m_sdf);
				paintEvent.painter->drawSDF(rect, GuiColorStyle::getForegroundColor(), m_sdf);
			}
		};



#define _CRAFT_ENGINE_GUI_SIGNAL_EMIT_FUNCTION_PREFIX(x)       _Signal_Emit_Function_##x
#define _CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(x)    _Signal_Connect_Function_##x
#define _CRAFT_ENGINE_GUI_SIGNAL_DISCONNECT_FUNCTION_PREFIX(x) _Signal_Disconnect_Function_##x
#define _CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(x)              _Signal_Define_##x
#define _CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(x)                _Signal_Lock_##x
		/*
		 定义一个信号
		 signalname：信号名称
		 functype：  回调函数类型
		*/
#define craft_engine_gui_signal(signalname, functype)\
		CraftEngine::core::Observer<functype> _CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname);\
		bool _CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) = false;\
		template<typename... Args>\
		void _CRAFT_ENGINE_GUI_SIGNAL_EMIT_FUNCTION_PREFIX(signalname)(Args...args)\
		{\
			if(_CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) == false) {\
				_CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) = true;\
				_CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname).notify(args...);\
				_CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) = false;\
			}\
		}\
		uint32_t _CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(signalname)(const CraftEngine::core::Callback<functype>& func)\
		{\
			return _CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname).connect(func);\
		}\
		void _CRAFT_ENGINE_GUI_SIGNAL_DISCONNECT_FUNCTION_PREFIX(signalname)(uint32_t key)\
		{\
			_CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname).disconnect(key);\
		}


#define craft_engine_gui_signal_v2(signalname, functype)\
		CraftEngine::core::Callback<functype> _CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname);\
		bool _CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) = false;\
		template<typename... Args>\
		void _CRAFT_ENGINE_GUI_SIGNAL_EMIT_FUNCTION_PREFIX(signalname)(Args...args)\
		{\
			if(_CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) == false) {\
				_CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) = true;\
				_CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname).call(std::forward(args)...);\
				_CRAFT_ENGINE_GUI_SIGNAL_LOCK_PREFIX(signalname) = false;\
			}\
		}\
		uint32_t _CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(signalname)(const CraftEngine::core::Callback<functype>& func)\
		{\
			_CRAFT_ENGINE_GUI_SIGNAL_DEFINE_PREFIX(signalname) = func;\
			return 0;\
		}\
		void _CRAFT_ENGINE_GUI_SIGNAL_DISCONNECT_FUNCTION_PREFIX(signalname)(uint32_t key)\
		{\
			\
		}


		/*
		 导出一个信号
		 psrcsender：     拥有源信号的对象
		 srcsignalname：  源信号名称
		 dstsignalname：  导出信号名称
		 functype：       回调函数类型
		*/
#define craft_engine_gui_signal_export(psrcsender, srcsignalname, dstsignalname, functype)\
		template<typename... Args>\
		void _CRAFT_ENGINE_GUI_SIGNAL_EMIT_FUNCTION_PREFIX(dstsignalname)(Args...args)\
		{\
			psrcsender->_CRAFT_ENGINE_GUI_SIGNAL_EMIT_FUNCTION_PREFIX(srcsignalname)(args...);\
		}\
		uint32_t _CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(dstsignalname)(const CraftEngine::core::Callback<functype>& func)\
		{\
			return psrcsender->_CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(srcsignalname)(func);\
		}\
		void _CRAFT_ENGINE_GUI_SIGNAL_DISCONNECT_FUNCTION_PREFIX(dstsignalname)(uint32_t key)\
		{\
			psrcsender->_CRAFT_ENGINE_GUI_SIGNAL_DISCONNECT_FUNCTION_PREFIX(srcsignalname)(key);\
		}

		/*
		 连接信号与信号接收函数，返回一个序号
		 psender：     信号发出者
		 signalname：  信号名称
		 paccepter：   信号接收者
		 funcname：    信号接收者成员函数
		*/
#define craft_engine_gui_connect(psender, signalname, paccepter, funcname)\
		(psender)->_CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(signalname)({paccepter, &std::decay_t<decltype(*paccepter)>::funcname})

		/*
		 连接信号与信号接收函数
		 psender：     信号发出者
		 signalname：  信号名称
		 funcname：    信号接收函数(static)
		*/
#define craft_engine_gui_connect_v2(psender, signalname, funcname)\
		(psender)->_CRAFT_ENGINE_GUI_SIGNAL_CONNECT_FUNCTION_PREFIX(signalname)({funcname})

		/*
		 连接信号与信号接收函数
		 psender：     信号发出者
		 signalname：  信号名称
		 key:          序号
		*/
#define craft_engine_gui_disconnect(psender, signalname, key)\
		(psender)->_CRAFT_ENGINE_GUI_SIGNAL_DISCONNECT_FUNCTION_PREFIX(signalname)(key)

		/*
		 发射一个信号
		 signalname：信号名称
		 ...：回调函数参数
		*/
#define craft_engine_gui_emit(signalname, ...)\
		_CRAFT_ENGINE_GUI_SIGNAL_EMIT_FUNCTION_PREFIX(signalname)(__VA_ARGS__)


		class Widget;

		class AbstractCustomLayout
		{
		public:
			virtual void arrange(Widget* comp) = 0;
		};


		struct Layout
		{
		public:
			enum LayoutMode
			{
				eCustomLayout,
				eFlowLayout,
				eRowLayout,
				eColLayout,
				eGridLayout,
			};
			struct FlowLayoutInfo
			{
				int baseOnRow = 1;
				int alignBorderMin = 0;
			};
			struct RowLayoutInfo
			{
				int alignBorderMin = 1;
				int allowFlow = 0;
			};
			struct ColLayoutInfo
			{
				int alignBorderMin = 1;
				int allowFlow = 0;
			};
			struct GridLayoutInfo
			{
				int rowCount = 0;
				int colCount = 0;
			};
			union LayoutInfo
			{
				FlowLayoutInfo flowInfo;
				RowLayoutInfo  rowInfo;
				ColLayoutInfo  colInfo;
				GridLayoutInfo gridInfo;
				LayoutInfo() {};
			}info;
			LayoutMode mode = eCustomLayout;

			Layout()
			{
				
			}

			Layout(LayoutMode mode)
			{
				setMode(mode);
			}

			void clear() { mode = eCustomLayout; }
			/*
			* @param rowMajor: row major
			* @param alignToBorderMin: align element to (1)min border (0)center (-1)max border
			*/
			void setFlow(bool rowMajor = true, int alignToBorderMin = 0)
			{
				mode = eFlowLayout;
				info.flowInfo.baseOnRow = rowMajor ? 1 : 0;
				info.flowInfo.alignBorderMin = alignToBorderMin;
			}

			/*
			* @param allowFlow: (false)no interval between two elements, (true)having interval between two elements,
			* @param alignToBorderMin: align element to (1)min border, (0)center, (-1)max border
			*/
			void setRow(bool allowFlow = false, int alignToBorderMin = 0)
			{
				mode = eRowLayout;
				info.rowInfo.allowFlow = allowFlow ? 1 : 0;
				info.rowInfo.alignBorderMin = alignToBorderMin;
			}

			/*
			* @param allowFlow: (false)no interval between two elements, (true)having interval between two elements,
			* @param alignToBorderMin: align element to (1)min border, (0)center, (-1)max border
			*/
			void setCol(bool allowFlow = false, int alignToBorderMin = 0)
			{
				mode = eColLayout;
				info.colInfo.allowFlow = allowFlow ? 1 : 0;
				info.colInfo.alignBorderMin = alignToBorderMin;
			}

			/*
			* @param rowCount: rowCount
			* @param colCount: colCount
			*/
			void setGird(int rowCount = false, int colCount = 0)
			{
				mode = eGridLayout;
				info.gridInfo.rowCount = rowCount;
				info.gridInfo.colCount = colCount;
			}


			void setMode(LayoutMode m)
			{
				mode = m;
				switch (m)
				{
				case CraftEngine::gui::Layout::eCustomLayout:
					break;
				case CraftEngine::gui::Layout::eFlowLayout:
					info.flowInfo = FlowLayoutInfo();
					break;
				case CraftEngine::gui::Layout::eRowLayout:
					info.rowInfo = RowLayoutInfo();
					break;
				case CraftEngine::gui::Layout::eColLayout:
					info.colInfo = ColLayoutInfo();
					break;
				case CraftEngine::gui::Layout::eGridLayout:
					info.gridInfo = GridLayoutInfo();
					break;
				default:
					break;
				}
			}

			core::ArrayList<Rect> calcLayout(const core::ArrayList<Size>& items, const Size& area);
		};



		class LayoutTree
		{
		public:

			class Block;

			enum SplitType
			{
				eSplitType_Fill,
				eSplitType_LeftFixed,
				eSplitType_LeftRatio,
				eSplitType_RightFixed,
				eSplitType_RightRatio,
				eSplitType_TopFixed,
				eSplitType_TopRatio,
				eSplitType_BottomFixed,
				eSplitType_BottomRatio,
			};

		private:
			class Node
			{
			private:
				friend class Block;

				core::ArrayList<Node*> m_nodes;
				Node* m_parent;
				Rect                   m_rect;
				SplitType              m_type;
				float                  m_value;
			public:

				Node()
				{
					m_parent = nullptr;
					m_type = eSplitType_Fill;
					m_value = 0.0f;
				}

				~Node()
				{
					_Clear();
				}

				void _Copy(Node* other)
				{
					_Clear();
					m_nodes.resize(other->m_nodes.size());
					m_rect = other->m_rect;
					m_parent = other->m_parent;
					m_type = other->m_type;
					m_value = other->m_value;
					for (int i = 0; i < m_nodes.size(); i++)
					{
						m_nodes[i] = new Node();
						m_nodes[i]->_Copy(other->m_nodes[i]);
					}
				}

				int _ChildCount() const { return m_nodes.size(); }

				void _Split(SplitType type, float value)
				{
					if (m_nodes.size() == 0)
					{
						m_nodes.resize(2);
						m_nodes[0] = new Node();
						m_nodes[0]->m_type = type;
						m_nodes[0]->m_value = value;
						m_nodes[0]->m_parent = this;
						m_nodes[1] = new Node();
						m_nodes[1]->m_type = SplitType::eSplitType_Fill;
						m_nodes[1]->m_value = 0.0f;
						m_nodes[1]->m_parent = this;
					}
					else
					{
						m_nodes.resize(m_nodes.size() + 1);
						m_nodes[m_nodes.size() - 1] = m_nodes[m_nodes.size() - 2];
						m_nodes[m_nodes.size() - 2] = new Node();
						m_nodes[m_nodes.size() - 2]->m_type = type;
						m_nodes[m_nodes.size() - 2]->m_value = value;
						m_nodes[m_nodes.size() - 2]->m_parent = this;
					}
				}

				Node* _GetParent()
				{
					return m_parent;
				}

				Node* _GetChild(int index)
				{
					return m_nodes[index];
				}

				void _RemoveChild(int index)
				{
					auto temp = m_nodes[index];
					m_nodes.erase(index);
					delete temp;
				}

				void _Clear()
				{
					for (auto it : m_nodes) {
						it->_Clear();
						delete it;
					}m_nodes.resize(0);
				}

				void _Calc(const Rect& area)
				{
					m_rect = area;
					int val = 0;

					Rect cur_rect = area;

					for (auto& it : m_nodes)
					{
						switch (it->m_type)
						{
						case SplitType::eSplitType_LeftFixed:
						case SplitType::eSplitType_RightFixed:
						case SplitType::eSplitType_TopFixed:
						case SplitType::eSplitType_BottomFixed:
							val = it->m_value;
							break;
						case SplitType::eSplitType_LeftRatio:
						case SplitType::eSplitType_RightRatio:
							val = m_rect.mWidth * it->m_value;
							break;
						case SplitType::eSplitType_TopRatio:
						case SplitType::eSplitType_BottomRatio:
							val = m_rect.mHeight * it->m_value;
							break;
						default:
							break;
						}

						auto node = it;
						switch (it->m_type)
						{
						case SplitType::eSplitType_Fill:
							node->_Calc(cur_rect);
							break;
						case SplitType::eSplitType_LeftFixed:
						case SplitType::eSplitType_LeftRatio:
							node->_Calc(Rect(cur_rect.mX, cur_rect.mY, val, cur_rect.mHeight));
							cur_rect = Rect(cur_rect.mX + val, cur_rect.mY, cur_rect.mWidth - val, cur_rect.mHeight);
							break;
						case SplitType::eSplitType_RightFixed:
						case SplitType::eSplitType_RightRatio:
							node->_Calc(Rect(cur_rect.mX + cur_rect.mWidth - val, cur_rect.mY, val, cur_rect.mHeight));
							cur_rect = Rect(cur_rect.mX, cur_rect.mY, cur_rect.mWidth - val, cur_rect.mHeight);
							break;
						case SplitType::eSplitType_TopFixed:
						case SplitType::eSplitType_TopRatio:
							node->_Calc(Rect(cur_rect.mX, cur_rect.mY, cur_rect.mWidth, val));
							cur_rect = Rect(cur_rect.mX, cur_rect.mY + val, cur_rect.mWidth, cur_rect.mHeight - val);
							break;
						case SplitType::eSplitType_BottomFixed:
						case SplitType::eSplitType_BottomRatio:
							node->_Calc(Rect(cur_rect.mX, cur_rect.mY + cur_rect.mHeight - val, cur_rect.mWidth, val));
							cur_rect = Rect(cur_rect.mX, cur_rect.mY, cur_rect.mWidth, cur_rect.mHeight - val);
							break;
						default:
							break;
						}
					}
				}
			};

		public:
			class Block
			{
			private:
				friend class LayoutTree;
				Node* m_ptr;
				Block(Node* a) :m_ptr(a) {}
			public:
				Block() :m_ptr(nullptr) {}
				void  reset() { m_ptr->_Clear(); }
				Block parent() const { return Block(m_ptr->_GetParent()); }
				Block child(int index) const { return Block(m_ptr->_GetChild(index)); }
				int   childCount() const { return m_ptr->_ChildCount(); }

				void  removeChild(int index) { m_ptr->_RemoveChild(index); }
				void  removeThis() {
					for (int i = 0; i < parent().childCount(); i++) {
						if (parent().m_ptr->m_nodes[i] == m_ptr) {
							parent().removeChild(i);
							m_ptr = nullptr;
							return;
						}
					}
				}

				Block last() { return child(m_ptr->_ChildCount() - 1); }
				Block split(SplitType type, float value) { m_ptr->_Split(type, value); return child(m_ptr->_ChildCount() - 2); }

				Block splitTopFixed(float value) { return split(eSplitType_TopFixed, value); }
				Block splitTopRatio(float value) { return split(eSplitType_TopRatio, value); }
				Block splitBottomFixed(float value) { return split(eSplitType_BottomFixed, value); }
				Block splitBottomRatio(float value) { return split(eSplitType_BottomRatio, value); }
				Block splitLeftFixed(float value) { return split(eSplitType_LeftFixed, value); }
				Block splitLeftRatio(float value) { return split(eSplitType_LeftRatio, value); }
				Block splitRightFixed(float value) { return split(eSplitType_RightFixed, value); }
				Block splitRightRatio(float value) { return split(eSplitType_RightRatio, value); }

				bool        isValid() const { return m_ptr != nullptr; }
				bool        isLeaf() const { return childCount() == 0; }
				bool        isRoot() const { return !parent().isValid(); }
				SplitType   getType() const { return m_ptr->m_type; }
				float       getValue() const { return m_ptr->m_value; }
				const Rect& getRect() const { return m_ptr->m_rect; }
				//void split(SplitType type, float value) { m_ptr->_Split(type, value); }
			};

		public:
			LayoutTree()
			{
				m_root = new Node;
			}
			LayoutTree(const LayoutTree& other)
			{
				m_root = new Node;
				m_root->_Copy(other.m_root);
			}
			~LayoutTree()
			{
				delete m_root;
			}
			Block block() { return Block(m_root); }
			void  calc(const Rect& area) { m_root->_Calc(area); }

		private:
			Node* m_root;
		};





		class Action
		{
		private:
			String m_text;
			HandleImage m_icon;

		public:
			craft_engine_gui_signal(triggered, void(void));
			craft_engine_gui_signal(checked, void(bool));
			Action(): m_icon(nullptr) {}
			Action(const String& text, HandleImage icon, std::function<void(void)> trigger) : m_text(text), m_icon(icon) { craft_engine_gui_connect_v2(this, triggered, trigger); }
			Action(const String& text, HandleImage icon, std::function<void(bool)> check) : m_text(text), m_icon(icon) { craft_engine_gui_connect_v2(this, checked, check); }
			Action(const String& text, HandleImage icon) : m_text(text), m_icon(icon) {}
			Action(const String& text) : m_text(text), m_icon(nullptr) {}
			Action(const String& text, std::function<void(void)> trigger) : m_text(text), m_icon(nullptr) { craft_engine_gui_connect_v2(this, triggered, trigger); }
			Action(const String& text, std::function<void(bool)> check) : m_text(text), m_icon(nullptr) { craft_engine_gui_connect_v2(this, checked, check); }

			void setIcon(HandleImage icon) { m_icon = icon; }
			HandleImage getIcon() const { return m_icon; }
			void setText(const String& text) { m_text = text; }
			const String& getText() const { return m_text; }
			void trigger() { craft_engine_gui_emit(triggered, ); }
			void check(bool c) { craft_engine_gui_emit(checked, c); }
		};

		class Translator
		{
		private:
			typedef std::unordered_map<String, String> LanguagePackage;
			LanguagePackage m_langPack;
		public:
			void loadLangPackFromFile(const char* path)
			{
				m_langPack.clear();
				auto lines = core::readLines(path);
				for (auto i = 0; i < lines.size() / 2; i++)
					m_langPack.emplace(std::make_pair(StringTool::fromUtf8(lines[i * 2 + 0]), StringTool::fromUtf8(lines[i * 2 + 1])));
			}
			void loadLangPackFromData(const char* data)
			{
				m_langPack.clear();
				auto lines = core::splitLines(data);
				for (auto i = 0; i < lines.size() / 2; i++)
					m_langPack.emplace(std::make_pair(StringTool::fromUtf8(lines[i * 2 + 0]), StringTool::fromUtf8(lines[i * 2 + 1])));
			}
			void loadLangPackFromData(const Char* data)
			{
				m_langPack.clear();
				auto lines = core::splitLines(data);
				for (auto i = 0; i < lines.size() / 2; i++)
					m_langPack.emplace(std::make_pair(lines[i * 2 + 0], lines[i * 2 + 1]));
			}
			const String& translate(const String& str) const
			{
				auto it = m_langPack.find(str);
				if (it != m_langPack.end())
					return it->second;
				else
					return str;
			}
			const String& operator[](const String& str) const
			{
				return translate(str);
			}
		};



	}
}








namespace CraftEngine
{
	namespace gui
	{
		CRAFT_ENGINE_GUI_API void Painter::drawLine(const Point& p1, const Point& p2, const Color& color)
		{
			drawLine(p1, p2, color, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawLine(const PointF& p1, const PointF& p2, const Color& color)
		{
			drawLine(p1, p2, color, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawLine(const Point& p1, const Point& p2, const Color& color1, const Color color2)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			drawLineNoCheck(p1, p2, color1, color2);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawLine(const PointF& p1, const PointF& p2, const Color& color1, const Color color2)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			drawLineNoCheck(p1, p2, color1, color2);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawLineNoCheck(const Point& p1, const Point& p2, const Color& color1, const Color color2)
		{
			drawLineNoCheck(PointF(p1), PointF(p2), color1, color2);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawLineNoCheck(const PointF& p1, const PointF& p2, const Color& color1, const Color color2)
		{
			auto& offset = getGlobalBasepoint();
			vec2 points[2] = { p1 + vec2(offset) + 0.5f, p2 + vec2(offset) + 0.5f };
			vec2 dxdy = math::normalize(vec2(p2 - p1)) * 0.5f;
			Vertex vertices[4];
			uint32_t indices[6];
			vertices[0].pos = vec2(points[0].x - dxdy.y, points[0].y + dxdy.x);
			vertices[1].pos = vec2(points[1].x - dxdy.y, points[1].y + dxdy.x);
			vertices[2].pos = vec2(points[1].x + dxdy.y, points[1].y - dxdy.x);
			vertices[3].pos = vec2(points[0].x + dxdy.y, points[0].y - dxdy.x);
			vertices[0].tex = vec2(0.5f);
			vertices[1].tex = vec2(0.5f);
			vertices[2].tex = vec2(0.5f);
			vertices[3].tex = vec2(0.5f);
			vertices[0].col = Color(color1.rgb, color1.a * getAlpha() / 255);
			vertices[1].col = Color(color2.rgb, color2.a * getAlpha() / 255);
			vertices[2].col = Color(color2.rgb, color2.a * getAlpha() / 255);
			vertices[3].col = Color(color1.rgb, color1.a * getAlpha() / 255);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			pushData(vertices, 4, indices, 6);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRect(const Rect& rect, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			Vertex vertices[4];
			uint32_t indices[6];
			vertices[0].pos = vec2(offset) + vec2(rect.mOffset);
			vertices[1].pos = vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y);
			vertices[2].pos = vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y + rect.mSize.y);
			vertices[3].pos = vec2(offset) + vec2(rect.mOffset.x, rect.mOffset.y + rect.mSize.y);
			vertices[0].tex = vec2(0.5f);
			vertices[1].tex = vec2(0.5f);
			vertices[2].tex = vec2(0.5f);
			vertices[3].tex = vec2(0.5f);
			vertices[0].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[1].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[2].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[3].col = Color(color.rgb, color.a * getAlpha() / 255);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			pushData(vertices, 4, indices, 6);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRect(const Rect& rect, const HandleImage& image)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(image, scissor, 0);
			Vertex vertices[4];
			uint32_t indices[6];
			vertices[0].pos = vec2(offset) + vec2(rect.mOffset);
			vertices[1].pos = vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y);
			vertices[2].pos = vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y + rect.mSize.y);
			vertices[3].pos = vec2(offset) + vec2(rect.mOffset.x, rect.mOffset.y + rect.mSize.y);
			vertices[0].tex = vec2(0.0f, 0.0f);
			vertices[1].tex = vec2(1.0f, 0.0f);
			vertices[2].tex = vec2(1.0f, 1.0f);
			vertices[3].tex = vec2(0.0f, 1.0f);
			vertices[0].col = Color(255, 255, 255, getAlpha());
			vertices[1].col = Color(255, 255, 255, getAlpha());
			vertices[2].col = Color(255, 255, 255, getAlpha());
			vertices[3].col = Color(255, 255, 255, getAlpha());
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			pushData(vertices, 4, indices, 6);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRectFrame(const Rect& rect, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			vec2 points[4] = {
				vec2(offset) + vec2(rect.mOffset) + vec2(0.5f),
				vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y) + vec2(0.5f),
				vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y + rect.mSize.y) + vec2(0.5f),
				vec2(offset) + vec2(rect.mOffset.x, rect.mOffset.y + rect.mSize.y) + vec2(0.5f),
			};
			Vertex vertices[16];
			uint32_t indices[24];
			vertices[0].pos = vec2(points[0].x, points[0].y - 0.5f);
			vertices[1].pos = vec2(points[1].x, points[1].y - 0.5f);
			vertices[2].pos = vec2(points[1].x, points[1].y + 0.5f);
			vertices[3].pos = vec2(points[0].x, points[0].y + 0.5f);

			vertices[4].pos = vec2(points[1].x + 0.5f, points[1].y);
			vertices[5].pos = vec2(points[2].x + 0.5f, points[2].y);
			vertices[6].pos = vec2(points[2].x - 0.5f, points[2].y);
			vertices[7].pos = vec2(points[1].x - 0.5f, points[1].y);

			vertices[8].pos = vec2(points[2].x, points[2].y + 0.5f);
			vertices[9].pos = vec2(points[3].x, points[3].y + 0.5f);
			vertices[10].pos = vec2(points[3].x, points[3].y - 0.5f);
			vertices[11].pos = vec2(points[2].x, points[2].y - 0.5f);

			vertices[12].pos = vec2(points[3].x - 0.5f, points[3].y);
			vertices[13].pos = vec2(points[0].x - 0.5f, points[0].y);
			vertices[14].pos = vec2(points[0].x + 0.5f, points[0].y);
			vertices[15].pos = vec2(points[3].x + 0.5f, points[3].y);

			for (int i = 0; i < 16; i++)
			{
				vertices[i].tex = vec2(0.5f);
				vertices[i].col = Color(color.rgb, color.a * getAlpha() / 255);
			}
			for (int i = 0; i < 4; i++)
			{
				auto index_offset = i * 6;
				auto vertex_offset = i * 4;
				indices[index_offset + 0] = vertex_offset + 0;
				indices[index_offset + 1] = vertex_offset + 1;
				indices[index_offset + 2] = vertex_offset + 2;
				indices[index_offset + 3] = vertex_offset + 2;
				indices[index_offset + 4] = vertex_offset + 3;
				indices[index_offset + 5] = vertex_offset + 0;
			}
			checkState(getDefaultImage(), scissor, 0);
			pushData(vertices, 16, indices, 24);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawSDF(const Rect& rect, const Color& color, const HandleImage image)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			Vertex vertices[4];
			uint32_t indices[6];
			vertices[0].pos = vec2(offset) + vec2(rect.mOffset);
			vertices[1].pos = vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y);
			vertices[2].pos = vec2(offset) + vec2(rect.mOffset.x + rect.mSize.x, rect.mOffset.y + rect.mSize.y);
			vertices[3].pos = vec2(offset) + vec2(rect.mOffset.x, rect.mOffset.y + rect.mSize.y);
			vertices[0].tex = vec2(0.0f, 0.0f);
			vertices[1].tex = vec2(1.0f, 0.0f);
			vertices[2].tex = vec2(1.0f, 1.0f);
			vertices[3].tex = vec2(0.0f, 1.0f);
			vertices[0].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[1].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[2].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[3].col = Color(color.rgb, color.a * getAlpha() / 255);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			checkState(image, scissor, 1);
			pushData(vertices, 4, indices, 6);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color& color)
		{
			if (count <= 0)
				return;
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
			int interval = font.getInterval();
			auto font_image = GuiFontSystem::getGlobalFontImage(font.getFontID());
			auto font_image_info = GuiRenderSystem::getImageInfo(font_image);
			checkState(font_image, scissor, 1);
			Point cp = point + offset;
			Vertex vertices[4];
			vec4 font_texcoord;
			vec4 image_texcoord;
			uint32_t indices[6];
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			vertices[0].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[1].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[2].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[3].col = Color(color.rgb, color.a * getAlpha() / 255);
			for (int i = 0; i < count; i++)
			{
				auto& slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
				font_texcoord = vec4(cp.x + slot.xoffset * scalar, cp.y + slot.yoffset * scalar, slot.width * scalar, slot.height * scalar);
				image_texcoord = vec4(slot.xcoord / float(font_image_info.mWidth), slot.ycoord / float(font_image_info.mHeight),
					slot.width / float(font_image_info.mWidth), slot.height / float(font_image_info.mHeight));
				vertices[0].pos = vec2(font_texcoord.xy);
				vertices[1].pos = vec2(font_texcoord.x + font_texcoord.z, font_texcoord.y);
				vertices[2].pos = vec2(font_texcoord.x + font_texcoord.z, font_texcoord.y + font_texcoord.w);
				vertices[3].pos = vec2(font_texcoord.x, font_texcoord.y + font_texcoord.w);
				vertices[0].tex = vec2(image_texcoord.xy);
				vertices[1].tex = vec2(image_texcoord.x + image_texcoord.z, image_texcoord.y);
				vertices[2].tex = vec2(image_texcoord.x + image_texcoord.z, image_texcoord.y + image_texcoord.w);
				vertices[3].tex = vec2(image_texcoord.x, image_texcoord.y + image_texcoord.w);
				pushData(vertices, 4, indices, 6);
				cp.x += GuiFontSystem::step_forward(slot.xadvance, scalar, interval); // 
			}
		}

		CRAFT_ENGINE_GUI_API void Painter::drawColorTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color* colorList)
		{
			if (count <= 0)
				return;
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
			int interval = font.getInterval();
			auto font_image = GuiFontSystem::getGlobalFontImage(font.getFontID());
			auto font_image_info = GuiRenderSystem::getImageInfo(font_image);
			checkState(font_image, scissor, 1);
			Point cp = point + offset;
			Vertex vertices[4];
			vec4 font_texcoord;
			vec4 image_texcoord;
			uint32_t indices[6];
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			for (int i = 0; i < count; i++)
			{
				auto& slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
				font_texcoord = vec4(cp.x + slot.xoffset * scalar, cp.y + slot.yoffset * scalar, slot.width * scalar, slot.height * scalar);
				image_texcoord = vec4(slot.xcoord / float(font_image_info.mWidth), slot.ycoord / float(font_image_info.mHeight),
					slot.width / float(font_image_info.mWidth), slot.height / float(font_image_info.mHeight));
				vertices[0].pos = vec2(font_texcoord.xy);
				vertices[1].pos = vec2(font_texcoord.x + font_texcoord.z, font_texcoord.y);
				vertices[2].pos = vec2(font_texcoord.x + font_texcoord.z, font_texcoord.y + font_texcoord.w);
				vertices[3].pos = vec2(font_texcoord.x, font_texcoord.y + font_texcoord.w);
				vertices[0].tex = vec2(image_texcoord.xy);
				vertices[1].tex = vec2(image_texcoord.x + image_texcoord.z, image_texcoord.y);
				vertices[2].tex = vec2(image_texcoord.x + image_texcoord.z, image_texcoord.y + image_texcoord.w);
				vertices[3].tex = vec2(image_texcoord.x, image_texcoord.y + image_texcoord.w);
				Color color = Color(colorList[i].rgb, colorList[i].a * getAlpha() / 255);
				vertices[0].col = color;
				vertices[1].col = color;
				vertices[2].col = color;
				vertices[3].col = color;
				pushData(vertices, 4, indices, 6);
				cp.x += GuiFontSystem::step_forward(slot.xadvance, scalar, interval); // 
			}
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPaletteTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color* colorList, const uint16_t* colorIdxList)
		{
			if (count <= 0)
				return;
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
			int interval = font.getInterval();
			auto font_image = GuiFontSystem::getGlobalFontImage(font.getFontID());
			auto font_image_info = GuiRenderSystem::getImageInfo(font_image);
			checkState(font_image, scissor, 1);
			Point cp = point + offset;
			Vertex vertices[4];
			vec4 font_texcoord;
			vec4 image_texcoord;
			uint32_t indices[6];
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			for (int i = 0; i < count; i++)
			{
				auto& slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
				font_texcoord = vec4(cp.x + slot.xoffset * scalar, cp.y + slot.yoffset * scalar, slot.width * scalar, slot.height * scalar);
				image_texcoord = vec4(slot.xcoord / float(font_image_info.mWidth), slot.ycoord / float(font_image_info.mHeight),
					slot.width / float(font_image_info.mWidth), slot.height / float(font_image_info.mHeight));
				vertices[0].pos = vec2(font_texcoord.xy);
				vertices[1].pos = vec2(font_texcoord.x + font_texcoord.z, font_texcoord.y);
				vertices[2].pos = vec2(font_texcoord.x + font_texcoord.z, font_texcoord.y + font_texcoord.w);
				vertices[3].pos = vec2(font_texcoord.x, font_texcoord.y + font_texcoord.w);
				vertices[0].tex = vec2(image_texcoord.xy);
				vertices[1].tex = vec2(image_texcoord.x + image_texcoord.z, image_texcoord.y);
				vertices[2].tex = vec2(image_texcoord.x + image_texcoord.z, image_texcoord.y + image_texcoord.w);
				vertices[3].tex = vec2(image_texcoord.x, image_texcoord.y + image_texcoord.w);
				Color color = Color(colorList[colorIdxList[i]].rgb, colorList[colorIdxList[i]].a * getAlpha() / 255);
				vertices[0].col = color;
				vertices[1].col = color;
				vertices[2].col = color;
				vertices[3].col = color;
				pushData(vertices, 4, indices, 6);
				cp.x += GuiFontSystem::step_forward(slot.xadvance, scalar, interval); // 
			}
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRichTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const RichTextFormat& format)
		{
			drawTextLine(pStr, count, point, font, format.getColor());
		}

		CRAFT_ENGINE_GUI_API void Painter::drawTriangle(const Point& p1, const Point& p2, const Point& p3, const Color& color)
		{
			drawTriangle(PointF(p1), PointF(p2), PointF(p3), color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawTriangle(const PointF& p1, const PointF& p2, const PointF& p3, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			Vertex vertices[3];
			uint32_t indices[3];
			vertices[0].pos = vec2(offset) + vec2(p1);
			vertices[1].pos = vec2(offset) + vec2(p2);
			vertices[2].pos = vec2(offset) + vec2(p3);
			vertices[0].tex = vec2(0.5f);
			vertices[1].tex = vec2(0.5f);
			vertices[2].tex = vec2(0.5f);
			vertices[0].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[1].col = Color(color.rgb, color.a * getAlpha() / 255);
			vertices[2].col = Color(color.rgb, color.a * getAlpha() / 255);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			pushData(vertices, 3, indices, 3);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawTriangleFrame(const Point& p1, const Point& p2, const Point& p3, const Color& color)
		{
			drawTriangleFrame(PointF(p1), PointF(p2), PointF(p3), color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawTriangleFrame(const PointF& p1, const PointF& p2, const PointF& p3, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			drawLineNoCheck(p1, p2, color, color);
			drawLineNoCheck(p2, p3, color, color);
			drawLineNoCheck(p3, p1, color, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawCircle(const Point& center, float radius, float delta, const Color& color)
		{
			drawEllipse(center, radius, radius, delta, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawCircle(const PointF& center, float radius, float delta, const Color& color)
		{
			drawEllipse(center, radius, radius, delta, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawCircleFrame(const Point& center, float radius, float delta, const Color& color)
		{
			drawEllipseFrame(center, radius, radius, delta, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawCircleFrame(const PointF& center, float radius, float delta, const Color& color)
		{
			drawEllipseFrame(center, radius, radius, delta, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawEllipse(const Point& center, float rx, float ry, float delta, const Color& color)
		{
			drawEllipse(PointF(center), rx, ry, delta, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawEllipse(const PointF& center, float rx, float ry, float delta, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			int n = math::clamp(int(math::max(rx, ry) * 2.0f * math::pi() * delta), 4, 100);
			float delta_angle = 2.0f * math::pi() / n;
			checkState(getDefaultImage(), scissor, 0);
			auto draw_buffer = applyData(n + 1, n * 3);
			draw_buffer.mVertexBuffer[0].pos = vec2(offset) + vec2(center);
			draw_buffer.mVertexBuffer[0].tex = vec2(0.5f);
			draw_buffer.mVertexBuffer[0].col = Color(color.rgb, color.a * getAlpha() / 255);
			float t = 0.0f;
			for (int i = 1; i <= n; i++)
			{
				draw_buffer.mVertexBuffer[i].pos =
					vec2(draw_buffer.mVertexBuffer[0].pos) +
					vec2(math::cos(t), math::sin(t)) * vec2(rx, ry);
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
				t += delta_angle;
			}
			for (int i = 0; i < n - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(n - 1) * 3 + 0] = 0;
			draw_buffer.mIndexBuffer[(n - 1) * 3 + 1] = n;
			draw_buffer.mIndexBuffer[(n - 1) * 3 + 2] = 1;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawEllipseFrame(const Point& center, float rx, float ry, float delta, const Color& color)
		{
			drawEllipseFrame(PointF(center), rx, ry, delta, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawEllipseFrame(const PointF& center, float rx, float ry, float delta, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			int n = math::clamp(int(math::max(rx, ry) * 2.0f * math::pi() * delta), 4, 100);
			float delta_angle = 2.0f * math::pi() / n;
			float t = 0.0f;
			vec2 p1, p2;
			vec2 temp_center = vec2(center);
			p2 = temp_center + vec2(math::cos(t), math::sin(t)) * vec2(rx, ry);
			for (int i = 1; i < n; i++)
			{
				p1 = p2;
				t += delta_angle;
				p2 = temp_center + vec2(math::cos(t), math::sin(t)) * vec2(rx, ry);
				drawLineNoCheck(p1, p2, color, color);
			}
			p1 = p2;
			t += delta_angle;
			p2 = temp_center + vec2(math::cos(0.0f), math::sin(0.0f)) * vec2(rx, ry);
			drawLineNoCheck(p1, p2, color, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRoundRect(const Rect& rect, float radius, float delta, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			vec2 left_top = vec2(rect.mOffset + offset);
			vec2 half_size = vec2(rect.mSize) * 0.5f;
			float clamped_radius = math::min(half_size.x, half_size.y, radius);
			vec2 vertex_offsets[4];
			vertex_offsets[0] = half_size - clamped_radius;
			vertex_offsets[1] = vec2(vertex_offsets[0].x, -vertex_offsets[0].y);
			vertex_offsets[2] = vec2(vertex_offsets[0].x, vertex_offsets[0].y);
			vertex_offsets[3] = vec2(-vertex_offsets[0].x, vertex_offsets[0].y);
			vertex_offsets[0] = vec2(-vertex_offsets[0].x, -vertex_offsets[0].y);
			int n = math::clamp(int(clamped_radius * math::half_pi() * delta), 3, 100);
			float delta_angle = math::half_pi() / n;
			checkState(getDefaultImage(), scissor, 0);
			auto draw_buffer = applyData(4 * (n + 1) + 1, 4 * (n + 1) * 3);
			float t = -math::pi();
			draw_buffer.mVertexBuffer[0].pos = left_top + half_size;
			draw_buffer.mVertexBuffer[0].tex = vec2(0.5f);
			draw_buffer.mVertexBuffer[0].col = Color(color.rgb, color.a * getAlpha() / 255);
			for (int i = 0; i < 4; i++)
			{
				for (int j = (n + 1) * i; j < (n + 1) * (i + 1); j++)
				{
					draw_buffer.mVertexBuffer[j + 1].pos =
						draw_buffer.mVertexBuffer[0].pos +
						vertex_offsets[i] +
						vec2(math::cos(t), math::sin(t)) * clamped_radius;
					draw_buffer.mVertexBuffer[j + 1].tex = vec2(0.5f);
					draw_buffer.mVertexBuffer[j + 1].col = Color(color.rgb, color.a * getAlpha() / 255);
					t += delta_angle;
				}
				t -= delta_angle;
			}
			for (int i = 0; i < 4 * (n + 1) - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(4 * (n + 1) - 1) * 3 + 0] = 0;
			draw_buffer.mIndexBuffer[(4 * (n + 1) - 1) * 3 + 1] = 4 * (n + 1);
			draw_buffer.mIndexBuffer[(4 * (n + 1) - 1) * 3 + 2] = 1;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRoundRect(const Rect& rect, float radius, float delta, const HandleImage& image)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			vec2 left_top = vec2(rect.mOffset + offset);
			vec2 half_size = vec2(rect.mSize) * 0.5f;
			float clamped_radius = math::min(half_size.x, half_size.y, radius);
			vec2 vertex_offsets[4];
			vertex_offsets[0] = half_size - clamped_radius;
			vertex_offsets[1] = vec2(vertex_offsets[0].x, -vertex_offsets[0].y);
			vertex_offsets[2] = vec2(vertex_offsets[0].x, vertex_offsets[0].y);
			vertex_offsets[3] = vec2(-vertex_offsets[0].x, vertex_offsets[0].y);
			vertex_offsets[0] = vec2(-vertex_offsets[0].x, -vertex_offsets[0].y);
			int n = math::clamp(int(clamped_radius * math::half_pi() * delta), 3, 100);
			float delta_angle = math::half_pi() / n;
			checkState(image, scissor, 0);
			auto draw_buffer = applyData(4 * (n + 1) + 1, 4 * (n + 1) * 3);
			float t = -math::pi();
			draw_buffer.mVertexBuffer[0].pos = left_top + half_size;
			draw_buffer.mVertexBuffer[0].tex = vec2(0.5f, 0.5f);
			draw_buffer.mVertexBuffer[0].col = Color(255, 255, 255, getAlpha());
			for (int i = 0; i < 4; i++)
			{
				for (int j = (n + 1) * i; j < (n + 1) * (i + 1); j++)
				{
					draw_buffer.mVertexBuffer[j + 1].pos =
						draw_buffer.mVertexBuffer[0].pos +
						vertex_offsets[i] +
						vec2(math::cos(t), math::sin(t)) * clamped_radius;
					draw_buffer.mVertexBuffer[j + 1].tex = (draw_buffer.mVertexBuffer[j + 1].pos - left_top) / vec2(rect.mSize);
					draw_buffer.mVertexBuffer[j + 1].col = Color(255, 255, 255, getAlpha());
					t += delta_angle;
				}
				t -= delta_angle;
			}
			for (int i = 0; i < 4 * (n + 1) - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(4 * (n + 1) - 1) * 3 + 0] = 0;
			draw_buffer.mIndexBuffer[(4 * (n + 1) - 1) * 3 + 1] = 4 * (n + 1);
			draw_buffer.mIndexBuffer[(4 * (n + 1) - 1) * 3 + 2] = 1;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawRoundRectFrame(const Rect& rect, float radius, float delta, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			vec2 left_top = vec2(rect.mOffset);
			vec2 half_size = vec2(rect.mSize) * 0.5f;
			float clamped_radius = math::min(half_size.x, half_size.y, radius);
			vec2 vertex_offsets[4];
			vertex_offsets[0] = half_size - clamped_radius;
			vertex_offsets[1] = vec2(vertex_offsets[0].x, -vertex_offsets[0].y);
			vertex_offsets[2] = vec2(vertex_offsets[0].x, vertex_offsets[0].y);
			vertex_offsets[3] = vec2(-vertex_offsets[0].x, vertex_offsets[0].y);
			vertex_offsets[0] = vec2(-vertex_offsets[0].x, -vertex_offsets[0].y);
			int n = math::clamp(int(clamped_radius * math::half_pi() * delta), 3, 100);
			float delta_angle = math::half_pi() / n;
			vec2 center = left_top + half_size;
			vec2 p1, p2;
			float t = -math::pi();
			p2 = center + vertex_offsets[0] + vec2(math::cos(t), math::sin(t)) * clamped_radius;
			for (int j = 0; j < 4; j++)
			{
				int a = j;
				int b = (j + 1) % 4;
				for (int i = 0; i < n; i++)
				{
					p1 = p2;
					t += delta_angle;
					p2 = center + vertex_offsets[a] + vec2(math::cos(t), math::sin(t)) * clamped_radius;
					drawLineNoCheck(p1, p2, color, color);
				}
				p1 = p2;
				p2 = center + vertex_offsets[b] + vec2(math::cos(t), math::sin(t)) * clamped_radius;
				drawLineNoCheck(p1, p2, color, color);
			}
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolyline(const Point* vertices, uint32_t count, bool close, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			for (int i = 1; i < count; i++)
				drawLineNoCheck(vertices[i - 1], vertices[i], color, color);
			if (close)
				drawLineNoCheck(vertices[count - 1], vertices[0], color, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolyline(const PointF* vertices, uint32_t count, bool close, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			for (int i = 1; i < count; i++)
				drawLineNoCheck(vertices[i - 1], vertices[i], color, color);
			if (close)
				drawLineNoCheck(vertices[count - 1], vertices[0], color, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolyline(const Point* vertices, uint32_t count, bool close, const Color* colors)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			for (int i = 1; i < count; i++)
				drawLineNoCheck(vertices[i - 1], vertices[i], colors[i - 1], colors[i]);
			if (close)
				drawLineNoCheck(vertices[count - 1], vertices[0], colors[count - 1], colors[0]);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolyline(const PointF* vertices, uint32_t count, bool close, const Color* colors)
		{
			auto& scissor = getGlobalScissor();
			checkState(getDefaultImage(), scissor, 0);
			for (int i = 1; i < count; i++)
				drawLineNoCheck(vertices[i - 1], vertices[i], colors[i - 1], colors[i]);
			if (close)
				drawLineNoCheck(vertices[count - 1], vertices[0], colors[count - 1], colors[0]);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolygon(const Point* vertices, uint32_t count, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			auto triangle_count = count - 2;
			auto draw_buffer = applyData(count, triangle_count * 3);
			for (int i = 0; i < count; i++)
			{
				draw_buffer.mVertexBuffer[i].pos = vec2(offset) + vec2(vertices[i]);
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
			}

			for (int i = 0; i < triangle_count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = i + 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 0] = triangle_count - 2;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 1] = triangle_count - 1;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 2] = 0;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolygon(const PointF* vertices, uint32_t count, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			auto triangle_count = count - 2;
			auto draw_buffer = applyData(count, triangle_count * 3);
			for (int i = 0; i < count; i++)
			{
				draw_buffer.mVertexBuffer[i].pos = vec2(offset) + vec2(vertices[i]);
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
			}

			for (int i = 0; i < triangle_count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = i + 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 0] = triangle_count - 2;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 1] = triangle_count - 1;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 2] = 0;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolygon(const Point* vertices, uint32_t count, const Color* colors)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			auto triangle_count = count - 2;
			auto draw_buffer = applyData(count, triangle_count * 3);
			for (int i = 0; i < count; i++)
			{
				draw_buffer.mVertexBuffer[i].pos = vec2(offset) + vec2(vertices[i]);
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(colors[i].rgb, colors[i].a * getAlpha() / 255);
			}

			for (int i = 0; i < triangle_count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = i + 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 0] = triangle_count - 2;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 1] = triangle_count - 1;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 2] = 0;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolygon(const PointF* vertices, uint32_t count, const Color* colors)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			auto triangle_count = count - 2;
			auto draw_buffer = applyData(count, triangle_count * 3);
			for (int i = 0; i < count; i++)
			{
				draw_buffer.mVertexBuffer[i].pos = vec2(offset) + vec2(vertices[i]);
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(colors[i].rgb, colors[i].a * getAlpha() / 255);
			}

			for (int i = 0; i < triangle_count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 3 + 0] = i + 0;
				draw_buffer.mIndexBuffer[i * 3 + 1] = i + 1;
				draw_buffer.mIndexBuffer[i * 3 + 2] = i + 2;
			}
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 0] = triangle_count - 2;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 1] = triangle_count - 1;
			draw_buffer.mIndexBuffer[(triangle_count - 1) * 3 + 2] = 0;
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWideLine(const Point& p1, const Point& p2, float width, const Color& color1, const Color color2)
		{
			drawWideLine(PointF(p1), PointF(p2), width, color1, color2);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWideLine(const PointF& p1, const PointF& p2, float width, const Color& color1, const Color color2)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			vec2 points[2] = { p1 + vec2(offset) + 0.5f, p2 + vec2(offset) + 0.5f };
			vec2 dxdy = math::normalize(vec2(p2 - p1)) * 0.5f * math::max(1.0f, width);
			Vertex vertices[4];
			uint32_t indices[6];
			vertices[0].pos = vec2(points[0].x - dxdy.y, points[0].y + dxdy.x);
			vertices[1].pos = vec2(points[1].x - dxdy.y, points[1].y + dxdy.x);
			vertices[2].pos = vec2(points[1].x + dxdy.y, points[1].y - dxdy.x);
			vertices[3].pos = vec2(points[0].x + dxdy.y, points[0].y - dxdy.x);
			vertices[0].tex = vec2(0.5f);
			vertices[1].tex = vec2(0.5f);
			vertices[2].tex = vec2(0.5f);
			vertices[3].tex = vec2(0.5f);
			vertices[0].col = Color(color1.rgb, color1.r * getAlpha() / 255);
			vertices[1].col = Color(color2.rgb, color2.r * getAlpha() / 255);
			vertices[2].col = Color(color2.rgb, color2.r * getAlpha() / 255);
			vertices[3].col = Color(color1.rgb, color1.r * getAlpha() / 255);
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;
			pushData(vertices, 4, indices, 6);
		}


		CRAFT_ENGINE_GUI_API void Painter::drawWidePolyline(const Point* vertices, uint32_t count, float width, bool close, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			vec2 segment_points[3], segment_directions[2], segment_normals[2], n, p1, p2;
			float cos_theta, cos_two_theta, len;
			const int vertex_count = count * 2;
			const int index_count = (close ? (count * 6) : (count - 1) * 6);
			const float half_width = 0.5f * math::max(1.0f, width);
			auto draw_buffer = applyData(vertex_count, index_count);

			if (close)
			{
				segment_points[0] = vec2(vertices[count - 1]);
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}

				segment_points[0] = segment_points[1];
				segment_points[1] = segment_points[2];
				segment_points[2] = vec2(vertices[0]);
				segment_directions[0] = segment_directions[1];
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);

				draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
			}
			else
			{
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				p1 = segment_points[1] - segment_normals[1] * half_width;
				p2 = segment_points[1] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}

				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				p1 = segment_points[2] - segment_normals[1] * half_width;
				p2 = segment_points[2] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
			}

			for (int i = 0; i < count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
			}
			for (int i = 0; i < vertex_count; i++)
			{
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
			}

			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWidePolyline(const PointF* vertices, uint32_t count, float width, bool close, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			vec2 segment_points[3], segment_directions[2], segment_normals[2], n, p1, p2;
			float cos_theta, cos_two_theta, len;
			const int vertex_count = count * 2;
			const int index_count = (close ? (count * 6) : (count - 1) * 6);
			const float half_width = 0.5f * math::max(1.0f, width);
			auto draw_buffer = applyData(vertex_count, index_count);

			if (close)
			{
				segment_points[0] = vec2(vertices[count - 1]);
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}

				segment_points[0] = segment_points[1];
				segment_points[1] = segment_points[2];
				segment_points[2] = vec2(vertices[0]);
				segment_directions[0] = segment_directions[1];
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);

				draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
			}
			else
			{
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				p1 = segment_points[1] - segment_normals[1] * half_width;
				p2 = segment_points[1] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}

				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				p1 = segment_points[2] - segment_normals[1] * half_width;
				p2 = segment_points[2] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
			}

			for (int i = 0; i < count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
			}
			for (int i = 0; i < vertex_count; i++)
			{
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
			}

			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWidePolyline(const Point* vertices, uint32_t count, float width, bool close, const Color* colors)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			vec2 segment_points[3], segment_directions[2], segment_normals[2], n, p1, p2;
			float cos_theta, cos_two_theta, len;
			const int vertex_count = count * 2;
			const int index_count = (close ? (count * 6) : (count - 1) * 6);
			const float half_width = 0.5f * math::max(1.0f, width);
			auto draw_buffer = applyData(vertex_count, index_count);

			if (close)
			{
				segment_points[0] = vec2(vertices[count - 1]);
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}

				segment_points[0] = segment_points[1];
				segment_points[1] = segment_points[2];
				segment_points[2] = vec2(vertices[0]);
				segment_directions[0] = segment_directions[1];
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);

				draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
			}
			else
			{
				//segment_points[0] = segment_points[count - 1];
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				//segment_directions[0] = segment_directions[1];
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				//segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				//cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				//cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				//n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta;
				p1 = segment_points[1] - segment_normals[1] * half_width;
				p2 = segment_points[1] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				p1 = segment_points[2] - segment_normals[1] * half_width;
				p2 = segment_points[2] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
			}

			for (int i = 0; i < count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
			}
			for (int i = 0; i < vertex_count; i++)
			{
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(colors[i / 2].rgb, colors[i / 2].a * getAlpha() / 255);
			}

			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWidePolyline(const PointF* vertices, uint32_t count, float width, bool close, const Color* colors)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			checkState(getDefaultImage(), scissor, 0);
			vec2 segment_points[3], segment_directions[2], segment_normals[2], n, p1, p2;
			float cos_theta, cos_two_theta, len;
			const int vertex_count = count * 2;
			const int index_count = (close ? (count * 6) : (count - 1) * 6);
			const float half_width = 0.5f * math::max(1.0f, width);
			auto draw_buffer = applyData(vertex_count, index_count);

			if (close)
			{
				segment_points[0] = vec2(vertices[count - 1]);
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}

				segment_points[0] = segment_points[1];
				segment_points[1] = segment_points[2];
				segment_points[2] = vec2(vertices[0]);
				segment_directions[0] = segment_directions[1];
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
				p1 = segment_points[1] - n;
				p2 = segment_points[1] + n;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);

				draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
				draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
			}
			else
			{
				//segment_points[0] = segment_points[count - 1];
				segment_points[1] = vec2(vertices[0]);
				segment_points[2] = vec2(vertices[1]);
				//segment_directions[0] = segment_directions[1];
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				//segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				//cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
				//cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
				//n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta;
				p1 = segment_points[1] - segment_normals[1] * half_width;
				p2 = segment_points[1] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[1].pos = p2 + vec2(offset) + vec2(0.5f);
				for (int i = 1; i < count - 1; i++)
				{
					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(vertices[i + 1]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
					draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
				}
				segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
				segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
				p1 = segment_points[2] - segment_normals[1] * half_width;
				p2 = segment_points[2] + segment_normals[1] * half_width;

				draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1 + vec2(offset) + vec2(0.5f);
				draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2 + vec2(offset) + vec2(0.5f);
			}

			for (int i = 0; i < count - 1; i++)
			{
				draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
				draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
				draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
			}
			for (int i = 0; i < vertex_count; i++)
			{
				draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				draw_buffer.mVertexBuffer[i].col = Color(colors[i / 2].rgb, colors[i / 2].a * getAlpha() / 255);
			}
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWideRoundPolyline(const Point* vertices, uint32_t count, float width, bool close, const Color& color)
		{
			float clamped_width = math::max(1.0f, width);
			for (int i = 1; i < count; i++)
				drawWideLine(vertices[i - 1], vertices[i], clamped_width, color, color);
			if (close)
				drawWideLine(vertices[count - 1], vertices[0], clamped_width, color, color);
			float radius = 0.5f * clamped_width;
			for (int i = 0; i < count; i++)
				drawCircle(vertices[i], radius, 0.5f, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawWideRoundPolyline(const PointF* vertices, uint32_t count, float width, bool close, const Color& color)
		{
			float clamped_width = math::max(1.0f, width);
			for (int i = 1; i < count; i++)
				drawWideLine(vertices[i - 1], vertices[i], clamped_width, color, color);
			if (close)
				drawWideLine(vertices[count - 1], vertices[0], clamped_width, color, color);
			float radius = 0.5f * clamped_width;
			for (int i = 0; i < count; i++)
				drawCircle(vertices[i], radius, 0.5f, color);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolygonEx(const PolygonInfo& info, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			mat4 transform = math::scale(vec3(info.scale, 1.0f));
			transform = math::rotate(info.rotation, vec3(0.0f, 0.0f, 1.0f), transform);
			transform = math::translate(vec3(info.translate + vec2(offset), 0.0f), transform);
			newDrawCommand(transform);
			checkState(info.image == nullptr ? getDefaultImage() : info.image, scissor, 2);
			auto index_count = info.indexCount == 0 ? info.vertexCount : info.indexCount;
			auto draw_buffer = applyData(info.vertexCount, index_count);
			if (info.mixBuffer != nullptr)
			{
				for (int i = 0; i < info.vertexCount; i++)
				{
					draw_buffer.mVertexBuffer[i].pos = info.mixBuffer[i].pos;
					draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
					draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
				}
			}
			else
			{
				if (info.posBuffer != nullptr)
				{
					for (int i = 0; i < info.vertexCount; i++)
					{
						draw_buffer.mVertexBuffer[i].pos = info.posBuffer[i];
						draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
						draw_buffer.mVertexBuffer[i].col = Color(color.rgb, color.a * getAlpha() / 255);
					}
				}
			}

			if (info.indexList != nullptr)
			{
				for (int i = 0; i < info.indexCount; i++)
					draw_buffer.mIndexBuffer[i] = info.indexList[i];
			}
			else
			{
				for (int i = 0; i < info.vertexCount; i++)
					draw_buffer.mIndexBuffer[i] = i;
			}
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolygonEx(const PolygonInfo& info)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			mat4 transform = math::scale(vec3(info.scale, 1.0f));
			transform = math::rotate(info.rotation, vec3(0.0f, 0.0f, 1.0f), transform);
			transform = math::translate(vec3(info.translate + vec2(offset), 0.0f), transform);

			newDrawCommand(transform);
			bool use_texture = info.image != nullptr;
			checkState(info.image == nullptr ? getDefaultImage() : info.image, scissor, 2);
			auto index_count = info.indexCount == 0 ? info.vertexCount : info.indexCount;
			auto draw_buffer = applyData(info.vertexCount, index_count);
			if (info.mixBuffer != nullptr)
			{
				if (use_texture)
				{
					for (int i = 0; i < info.vertexCount; i++)
					{
						draw_buffer.mVertexBuffer[i].pos = info.mixBuffer[i].pos;
						draw_buffer.mVertexBuffer[i].tex = info.mixBuffer[i].tex;
						draw_buffer.mVertexBuffer[i].col = Color(info.mixBuffer[i].col.rgb, info.mixBuffer[i].col.a * getAlpha() / 255);
					}
				}
				else
				{
					for (int i = 0; i < info.vertexCount; i++)
					{
						draw_buffer.mVertexBuffer[i].pos = info.mixBuffer[i].pos;
						draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
						draw_buffer.mVertexBuffer[i].col = Color(info.mixBuffer[i].col.rgb, info.mixBuffer[i].col.a * getAlpha() / 255);
					}
				}
			}
			else
			{
				if (info.posBuffer != nullptr)
					for (int i = 0; i < info.vertexCount; i++)
						draw_buffer.mVertexBuffer[i].pos = info.posBuffer[i];
				if (info.texBuffer != nullptr && info.image != nullptr)
					for (int i = 0; i < info.vertexCount; i++)
						draw_buffer.mVertexBuffer[i].tex = info.texBuffer[i];
				else
					for (int i = 0; i < info.vertexCount; i++)
						draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);

				if (info.colBuffer != nullptr)
					for (int i = 0; i < info.vertexCount; i++)
						draw_buffer.mVertexBuffer[i].col = Color(info.colBuffer[i].rgb, info.colBuffer[i].a * getAlpha() / 255);
				else
					for (int i = 0; i < info.vertexCount; i++)
						draw_buffer.mVertexBuffer[i].col = Color(255, 255, 255, getAlpha());
			}

			if (info.indexList != nullptr)
			{
				for (int i = 0; i < info.indexCount; i++)
					draw_buffer.mIndexBuffer[i] = info.indexList[i];
			}
			else
			{
				for (int i = 0; i < info.vertexCount; i++)
					draw_buffer.mIndexBuffer[i] = i;
			}
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolylineEx(const PolygonInfo& info, const Color& color)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			mat4 transform = math::scale(vec3(info.scale, 1.0f));
			transform = math::rotate(info.rotation, vec3(0.0f, 0.0f, 1.0f), transform);
			transform = math::translate(vec3(info.translate + vec2(offset) + vec2(0.5f), 0.0f), transform);

			newDrawCommand(transform);
			checkState(info.image == nullptr ? getDefaultImage() : info.image, scissor, 2);

			vec2 segment_points[3], segment_directions[2], segment_normals[2], n, p1, p2;
			float cos_theta, cos_two_theta, len;
			const int count = info.indexCount == 0 ? info.vertexCount : info.indexCount;
			const int vertex_count = count * 2;
			const bool close = info.close;
			const int index_count = (close ? (count * 6) : (count - 1) * 6);
			const float line_half_width = 0.5f * math::max(1.0f, info.lineWidth);
			const vec2 half_width = line_half_width * math::inverse(vec2(info.scale));
			auto draw_buffer = applyData(vertex_count, index_count);

			if (info.indexCount == 0)
				for (int i = 0; i < count; i++)
					draw_buffer.mIndexBuffer[i] = i;
			else
				for (int i = 0; i < count; i++)
					draw_buffer.mIndexBuffer[i] = info.indexList[i];

			if (info.mixBuffer != nullptr)
			{
				if (close)
				{
					segment_points[0] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[count - 1]].pos);
					segment_points[1] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[0]].pos);
					segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[1]].pos);
					segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[i + 1]].pos);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[0]].pos);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;

					draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
				}
				else
				{
					segment_points[1] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[0]].pos);
					segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[1]].pos);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[1] - segment_normals[1] * half_width;
					p2 = segment_points[1] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[i + 1]].pos);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[2] - segment_normals[1] * half_width;
					p2 = segment_points[2] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;
				}

				for (int i = 0; i < count - 1; i++)
				{
					draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
				}
				if (info.image != nullptr)
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].tex = info.mixBuffer[i / 2].tex;
				else
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				for (int i = 0; i < vertex_count; i++)
					draw_buffer.mVertexBuffer[i].col = Color(info.mixBuffer[i / 2].col.rgb, info.mixBuffer[i / 2].col.a * getAlpha() / 255);
			}
			else
			{
				if (close)
				{
					segment_points[0] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[count - 1]]);
					segment_points[1] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[0]]);
					segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[1]]);
					segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[i + 1]]);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[0]]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;

					draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
				}
				else
				{
					segment_points[1] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[0]]);
					segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[1]]);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[1] - segment_normals[1] * half_width;
					p2 = segment_points[1] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[i + 1]]);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[2] - segment_normals[1] * half_width;
					p2 = segment_points[2] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;
				}
				for (int i = 0; i < count - 1; i++)
				{
					draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
				}
				if (info.texBuffer != nullptr && info.image != nullptr)
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].tex = info.texBuffer[i / 2];
				else
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				for (int i = 0; i < vertex_count; i++)
					draw_buffer.mVertexBuffer[i].col = Color(255, 255, 255, getAlpha());
			}
			submitData(draw_buffer);
		}

		CRAFT_ENGINE_GUI_API void Painter::drawPolylineEx(const PolygonInfo& info)
		{
			auto& scissor = getGlobalScissor();
			auto& offset = getGlobalBasepoint();
			mat4 transform = math::scale(vec3(info.scale, 1.0f));
			transform = math::rotate(info.rotation, vec3(0.0f, 0.0f, 1.0f), transform);
			transform = math::translate(vec3(info.translate + vec2(offset), 0.0f), transform);

			newDrawCommand(transform);
			checkState(info.image == nullptr ? getDefaultImage() : info.image, scissor, 2);

			vec2 segment_points[3], segment_directions[2], segment_normals[2], n, p1, p2;
			float cos_theta, cos_two_theta, len;
			const int count = info.indexCount == 0 ? info.vertexCount : info.indexCount;
			const int vertex_count = count * 2;
			const bool close = info.close;
			const int index_count = (close ? (count * 6) : (count - 1) * 6);
			const float line_half_width = 0.5f * math::max(1.0f, info.lineWidth);
			const vec2 half_width = line_half_width * math::inverse(vec2(info.scale));
			auto draw_buffer = applyData(vertex_count, index_count);

			if (info.indexCount == 0)
				for (int i = 0; i < count; i++)
					draw_buffer.mIndexBuffer[i] = i;
			else
				for (int i = 0; i < count; i++)
					draw_buffer.mIndexBuffer[i] = info.indexList[i];

			if (info.mixBuffer != nullptr)
			{
				if (close)
				{
					segment_points[0] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[count - 1]].pos);
					segment_points[1] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[0]].pos);
					segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[1]].pos);
					segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[i + 1]].pos);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[0]].pos);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;

					draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
				}
				else
				{
					segment_points[1] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[0]].pos);
					segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[1]].pos);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[1] - segment_normals[1] * half_width;
					p2 = segment_points[1] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.mixBuffer[draw_buffer.mIndexBuffer[i + 1]].pos);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[2] - segment_normals[1] * half_width;
					p2 = segment_points[2] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;
				}

				for (int i = 0; i < count - 1; i++)
				{
					draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
				}
				for (int i = 0; i < vertex_count; i++)
				{
					draw_buffer.mVertexBuffer[i].tex = info.mixBuffer[i / 2].tex;
					draw_buffer.mVertexBuffer[i].col = Color(info.mixBuffer[i / 2].col.rgb, info.mixBuffer[i / 2].col.a * getAlpha() / 255); 
				}
			}
			else
			{
				if (close)
				{
					segment_points[0] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[count - 1]]);
					segment_points[1] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[0]]);
					segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[1]]);
					segment_directions[0] = math::normalize(segment_points[1] - segment_points[0]);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[i + 1]]);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_points[0] = segment_points[1];
					segment_points[1] = segment_points[2];
					segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[0]]);
					segment_directions[0] = segment_directions[1];
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
					cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
					n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
					p1 = segment_points[1] - n;
					p2 = segment_points[1] + n;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;

					draw_buffer.mIndexBuffer[(count - 1) * 6 + 0] = (count - 1) * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 1] = 0 * 2 + 0;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 2] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 3] = 0 * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 4] = (count - 1) * 2 + 1;
					draw_buffer.mIndexBuffer[(count - 1) * 6 + 5] = (count - 1) * 2 + 0;
				}
				else
				{
					segment_points[1] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[0]]);
					segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[1]]);
					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[1] - segment_normals[1] * half_width;
					p2 = segment_points[1] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[0].pos = p1;
					draw_buffer.mVertexBuffer[1].pos = p2;
					for (int i = 1; i < count - 1; i++)
					{
						segment_points[0] = segment_points[1];
						segment_points[1] = segment_points[2];
						segment_points[2] = vec2(info.posBuffer[draw_buffer.mIndexBuffer[i + 1]]);
						segment_directions[0] = segment_directions[1];
						segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
						segment_normals[0] = vec2(-segment_directions[0].y, segment_directions[0].x);
						segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
						cos_two_theta = math::dot(segment_normals[0], segment_normals[1]);
						cos_theta = math::sqrt((1.0f - cos_two_theta) * 0.5f);
						n = math::normalize(segment_normals[0] + segment_normals[1]) / cos_theta * half_width;
						p1 = segment_points[1] - n;
						p2 = segment_points[1] + n;

						draw_buffer.mVertexBuffer[i * 2 + 0].pos = p1;
						draw_buffer.mVertexBuffer[i * 2 + 1].pos = p2;
					}

					segment_directions[1] = math::normalize(segment_points[2] - segment_points[1]);
					segment_normals[1] = vec2(-segment_directions[1].y, segment_directions[1].x);
					p1 = segment_points[2] - segment_normals[1] * half_width;
					p2 = segment_points[2] + segment_normals[1] * half_width;

					draw_buffer.mVertexBuffer[(count - 1) * 2 + 0].pos = p1;
					draw_buffer.mVertexBuffer[(count - 1) * 2 + 1].pos = p2;
				}
				for (int i = 0; i < count - 1; i++)
				{
					draw_buffer.mIndexBuffer[i * 6 + 0] = i * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 1] = (i + 1) * 2 + 0;
					draw_buffer.mIndexBuffer[i * 6 + 2] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 3] = (i + 1) * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 4] = i * 2 + 1;
					draw_buffer.mIndexBuffer[i * 6 + 5] = i * 2 + 0;
				}
				if (info.texBuffer != nullptr)
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].tex = info.texBuffer[i / 2];
				else
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].tex = vec2(0.5f);
				if (info.colBuffer != nullptr)
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].col = Color(info.colBuffer[i / 2].rgb, info.colBuffer[i / 2].a * getAlpha() / 255);
				else
					for (int i = 0; i < vertex_count; i++)
						draw_buffer.mVertexBuffer[i].col = Color(255, 255, 255, getAlpha());
			}
			submitData(draw_buffer);
		}


	}
}





namespace CraftEngine
{
	namespace gui
	{


		// GuiRenderSystem
		CRAFT_ENGINE_GUI_API void GuiRenderSystem::initSystem(AbstractGuiRenderSystem* renderSystem)
		{
			if (!m_isInitialized)
			{
				m_renderSystem = renderSystem;
				GuiColorStyle::init();
				gui::_Gui_Api_Detail::_Load_Default_Assets();
				gui::_Gui_Api_Detail::_Load_Default_Font_File();
				gui::_Gui_Api_Detail::_Load_Fixed_Width_Font_File();
				m_isInitialized = true;
				std::cout << "Gui Renderer System is initialized." << std::endl;
			}
			else
			{
				throw std::runtime_error("Gui Renderer System is already initialized");
			}
		}
		CRAFT_ENGINE_GUI_API void GuiRenderSystem::destroySystem()
		{
			if (m_isInitialized)
			{
				GuiAssetsManager::clear();
				GuiFontSystem::clearAllFont();
				delete m_renderSystem;
				m_renderSystem = nullptr;
				m_isInitialized = false;
				std::cout << "Gui Renderer System is destroyed." << std::endl;
			}
			else
			{
				throw std::runtime_error("Gui Renderer System is not yet initialized");
			}
		}
		CRAFT_ENGINE_GUI_API inline bool GuiRenderSystem::isInitialized()
		{
			return m_isInitialized;
		}
		CRAFT_ENGINE_GUI_API HandleImage GuiRenderSystem::createImage(const void* data, uint32_t size, uint32_t width, uint32_t height){
			return m_renderSystem->createImage(data, size, width, height);
		}
		CRAFT_ENGINE_GUI_API  void GuiRenderSystem::deleteImage(HandleImage image){
			m_renderSystem->deleteImage(image);
		}
		CRAFT_ENGINE_GUI_API  void GuiRenderSystem::updateImage(HandleImage image, const void* data, uint32_t size, uint32_t width, uint32_t height) {
			m_renderSystem->updateImage(image, data, size, width, height);		
		}
		CRAFT_ENGINE_GUI_API ImageInfo GuiRenderSystem::getImageInfo(HandleImage image){
			return m_renderSystem->getImageInfo(image);
		}
		CRAFT_ENGINE_GUI_API AbstractGuiCanvas* GuiRenderSystem::createGuiCanvas(const Size& size) {
			return m_renderSystem->createGuiCanvas(size);
		}
		CRAFT_ENGINE_GUI_API void GuiRenderSystem::deleteGuiCanvas(AbstractGuiCanvas* canvas) {
			return m_renderSystem->deleteGuiCanvas(canvas);
		}



		CRAFT_ENGINE_GUI_API HandleImage GuiAssetsManager::loadImage(const std::string& name)
		{
			auto it = m_images.find(name);
			if (it == m_images.end())
			{
				std::cout << ("image [" + name + "] dose not exist!");
				return HandleImage();
			}
			else
				return it->second;
		}
		CRAFT_ENGINE_GUI_API void GuiAssetsManager::storeImage(const char* name, HandleImage image)
		{
			auto pair = std::make_pair<std::string, HandleImage>(std::string(name), HandleImage(image));
			m_images.emplace(pair);
		}
		CRAFT_ENGINE_GUI_API inline HandleCursor GuiAssetsManager::loadCursor(const std::string& name)
		{
			auto it = m_cursors.find(name);
			if (it == m_cursors.end())
			{
				std::cout << ("cursor [" + name + "] dose not exist!");
				return HandleCursor();
			}
			else
				return HandleCursor(it->second);	
		}
		CRAFT_ENGINE_GUI_API inline void GuiAssetsManager::storeCursor(const char* name, AbstractCursor* cursor)
		{
			auto pair = std::make_pair<std::string, AbstractCursor*>(std::string(name), (AbstractCursor*)(cursor));
			m_cursors.emplace(pair);
		}
		CRAFT_ENGINE_GUI_API inline void GuiAssetsManager::clear()
		{
			for (auto it : m_images)
				GuiRenderSystem::deleteImage(it.second);
			m_images.clear();
			for (auto it : m_cursors)
				delete it.second;
			m_cursors.clear();
		}



		CRAFT_ENGINE_GUI_API void GuiFontSystem::freeFont(uint32_t id)
		{
			if (m_instanceList[id] != nullptr)
			{
				GuiRenderSystem::deleteImage(m_instanceList[id]->m_fontImage);
				delete m_instanceList[id];
				m_instanceList[id] = nullptr;
			}
		}

		CRAFT_ENGINE_GUI_API void GuiFontSystem::loadFont(const FontFile* font, uint32_t id)
		{
			freeFont(id);
			m_instanceList[id] = new FontInstance;
			m_instanceList[id]->m_fontInfo.scalar = 1.0f / font->fontYHeight();
			m_instanceList[id]->m_fontInfo.yHeight = font->fontYHeight();
			m_instanceList[id]->m_fontInfo.yLineHeight = font->fontYLineHeight();
			m_instanceList[id]->m_fontInfo.yMaxHeight = font->fontYMaxHeight();
			m_instanceList[id]->m_fontInfo.yMaxOffset = font->fontYMaxOffset();
			m_instanceList[id]->m_fontInfo.yOffset = font->fontYOffset();
			memset(m_instanceList[id]->m_fontUnits.data(), 0, sizeof(m_instanceList[id]->m_fontUnits));

			auto pfont_units = m_instanceList[id]->m_fontUnits.data();
			for (int i = 0; i < font->fontCount(); i++)
			{
				pfont_units[font->fontUnits()[i].id].id = font->fontUnits()[i].id;
				pfont_units[font->fontUnits()[i].id].xcoord = font->fontUnits()[i].xcoord;
				pfont_units[font->fontUnits()[i].id].ycoord = font->fontUnits()[i].ycoord;
				pfont_units[font->fontUnits()[i].id].width = font->fontUnits()[i].width;
				pfont_units[font->fontUnits()[i].id].height = font->fontUnits()[i].height;
				pfont_units[font->fontUnits()[i].id].xoffset = font->fontUnits()[i].xoffset;
				pfont_units[font->fontUnits()[i].id].yoffset = font->fontUnits()[i].yoffset;
				pfont_units[font->fontUnits()[i].id].xadvance = font->fontUnits()[i].xadvance;
			}
			for (int i = 1; i < 65535; i++)
				if (pfont_units[i].id == 0)
					pfont_units[i] = pfont_units[L'?'];

			pfont_units['\t'] = pfont_units[' '];
			pfont_units['\t'].xadvance = pfont_units['\t'].xadvance * 4;
			pfont_units['\t'].width = pfont_units['\t'].width * 4;
			pfont_units['\t'].id = '\t';

			auto& fontBitmap = font->getSdfImage();
			m_instanceList[id]->m_fontImage = GuiRenderSystem::createImage(fontBitmap.data(), fontBitmap.size(), fontBitmap.width(), fontBitmap.height());
		}

		CRAFT_ENGINE_GUI_API const GuiFontSystem::FontUnit& GuiFontSystem::getGlobalFontUnit(Char idx, int id)
		{
			return m_instanceList[id]->m_fontUnits[idx];
		}
		CRAFT_ENGINE_GUI_API HandleImage GuiFontSystem::getGlobalFontImage(int id)
		{
			return m_instanceList[id]->m_fontImage;
		}
		CRAFT_ENGINE_GUI_API float GuiFontSystem::getGlobalFontCharWidth(Char c, int id)
		{
			return m_instanceList[id]->m_fontUnits[c].xadvance;
		}



		CRAFT_ENGINE_GUI_API int GuiFontSystem::calcFontLineWidth(const Char* pStr, int32_t count, const Font& font)
		{
			int wacc = 0.0f;
			float scalar = font.getSize() * getGlobalFontScalar(font.getFontID());
			int interval = font.getInterval();
			for (int i = 0; i < count; i++)
				wacc += step_forward(getGlobalFontUnit(pStr[i], font.getFontID()).xadvance, scalar, interval);//
			return wacc;
		}
		CRAFT_ENGINE_GUI_API int GuiFontSystem::calcFontLineWidth(const String& str, const Font& font)
		{
			return calcFontLineWidth(str.data(), str.size(), font);
		}
		CRAFT_ENGINE_GUI_API int GuiFontSystem::calcFontLineWidth(const StringRef& str, const Font& font)
		{
			return calcFontLineWidth(str.data(), str.size(), font);
		}
		CRAFT_ENGINE_GUI_API Point GuiFontSystem::calcFontBasePoint(const Char* pStr, int32_t count, const Rect& rect, const Font& font)
		{
			float w = GuiFontSystem::calcFontLineWidth(pStr, count, font);
			float x = 0, y = 0;
			switch (font.getAlignment().mHorizonMode)
			{
			case Alignment::AlignmentMode::eAlignmentMode_Center:
				x = rect.left() + 0.5f * (rect.mWidth - w); break;
			case Alignment::AlignmentMode::eAlignmentMode_Left:
				x = rect.left() + font.getAlignment().mHorizonOffset; break;
			case Alignment::AlignmentMode::eAlignmentMode_Right:
				x = rect.right() - w - font.getAlignment().mHorizonOffset; break;
			default:
				x = rect.left() + 0.5f * (rect.mWidth - w); break;
			}

			switch (font.getAlignment().mVerticalMode)
			{
			case Alignment::AlignmentMode::eAlignmentMode_Center:
				y = rect.top() + 0.5f * (rect.mHeight - (getGlobalFontYHeight(font.getFontID()) + 2 * getGlobalFontYOffset(font.getFontID())) * getGlobalFontScalar(font.getFontID()) * font.getSize()); break;
			case Alignment::AlignmentMode::eAlignmentMode_Top:
				y = rect.top() - (getGlobalFontYOffset(font.getFontID()) + 0.5 * (getGlobalFontYHeight(font.getFontID()) - font.getLineHeight() * getGlobalFontYStandardLineHeight(font.getFontID()))) * getGlobalFontScalar(font.getFontID()) * font.getSize() + font.getAlignment().mHorizonOffset; break;
			case Alignment::AlignmentMode::eAlignmentMode_Bottom:
				y = rect.bottom() - (getGlobalFontYOffset(font.getFontID()) + 0.5 * (getGlobalFontYHeight(font.getFontID()) + font.getLineHeight() * getGlobalFontYStandardLineHeight(font.getFontID()))) * getGlobalFontScalar(font.getFontID()) * font.getSize() - font.getAlignment().mHorizonOffset; break;
			default:
				y = rect.top() + 0.5f * (rect.mHeight - (getGlobalFontYHeight(font.getFontID()) + 2 * getGlobalFontYOffset(font.getFontID())) * getGlobalFontScalar(font.getFontID()) * font.getSize()); break;
			}
			return Point(roundf(x), roundf(y));
		}
		CRAFT_ENGINE_GUI_API Point GuiFontSystem::calcFontBasePoint(const String& str, const Rect& rect, const Font& font)
		{
			return calcFontBasePoint(str.data(), str.size(), rect, font);
		}
		CRAFT_ENGINE_GUI_API Point GuiFontSystem::calcFontBasePoint(const StringRef& str, const Rect& rect, const Font& font)
		{
			return GuiFontSystem::calcFontBasePoint(str.data(), str.size(), rect, font);
		}
		CRAFT_ENGINE_GUI_API int GuiFontSystem::calcFontPointerIndex(const Char* pStr, int32_t count, int xcooed, const Font& font)
		{
			int x = 0;
			int interval = font.getInterval();
			float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
			int advance = 0;
			int index = 0;
			for (int i = 0; i < count; i++)
			{
				auto slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
				advance = step_advance(slot.xadvance, scalar);//
				if (xcooed < x + advance / 2)
					break;
				else if (xcooed < x + advance)
				{
					index++;
					break;
				}
				else
				{
					x += advance + interval;
					index++;
				}
			}
			return index;
		}
		CRAFT_ENGINE_GUI_API int GuiFontSystem::calcFontPointerIndex(const String& str, int xcooed, const Font& font)
		{
			return calcFontPointerIndex(str.data(), str.size(), xcooed, font);
		}
		CRAFT_ENGINE_GUI_API int GuiFontSystem::calcFontPointerIndex(const StringRef& str, int xcooed, const Font& font)
		{
			return calcFontPointerIndex(str.data(), str.size(), xcooed, font);
		}

		CRAFT_ENGINE_GUI_API Rect GuiFontSystem::calcFontCursorRect(const Char* pStr, int32_t count, Point basepoint, const Font& font)
		{
			auto w = GuiFontSystem::calcFontLineWidth(pStr, count, font);
			auto scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
			Rect rect;
			rect.mX = basepoint.x + w + font.getInterval() / 2;
			rect.mY = roundf(basepoint.y + scalar * (GuiFontSystem::getGlobalFontYOffset(font.getFontID()) + GuiFontSystem::getGlobalFontYHeight(font.getFontID()) / 2 - font.getLineHeight() * GuiFontSystem::getGlobalFontYStandardLineHeight(font.getFontID()) / 2));
			rect.mWidth = 1;
			rect.mHeight = roundf(scalar * font.getLineHeight() * GuiFontSystem::getGlobalFontYStandardLineHeight(font.getFontID()));
			return rect;
		}

		CRAFT_ENGINE_GUI_API Rect GuiFontSystem::calcFontCursorRect(const String& str, Point basepoint, const Font& font)
		{
			return calcFontCursorRect(str.data(), str.size(), basepoint, font);
		}

		CRAFT_ENGINE_GUI_API Rect GuiFontSystem::calcFontCursorRect(const StringRef& str, Point basepoint, const Font& font)
		{
			return calcFontCursorRect(str.data(), str.size(), basepoint, font);
		}

		CRAFT_ENGINE_GUI_API void GuiColorStyle::init(ColorStyle color_style)
		{
			cur_color_style = color_style;
			StyleData style;
			switch (color_style)
			{
			case CraftEngine::gui::GuiColorStyle::eLight:
				style.colors[0] = Color(255, 255, 255, 200);
				style.colors[1] = Color(245, 245, 245, 200);
				style.colors[2] = Color(225, 225, 225, 200);
				style.colors[3] = Color(200, 200, 200, 200);
				style.colors[4] = Color(175, 175, 175, 200);
				style.colors[5] = Color(150, 150, 150, 200);
				style.colors[6] = Color(135, 135, 135, 200);
				style.colors[7] = Color(120, 120, 120, 200); //
				style.colors[8] = Color(105, 105, 105, 200);
				style.colors[9] = Color(90, 90, 90, 200);
				style.colors[10] = Color(75, 75, 75, 200);
				style.colors[11] = Color(60, 60, 60, 200);
				style.colors[12] = Color(45, 45, 45, 200);
				style.colors[13] = Color(30, 30, 30, 200);
				style.colors[14] = Color(15, 15, 15, 200);
				style.colors[15] = Color(0, 0, 0, 200);
				break;
			case CraftEngine::gui::GuiColorStyle::eDark:
				style.colors[15] = Color(255, 255, 255, 200);
				style.colors[14] = Color(245, 245, 245, 200);
				style.colors[13] = Color(225, 225, 225, 200);
				style.colors[12] = Color(200, 200, 200, 200);
				style.colors[11] = Color(175, 175, 175, 200);
				style.colors[10] = Color(150, 150, 150, 200);
				style.colors[9] = Color(135, 135, 135, 200);
				style.colors[8] = Color(120, 120, 120, 200); //
				style.colors[7] = Color(105, 105, 105, 200);
				style.colors[6] = Color(90, 90, 90, 200);
				style.colors[5] = Color(75, 75, 75, 200);
				style.colors[4] = Color(60, 60, 60, 200);
				style.colors[3] = Color(45, 45, 45, 200);
				style.colors[2] = Color(30, 30, 30, 200);
				style.colors[1] = Color(15, 15, 15, 200);
				style.colors[0] = Color(0, 0, 0, 200);
				break;
			case CraftEngine::gui::GuiColorStyle::eRed:
				style.colors[0] = Color(255, 255, 255, 225);
				style.colors[1] = Color(245, 230, 230, 225);
				style.colors[2] = Color(245, 225, 225, 225);
				style.colors[3] = Color(245, 200, 200, 225);
				style.colors[4] = Color(245, 175, 175, 225);
				style.colors[5] = Color(245, 150, 150, 225);
				style.colors[6] = Color(245, 135, 135, 255);
				style.colors[7] = Color(245, 120, 120, 225); //
				style.colors[8] = Color(200, 105, 105, 225);
				style.colors[9] = Color(150, 90, 90, 225);
				style.colors[10] = Color(120, 75, 75, 225);
				style.colors[11] = Color(90, 60, 60, 255);
				style.colors[12] = Color(55, 45, 45, 225);
				style.colors[13] = Color(30, 30, 30, 225);
				style.colors[14] = Color(15, 15, 15, 225);
				style.colors[15] = Color(0, 0, 0, 225);
				break;
			default:
				break;
			}
			_Init(style);
		}

		CRAFT_ENGINE_GUI_API void GuiColorStyle::_Init(const StyleData& style)
		{
			foreground_color = style[15];
			background_color = style[0];
			//auto& palette;
			{
				auto& palette = color_style[eWidget];
				palette.mFillMode = Palette::eFillMode_None;
				palette.mFrameMode = Palette::eFrameMode_None;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[1];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15];
				palette.mForegroundHighLightColor = style[15];
				palette.mForegroundFocusColor = style[0];
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				// same as widget
				auto& palette = color_style[eLabel];
				palette.mFillMode = Palette::eFillMode_None;
				palette.mFrameMode = Palette::eFrameMode_None;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[1];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15];
				palette.mForegroundHighLightColor = style[15];
				palette.mForegroundFocusColor = style[0];
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[ePushButton];
				palette.mFillMode = Palette::eFillMode_Color; //
				palette.mFrameMode = Palette::eFrameMode_None;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[2];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15];
				palette.mForegroundHighLightColor = style[15];
				palette.mForegroundFocusColor = style[0];
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eTextInput];
				palette.mFillMode = Palette::eFillMode_Color; // 
				palette.mFrameMode = Palette::eFrameMode_Surround; //
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[0];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[0];
				palette.mForegroundColor = style[15]; // *
				palette.mForegroundHighLightColor = style[15]; // 
				palette.mForegroundFocusColor = style[15]; // 
				palette.mForegroundDisableColor = style[9];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eSlider];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_None;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[5];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15]; // *
				palette.mForegroundHighLightColor = style[15]; // 
				palette.mForegroundFocusColor = style[0]; // 
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eSliderBar];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_Surround;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[2];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15]; // *
				palette.mForegroundHighLightColor = style[15]; // 
				palette.mForegroundFocusColor = style[0]; // 
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eSeekBar];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_Surround;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[2];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15]; // *
				palette.mForegroundHighLightColor = style[15]; // 
				palette.mForegroundFocusColor = style[0]; // 
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eScorllBar];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_Surround;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[2];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15]; // *
				palette.mForegroundHighLightColor = style[15]; // 
				palette.mForegroundFocusColor = style[0]; // 
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eProgressBar];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_Surround;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[2];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15]; // *
				palette.mForegroundHighLightColor = style[15]; // 
				palette.mForegroundFocusColor = style[0]; // 
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eItemView];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_Surround;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = Color(0);
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[5];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15];
				palette.mForegroundHighLightColor = style[15];
				palette.mForegroundFocusColor = style[0];
				palette.mForegroundDisableColor = style[4];
				palette.mFrameColor = style[7];
			}
			{
				auto& palette = color_style[eMenu];
				palette.mFillMode = Palette::eFillMode_Color;
				palette.mFrameMode = Palette::eFrameMode_Surround;
				palette.mBackgroundImage = HandleImage(nullptr);
				palette.mBackgroundColor = style[2];
				palette.mBackgroundDisableColor = style[3];
				palette.mBackgroundHighLightColor = style[4];
				palette.mBackgroundFocusColor = style[6];
				palette.mForegroundColor = style[15];
				palette.mForegroundHighLightColor = style[15];
				palette.mForegroundFocusColor = style[0];
				palette.mForegroundDisableColor = style[6];
				palette.mFrameColor = style[7];
			}

			
		}



		namespace _Gui_Api_Detail
		{

			CRAFT_ENGINE_STATIC core::ArrayList<Rect> calcFlowLayout(const core::ArrayList<Size>& items, const Size& area, const Layout::FlowLayoutInfo& info)
			{
				core::ArrayList<Rect> itemLayout;
				if (items.size() == 0)
					return itemLayout;
				const int areaWidth = area.x, areaHeight = area.y;
				struct LineInfo
				{
					int indexStart;
					int indexEnd;
					int width;
					int height;
					int coordInternal;
					int coordTotal;
				};
				CraftEngine::core::ArrayList<LineInfo> lines;
				lines.resize(1);
				lines[0].indexStart = 0;
				lines[0].indexEnd = 0;
				lines[0].height = 0;
				lines[0].width = 0;
				int curLineIndex = 0;

				if (info.baseOnRow)
				{
					int accYCoord = 0;
					for (int i = 0; i < items.size(); i++)
					{
						if (lines[curLineIndex].width + items[i].x > areaWidth)
						{
							lines[curLineIndex].indexEnd = CraftEngine::math::max(i - 1, lines[curLineIndex].indexStart);
							lines[curLineIndex].coordInternal =
								CraftEngine::math::max(areaWidth - lines[curLineIndex].width, 0) /
								(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
							lines[curLineIndex].coordTotal = lines[curLineIndex].width +
								lines[curLineIndex].coordInternal *
								(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
							accYCoord += lines[curLineIndex].height;
							curLineIndex++;
							lines.resize(curLineIndex + 1);
							lines[curLineIndex].indexStart = i;
							lines[curLineIndex].indexEnd = i;
							lines[curLineIndex].height = 0;
							lines[curLineIndex].width = 0;
						}
						lines[curLineIndex].width += items[i].x;
						lines[curLineIndex].height = CraftEngine::math::max(items[i].y, lines[curLineIndex].height);
					}
					lines[curLineIndex].indexEnd = CraftEngine::math::max(int(items.size() - 1), lines[curLineIndex].indexStart);
					lines[curLineIndex].coordInternal = CraftEngine::math::max(areaWidth - lines[curLineIndex].width, 0) /
						(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
					lines[curLineIndex].coordTotal = lines[curLineIndex].width +
						lines[curLineIndex].coordInternal *
						(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
					accYCoord += lines[curLineIndex].height;
					int internalY = CraftEngine::math::max(areaHeight - accYCoord, 0) / (lines.size() + 1);
					int totalY = accYCoord + internalY * (lines.size() + 1);
					int globalOffsetY = (areaHeight - totalY) / 2;
					if (globalOffsetY < 0)
					{
						if (info.alignBorderMin > 0)
							globalOffsetY = 0;
						else if (info.alignBorderMin < 0)
							globalOffsetY = areaHeight - totalY;
					}
					int curCoordY = 0;
					int curLineAccX = 0;
					for (int i = 0; i < lines.size(); i++)
					{
						Offset offset = Offset((areaWidth - lines[i].coordTotal) / 2, globalOffsetY + curCoordY + internalY * (i + 1));
						for (int j = lines[i].indexStart, k = 1; j <= lines[i].indexEnd; j++, k++)
						{
							Offset pos = offset + Offset(curLineAccX + k * lines[i].coordInternal, (lines[i].height - items[i].y) / 2);
							itemLayout.push_back(Rect(pos, items[i]));
							curLineAccX += items[i].x;
						}
						curLineAccX = 0;
						curCoordY += lines[i].height;
					}
				}
				else
				{
					int accXCoord = 0;
					for (int i = 0; i < items.size(); i++)
					{
						if (lines[curLineIndex].height + items[i].y > areaHeight)
						{
							lines[curLineIndex].indexEnd = CraftEngine::math::max(i - 1, lines[curLineIndex].indexStart);
							lines[curLineIndex].coordInternal =
								CraftEngine::math::max(areaHeight - lines[curLineIndex].height, 0) /
								(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
							lines[curLineIndex].coordTotal = lines[curLineIndex].height +
								lines[curLineIndex].coordInternal *
								(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
							accXCoord += lines[curLineIndex].width;
							curLineIndex++;
							lines.resize(curLineIndex + 1);
							lines[curLineIndex].indexStart = i;
							lines[curLineIndex].indexEnd = i;
							lines[curLineIndex].height = 0;
							lines[curLineIndex].width = 0;
						}
						lines[curLineIndex].height += items[i].y;
						lines[curLineIndex].width = CraftEngine::math::max(items[i].x, lines[curLineIndex].width);
					}
					lines[curLineIndex].indexEnd = CraftEngine::math::max(int(items.size() - 1), lines[curLineIndex].indexStart);
					lines[curLineIndex].coordInternal = CraftEngine::math::max(areaHeight - lines[curLineIndex].height, 0) /
						(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
					lines[curLineIndex].coordTotal = lines[curLineIndex].height +
						lines[curLineIndex].coordInternal *
						(lines[curLineIndex].indexEnd - lines[curLineIndex].indexStart + 2);
					accXCoord += lines[curLineIndex].width;
					int internalX = CraftEngine::math::max(areaWidth - accXCoord, 0) / (lines.size() + 1);
					int totalX = accXCoord + internalX * (lines.size() + 1);
					int globalOffsetX = (areaWidth - totalX) / 2;
					if (globalOffsetX < 0)
					{
						if (info.alignBorderMin > 0)
							globalOffsetX = 0;
						else if (info.alignBorderMin < 0)
							globalOffsetX = areaWidth - totalX;
					}
					int curCoordX = 0;
					int curLineAccY = 0;
					for (int i = 0; i < lines.size(); i++)
					{
						Offset offset = Offset(globalOffsetX + curCoordX + internalX * (i + 1), (areaHeight - lines[i].coordTotal) / 2);
						for (int j = lines[i].indexStart, k = 1; j <= lines[i].indexEnd; j++, k++)
						{
							Offset pos = offset + Offset((lines[i].width - items[i].x) / 2, curLineAccY + k * lines[i].coordInternal);
							itemLayout.push_back(Rect(pos, items[i]));
							curLineAccY += items[i].y;
						}
						curLineAccY = 0;
						curCoordX += lines[i].width;
					}
				}
				return itemLayout;
			}

			CRAFT_ENGINE_STATIC core::ArrayList<Rect> calcRowLayout(const core::ArrayList<Size>& items, const Size& area, const Layout::RowLayoutInfo& info)
			{
				core::ArrayList<Rect> itemLayout;
				if (items.size() == 0)
					return itemLayout;
				int accCoordY = 0;
				for (auto it : items)
					accCoordY += it.y;
				int internalY = CraftEngine::math::max(area.y - accCoordY, 0) / (items.size() + 1);
				if (!info.allowFlow)
					internalY = 0;
				int totalY = accCoordY + internalY * (items.size() + 1);
				int offsetY = 0;
				if (info.alignBorderMin == 0)
					offsetY = (area.y - totalY) / 2;
				else if (info.alignBorderMin < 0)
					offsetY = area.y - totalY;
				int yAcc = offsetY;
				for (int i = 0; i < items.size(); i++)
				{
					yAcc += internalY;
					itemLayout.push_back(Rect(Point(0, yAcc), Size(area.x, items[i].y)));
					yAcc += items[i].y;
				}
				return itemLayout;
			}

			CRAFT_ENGINE_STATIC core::ArrayList<Rect> calcColLayout(const core::ArrayList<Size>& items, const Size& area, const Layout::ColLayoutInfo& info)
			{
				core::ArrayList<Rect> itemLayout;
				if (items.size() == 0)
					return itemLayout;
				int accCoordX = 0;
				for (auto it : items)
					accCoordX += it.x;
				int internalX = CraftEngine::math::max(area.x - accCoordX, 0) / (items.size() + 1);
				if (!info.allowFlow)
					internalX = 0;
				int totalX = accCoordX + internalX * (items.size() + 1);
				int offsetX = 0;
				if (info.alignBorderMin == 0)
					offsetX = (area.x - totalX) / 2;
				else if (info.alignBorderMin < 0)
					offsetX = area.x - totalX;
				int xAcc = offsetX;
				for (int i = 0; i < items.size(); i++)
				{
					xAcc += internalX;
					itemLayout.push_back(Rect(Point(xAcc, 0), Size(items[i].x, area.y)));
					xAcc += items[i].x;
				}
				return itemLayout;
			}

			CRAFT_ENGINE_STATIC core::ArrayList<Rect> calcGridLayout(const core::ArrayList<Size>& items, const Size& area, const Layout::GridLayoutInfo& info)
			{
				core::ArrayList<Rect> itemLayout;
				core::ArrayList<Size> normalizedItems(items.size());
				Size gridSize = area / Size(info.rowCount, info.colCount);






				return itemLayout;
			}


		}



		CRAFT_ENGINE_GUI_API core::ArrayList<Rect> Layout::calcLayout(const core::ArrayList<Size>& items, const Size& area)
		{
			core::ArrayList<Rect> itemLayoutList;
			switch (mode)
			{
			case CraftEngine::gui::Layout::LayoutMode::eCustomLayout:
				break;
			case CraftEngine::gui::Layout::LayoutMode::eFlowLayout:
				itemLayoutList = _Gui_Api_Detail::calcFlowLayout(items, area, info.flowInfo);
				break;
			case CraftEngine::gui::Layout::LayoutMode::eRowLayout:
				itemLayoutList = _Gui_Api_Detail::calcRowLayout(items, area, info.rowInfo);
				break;
			case CraftEngine::gui::Layout::LayoutMode::eColLayout:
				itemLayoutList = _Gui_Api_Detail::calcColLayout(items, area, info.colInfo);
				break;
			case CraftEngine::gui::Layout::LayoutMode::eGridLayout:
				itemLayoutList = _Gui_Api_Detail::calcGridLayout(items, area, info.gridInfo);
				break;
			default:
				break;
			}
			return itemLayoutList;
		}



	}
}





#include "./platforms/assets.inl"

#endif // !CRAFT_ENGINE_GUI_COMMON_H_