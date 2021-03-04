#pragma once

#include "../widgets/Widget.h"
#include <imgui.h>

namespace CraftEngine
{
	namespace gui
	{
		namespace imgui
		{
			bool imguiIsInitialized = false;
			HandleImage imguiFontImage = HandleImage(nullptr);

			void ImGui_ImplOpenGL3_CreateDeviceObjects()
			{
				ImGuiIO& io = ImGui::GetIO();
				unsigned char* pixels;
				int width, height;
				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);  
				auto img = GuiRenderSystem::createImage(pixels, 4 * width * height, width, height);
				io.Fonts->TexID = img.unused;
                imguiFontImage = img;
				imguiIsInitialized = true;
			}

			void ImGui_ImplOpenGL3_DestroyDeviceObjects()
			{
				if (imguiFontImage != nullptr)
				{
					GuiRenderSystem::deleteImage(imguiFontImage);
					imguiFontImage = HandleImage(nullptr);
				}
				imguiIsInitialized = false;
			}

			IMGUI_IMPL_API void ImGui_ImplCraftEngine_Renderer_Init()
			{
				ImGuiIO& io = ImGui::GetIO();
                io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
                io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
                io.BackendPlatformName = "imgui_impl_craft_engine";
                io.BackendRendererName = "imgui_impl_craft_engine_gui_renderer";

                io.KeyMap[ImGuiKey_Tab] = KeyBoard::eKey_Tab;
                io.KeyMap[ImGuiKey_LeftArrow] = KeyBoard::eKey_Left;
                io.KeyMap[ImGuiKey_RightArrow] = KeyBoard::eKey_Right;
                io.KeyMap[ImGuiKey_UpArrow] = KeyBoard::eKey_Up;;
                io.KeyMap[ImGuiKey_DownArrow] = KeyBoard::eKey_Down;;
                //io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
                //io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
                io.KeyMap[ImGuiKey_Home] = KeyBoard::eKey_Home;
                io.KeyMap[ImGuiKey_End] = KeyBoard::eKey_End;
                io.KeyMap[ImGuiKey_Insert] = KeyBoard::eKey_Insert;
                io.KeyMap[ImGuiKey_Delete] = KeyBoard::eKey_Delete;
                io.KeyMap[ImGuiKey_Backspace] = KeyBoard::eKey_Back;
                io.KeyMap[ImGuiKey_Space] = KeyBoard::eKey_Space;
                io.KeyMap[ImGuiKey_Enter] = KeyBoard::eKey_Return;
                io.KeyMap[ImGuiKey_Escape] = KeyBoard::eKey_Escape;
                //io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
                io.KeyMap[ImGuiKey_A] = L'A';
                io.KeyMap[ImGuiKey_C] = L'C';
                io.KeyMap[ImGuiKey_V] = L'V';
                io.KeyMap[ImGuiKey_X] = L'X';
                io.KeyMap[ImGuiKey_Y] = L'Y';
                io.KeyMap[ImGuiKey_Z] = L'Z';
			}


			IMGUI_IMPL_API void ImGui_ImplCraftEngine_Renderer_Shutdown()
			{
				ImGui_ImplOpenGL3_DestroyDeviceObjects();
			}


			IMGUI_IMPL_API void ImGui_ImplCraftEngine_Renderer_NewFrame()
			{
				if (!imguiIsInitialized)
					ImGui_ImplOpenGL3_CreateDeviceObjects();
			}



		}
	}
}





namespace CraftEngine
{
	namespace gui
	{

		class ImguiWidget : public Widget
		{
        private:
            ImGuiContext* m_imguiCtx = nullptr;

		public:
			craft_engine_gui_signal_v2(imguiPainting, void(void));

			ImguiWidget(Widget* parent) : Widget(parent)
			{
                setDragable(true);
                setCharAcceptable(true);
                m_imguiCtx = ImGui::CreateContext();
                ImGui::SetCurrentContext(m_imguiCtx);
                imgui::ImGui_ImplCraftEngine_Renderer_Init();
				ImGui::StyleColorsDark();
				imgui::ImGui_ImplCraftEngine_Renderer_NewFrame();  
                ImGui::SetCurrentContext(nullptr);
			}

			~ImguiWidget()
			{
				imgui::ImGui_ImplCraftEngine_Renderer_Shutdown();
				ImGui::DestroyContext(m_imguiCtx);
			}

