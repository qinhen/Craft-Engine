#pragma once
#include "./s3d_gui_renderer.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _Soft3D_Render_System_Detail
		{



			class Soft3DGuiCanvasRenderer : public AbstractSoft3DGuiRenderer
			{
			public:


				virtual void begin(void* userData)
				{
					
				}


				virtual void end(void* userData)
				{

				}
			};

			

			class Soft3DGuiCanvas : public AbstractGuiCanvas
			{
			private:
			
			public:

				void init()
				{

				}
				void clear()
				{

				}

				virtual Size getCanvasSize() override
				{
					return Size();
				}
				virtual void setCanvasSize(const Size& size) override
				{

				}
				virtual void setCurrentFramebufferIndex(int32_t idx) override
				{

				}
				virtual uint32_t getCurrentFramebufferIndex() const override
				{
					return 0;
				}
				virtual HandleImage getFramebufferImage(uint32_t attachmentIdx = 0, uint32_t framebufferIdx = 0)  override
				{
					return HandleImage(nullptr);
				}
				virtual uint32_t getFramebufferCount() const  override
				{
					return 1;
				}
				virtual uint32_t getAttachmentCount() const override
				{
					return 1;
				}
				virtual AbstractGuiRenderer* getGuiRenderer() override
				{
					return nullptr;
				}
			};

		}

	};



}