			virtual void onPaintEvent() override
			{
                ImGui::SetCurrentContext(m_imguiCtx);
                using vec2 = math::vec2;
                ImGuiIO& io = ImGui::GetIO();
                IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer backend. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");
                int w, h;
                int display_w, display_h;
                display_w = w = getWidth();
                display_h = h = getHeight();
                io.DisplaySize = ImVec2((float)w, (float)h);
                if (w > 0 && h > 0)
                    io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
                io.DeltaTime = (float)(1.0f / 60.0f);

				ImGui::NewFrame();
				craft_engine_gui_emit(imguiPainting, );
				ImGui::Render();

                auto painter = getPainter();
                {
                    auto draw_data = ImGui::GetDrawData();
                    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
                    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
                    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
                    if (fb_width <= 0 || fb_height <= 0)
                        return;

                    // Will project scissor/clipping rectangles into framebuffer space
                    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
                    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

                    // Render command lists
                    for (int n = 0; n < draw_data->CmdListsCount; n++)
                    {
                        const ImDrawList* cmd_list = draw_data->CmdLists[n];
                        uint32_t* idx_buf = new uint32_t[cmd_list->IdxBuffer.Size];
                        for (int i = 0; i < cmd_list->IdxBuffer.Size; i++)
                            idx_buf[i] = cmd_list->IdxBuffer.Data[i];
                        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
                        {
                            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                            if (pcmd->UserCallback != NULL)
                            {

                            }
                            else
                            {
                                ImVec4 clip_rect;
                                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;
                                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                                {
                                    auto painter = getPainter();
                                    PolygonInfo info{};
                                    info.image = HandleImage(pcmd->TextureId);
                                    info.mixBuffer = (PolygonInfo::VertexData*)cmd_list->VtxBuffer.Data;
                                    info.indexList = &idx_buf[pcmd->IdxOffset];
                                    info.indexCount = pcmd->ElemCount;

                                    int max_vertex = 0;
                                    for (int i = 0; i < pcmd->ElemCount; i++)
                                    {
                                        if (info.indexList[i] > max_vertex)
                                            max_vertex = info.indexList[i];
                                    }
                                    info.vertexCount = max_vertex;
                                    info.vertexCount = cmd_list->VtxBuffer.Size;
                                    Rect scissor = Rect((int)clip_rect.x, (int)(clip_rect.y), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                                    painter.setScissor(scissor);
                                    painter.drawPolygonEx(info);
                                }
                            }
                        }
                        delete[] idx_buf;
                    }
                }
                ImGui::SetCurrentContext(nullptr);
			}


            virtual void onPressed(const MouseEvent& mouseEvent) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                // Update buttons
                ImGuiIO& io = ImGui::GetIO();
                switch (mouseEvent.button)
                {
                case MouseEvent::eLeftButton:
                    io.MouseDown[0] = true;
                    break;
                case MouseEvent::eRightButton:
                    io.MouseDown[1] = true;
                    break;
                }
                // Update mouse position
                const ImVec2 mouse_pos_backup = io.MousePos;
                io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
                if (io.WantSetMousePos)
                {
                    /*    glfwSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);*/
                }
                else
                {
                    auto local_pos = getLocalPos(mouseEvent.global);
                    io.MousePos = ImVec2((float)local_pos.x, (float)local_pos.y);
                }
                ImGui::SetCurrentContext(nullptr);
            }
            virtual void onReleased(const MouseEvent& mouseEvent) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                // Update buttons
                ImGuiIO& io = ImGui::GetIO();
                switch (mouseEvent.button)
                {
                case MouseEvent::eLeftButton:
                    io.MouseDown[0] = false;
                    break;
                case MouseEvent::eRightButton:
                    io.MouseDown[1] = false;
                    break;
                }

                // Update mouse position
                const ImVec2 mouse_pos_backup = io.MousePos;
                io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
                if (io.WantSetMousePos)
                {
                    /*    glfwSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);*/
                }
                else
                {
                    auto local_pos = getLocalPos(mouseEvent.global);
                    io.MousePos = ImVec2((float)local_pos.x, (float)local_pos.y);
                }
                ImGui::SetCurrentContext(nullptr);
            }
            virtual void onMoving(const MouseEvent& mouseEvent) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                ImGuiIO& io = ImGui::GetIO();
                // Update mouse position
                const ImVec2 mouse_pos_backup = io.MousePos;
                io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
                if (io.WantSetMousePos)
                {
                    /*    glfwSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);*/
                }
                else
                {
                    auto local_pos = getLocalPos(mouseEvent.global);
                    io.MousePos = ImVec2((float)local_pos.x, (float)local_pos.y);
                }
                ImGui::SetCurrentContext(nullptr);
            }
            virtual void onMoveIn(const MouseEvent& mouseEvent) override final
            {

            }
            virtual void onMoveOut(const MouseEvent& mouseEvent) override final
            {

            }
            virtual void onDrag(const MouseEvent& mouseEvent) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                ImGuiIO& io = ImGui::GetIO();
                // Update mouse position
                const ImVec2 mouse_pos_backup = io.MousePos;
                io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
                if (io.WantSetMousePos)
                {
                    /*    glfwSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);*/
                }
                else
                {
                    auto local_pos = getLocalPos(mouseEvent.global) + mouseEvent.offset;
                    io.MousePos = ImVec2((float)local_pos.x, (float)local_pos.y);
                }
                ImGui::SetCurrentContext(nullptr);
            }
            virtual void onWheel(const MouseEvent& mouseEvent) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                ImGuiIO& io = ImGui::GetIO();
                io.MouseWheelH += (float)math::sign(mouseEvent.offset.x);
                io.MouseWheel += (float)math::sign(mouseEvent.offset.y);
                ImGui::SetCurrentContext(nullptr);
            }

            virtual void onKeyBoardEvent(const KeyboardEvent& keyboardEvent) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                ImGuiIO& io = ImGui::GetIO();
                io.KeysDown[keyboardEvent.key] = keyboardEvent.down;

                // Modifiers are not reliable across systems
                io.KeyCtrl = io.KeysDown[KeyBoard::eKey_Control];
                io.KeyShift = io.KeysDown[KeyBoard::eKey_Shift];
                //io.KeyAlt = io.KeysDown[KeyBoard::eKey_Alt] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
                io.KeySuper = false;
#else
                //io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
                ImGui::SetCurrentContext(nullptr);
            }

            virtual void onAcceptChar(Char c) override final
            {
                ImGui::SetCurrentContext(m_imguiCtx);
                ImGuiIO& io = ImGui::GetIO();
                io.AddInputCharacter(c);
                ImGui::SetCurrentContext(nullptr);
            }

		};

	}
}





