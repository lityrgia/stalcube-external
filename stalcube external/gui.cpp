#include "gui.h"

#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#include "ext/imgui/imgui.h"

#include "CEntity.h"
#include "options.h"
#include "fonts.hpp"
#include "util.h"
#include "types.h"
#include "cheat.h"

#define HEX_TO_IMCOLOR(hex) ImColor( \
    ((hex >> 16) & 0xFF) / 255.0f,  \
    ((hex >> 8) & 0xFF) / 255.0f,   \
    (hex & 0xFF) / 255.0f           \
)

#define HEX_TO_IMCOLOR_A(hex, a) ImColor( \
    ((hex >> 16) & 0xFF) / 255.0f,         \
    ((hex >> 8) & 0xFF) / 255.0f,          \
    (hex & 0xFF) / 255.0f,                 \
    (a) / 255.0f                           \
)

namespace colors {
    static constexpr ImColor gui_start(HEX_TO_IMCOLOR(0x0A0A12));
    static constexpr ImColor gui_mid(HEX_TO_IMCOLOR(0x0E1A30));
    static constexpr ImColor gui_end(HEX_TO_IMCOLOR(0x162B4D));
    static constexpr ImColor hover_btn_start(HEX_TO_IMCOLOR(0x3B82F6));
    static constexpr ImColor hover_btn_end(HEX_TO_IMCOLOR(0x1D4ED8));
    static constexpr ImColor child_bg(HEX_TO_IMCOLOR_A(0x0066FF, 10));
}

void setImguiStyle() {
    auto &io = ImGui::GetIO();

    auto& colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_ButtonHovered] = HEX_TO_IMCOLOR(0x272727);
    colors[ImGuiCol_Border] = colors::gui_end;
    colors[ImGuiCol_WindowBg] = ImColor(30, 30, 30, 255);

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.FramePadding = ImVec2(0, 0);
    style.WindowPadding = ImVec2(0, 0);
    style.ItemSpacing = ImVec2(0, 0);
}

void drawLine(ImColor color, float thickness, std::string direction) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 lineStart;
    ImVec2 lineEnd;

    if (direction == "up") {
        lineStart = ImVec2(windowPos.x, windowPos.y);
        lineEnd = ImVec2(windowPos.x + windowSize.x - 1, windowPos.y);
    }
    else if (direction == "down") {
        lineStart = ImVec2(windowPos.x, windowPos.y + windowSize.y);
        lineEnd = ImVec2(windowPos.x + windowSize.x - 1, windowPos.y + windowSize.y);
    }
    else if (direction == "right") {
        lineStart = ImVec2(windowPos.x + windowSize.x - 1, windowPos.y);
        lineEnd = ImVec2(windowPos.x + windowSize.x - 1, windowPos.y + windowSize.y);
    }
    else if (direction == "left") {
        lineStart = ImVec2(windowPos.x, windowPos.y);
        lineEnd = ImVec2(windowPos.x, windowPos.y + windowSize.y);
    }
    else {
        return;
    }

    drawList->AddLine(lineStart, lineEnd, color, thickness);
}

void showColorPickerButton(ImVec4* color, int id) {
    ImGui::PushID(id);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 1));

    if (ImGui::ColorButton("ColorButton", *color)) {
        ImGui::OpenPopup("ColorPickerPopup");
    }

    if (ImGui::BeginPopup("ColorPickerPopup")) {
        ImGui::ColorPicker4("##picker", (float*)color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
}

void centeredText(const char* text) {
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float x = (windowSize.x - textSize.x) * 0.5f;
    ImGui::SetCursorPosX(x);
    ImGui::Text("%s", text);

    ImGui::PopStyleVar(3);
}

void centeredTextV(const char* text) {
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 textSize = ImGui::CalcTextSize(text);

    float x = (windowSize.x - textSize.x) * 0.5f;
    float y = (windowSize.y - textSize.y) * 0.5f;

    ImGui::SetCursorPos(ImVec2(x, y));
    ImGui::Text("%s", text);

    ImGui::PopStyleVar(3);
}

bool toggleButton(const char* str_id, bool* v) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;

    ImU32 col_bg = *v ? IM_COL32(255, 140, 0, 255) : IM_COL32(255, 255, 255, 255);
    ImU32 circle_bg = *v ? IM_COL32(255, 255, 255, 255) : IM_COL32(16, 24, 32, 255);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, circle_bg);

    return *v;
}

void colorPickerButton(ImVec4* color, int id) {
    ImGui::PushID(id);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 1));

    if (ImGui::ColorButton("ColorButton", *color)) {
        ImGui::OpenPopup("ColorPickerPopup");
    }

    if (ImGui::BeginPopup("ColorPickerPopup")) {
        ImGui::ColorPicker4("##picker", (float*)color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
}

std::string GetKeyName(int vk) {
    UINT scanCode = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
    char name[128] = { 0 };
    if (GetKeyNameTextA(scanCode << 16, name, sizeof(name)))
        return std::string(name);
    return "Unknown";
}

bool keyBindButton(const char* id, int& selectedKey) {
    static const char* waitingID = nullptr;

    std::string buttonText;
    if (waitingID == id)
        buttonText = " ... ";
    else if (selectedKey != 0)
        buttonText = " " + GetKeyName(selectedKey) + " ";
    else
        buttonText = " n/a ";

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(24 / 255.f, 43 / 255.f, 77 / 255.f, 200 / 255.f));
    if (ImGui::Button(buttonText.c_str())) {
        waitingID = id;
    }
    ImGui::PopStyleColor();

    if (waitingID == id) {
        for (int key = 0x01; key <= 0xFE; ++key) {
            if (GetAsyncKeyState(key) & 0x8000) {
                selectedKey = key;
                waitingID = nullptr;
                break;
            }
        }
    }

    return false;
}


inline ImFont* largeFont = nullptr;
inline ImFont* regularFont = nullptr;
inline ImFont* iconsFont = nullptr;

void setupFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    static const ImWchar* glyphRanges = io.Fonts->GetGlyphRangesCyrillic();

    regularFont = io.Fonts->AddFontFromMemoryTTF((void*)montserrat_Regular, sizeof(montserrat_Regular), 18.f, nullptr, glyphRanges);

    largeFont = io.Fonts->AddFontFromMemoryTTF((void*)montserrat_Regular, sizeof(montserrat_Regular), 21.f);

    static const ImWchar iconRanges[]{ ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig iconsConfig;

    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.OversampleH = 3;
    iconsConfig.OversampleV = 3;

    iconsFont = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 22.0f, &iconsConfig, iconRanges);
}

static int selectedTab = 0;  // 0: Aim, 1: Render, 2: Credits
enum selected {
    Aim,
    Render,
    Credits
};

bool leftCenteredTextButton(const char* label, const ImVec2& size = ImVec2(0, 0), int id = -1) {
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    ImVec2 buttonSize = size;
    if (buttonSize.x == 0) buttonSize.x = labelSize.x + ImGui::GetStyle().FramePadding.x * 2;
    if (buttonSize.y == 0) buttonSize.y = labelSize.y + ImGui::GetStyle().FramePadding.y * 2;

    ImGui::PushID(label);
    bool pressed = ImGui::InvisibleButton("##button", buttonSize);

    ImU32 color = ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) :
        ImGui::IsItemActive() ? ImGui::GetColorU32(ImGuiCol_ButtonActive) :
        ImGui::GetColorU32(ImGuiCol_Button);

    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetItemRectMin();
    ImVec2 end = ImGui::GetItemRectMax();
    drawList->AddRectFilled(pos, end, color, ImGui::GetStyle().FrameRounding);

    float textY = pos.y + (buttonSize.y - labelSize.y) * 0.5f;
    float textX = pos.x + ImGui::GetStyle().FramePadding.x;

    drawList->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), label);

    if (id != -1 && selectedTab == id) {
        float rectWidth = 8.0f;
        float rectHeight = 29.0f;
        float padding = ImGui::GetStyle().FramePadding.x;

        ImVec2 rectMax = ImVec2(end.x - padding - 20.f, pos.y + (buttonSize.y + rectHeight) * 0.5f);
        ImVec2 rectMin = ImVec2(rectMax.x - rectWidth, rectMax.y - rectHeight);

        drawList->AddRectFilledMultiColor(
            rectMin,
            rectMax,
            colors::hover_btn_start, colors::hover_btn_start, colors::hover_btn_end, colors::hover_btn_end
        );
    }

    ImGui::PopID();
    return pressed;
}

ImVec2 menuSize(700, 400);
ImVec2 buttonSize(159, 50);
float linesThickness = 1.5f;

ImVec2 smallPadding(0, 10);
ImVec2 defPadding(0, 15);

bool box2dsettings = false;

const char* aimbotButtons[] = { "Auto", "Left button", "Right button" };

void guiStart() {

    ImGui::Begin("Cubus", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowSize(menuSize, ImGuiCond_Always);

    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_size = ImGui::GetWindowSize();

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (options::bPreviewEnabled) {
        ImVec2 espWindowPos = ImVec2(window_pos.x + window_size.x + 40, window_pos.y);

        ImGui::Begin("Esp preview", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowPos(espWindowPos, ImGuiCond_FirstUseEver);
        ImGui::SetWindowSize(ImVec2(230, 400), ImGuiCond_FirstUseEver);

        ImVec2 espFramePos = ImGui::GetWindowPos();
        ImVec2 espFrameSize = ImGui::GetWindowSize();

        if (options::b2DBoxEnabled) {
            ImGui::GetWindowDrawList()->AddRect(ImVec2(espFramePos.x + 20, espFramePos.y + 30), ImVec2(espFramePos.x + espFrameSize.x - 20, espFramePos.y + espFrameSize.y - 20), (ImColor)options::playersColor, 0, 1.4f);
        }

        if (options::bSnaplinesEnabled) {
            ImGui::GetWindowDrawList()->AddLine(ImVec2(espFramePos.x + espFrameSize.x / 2, espFramePos.y + espFrameSize.y), ImVec2(espFramePos.x + espFrameSize.x / 2, espFramePos.y + espFrameSize.y - 20), (ImColor)options::playersColor, 2.f);
        }

        if (options::bHealthEnabled) {
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(espFramePos.x + 20, espFramePos.y + 30), ImVec2(espFramePos.x + 13, espFramePos.y + espFrameSize.y - 20), ImColor(0.0f, 1.0f, 0.0f, 1.f));
        }

        ImGui::End();
    }

    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
        window_pos,
        ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y),
        colors::gui_start, colors::gui_mid, colors::gui_end, colors::gui_start
    );

    ImGui::BeginChild("LeftPane", ImVec2(160, 0), true);
    {
        ImGui::BeginChild("UpLeftPane", ImVec2(0, 70), false);
        {
            drawLine(colors::gui_end, linesThickness, "right");
            ImGui::PushFont(largeFont);
            centeredTextV("Cubus");
            ImGui::PopFont();
        }
        ImGui::EndChild();

        ImGui::BeginChild("DownLeftPane", ImVec2(0, 0), false);
        {
            drawLine(colors::gui_end, linesThickness, "up");
            drawLine(colors::gui_end, linesThickness, "right");

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            ImGui::Dummy(ImVec2(0, 1.f));
            if (leftCenteredTextButton("     Aim", buttonSize, selected::Aim))
                selectedTab = selected::Aim;
            ImGui::Dummy(ImVec2(0, 10.f));

            if (leftCenteredTextButton("     Render", buttonSize, selected::Render))
                selectedTab = selected::Render;
            ImGui::Dummy(ImVec2(0, 10.f));

            if (leftCenteredTextButton("     Credits", buttonSize, selected::Credits))
                selectedTab = selected::Credits;

            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(0, 0), true);
    {
        ImVec2 rightPane_size = ImGui::GetWindowSize();

        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.7f);

        float padding = 10.0f;
        ImGui::SetCursorPos(ImVec2(padding, padding));

        switch (selectedTab)
        {
        case selected::Aim: 
        {
            ImGui::BeginChild("aimbotFrame", ImVec2(rightPane_size.x * 0.5f - 20, rightPane_size.y - 25), true);
            ImGui::Indent(15);

            ImVec2 frame_pos = ImGui::GetWindowPos();
            ImVec2 frame_size = ImGui::GetWindowSize();

            drawList->AddRectFilled(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + frame_size.y),
                colors::child_bg
            );

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Aimbot");
            ImGui::Dummy(ImVec2(0, 25));

            drawList->AddRectFilled(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + 40),
                colors::child_bg
            );

            drawList->AddRect(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + 40),
                colors::gui_end,
                0.0f,
                0,
                1.5f
            );

            ImGui::Checkbox(" Enable ", &options::bAimbotEnabled);
            ImGui::SameLine();
            keyBindButton("##aimbotButt", options::aimbotButton);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Combo("##comboAimingButton", &options::aimTypeButton, aimbotButtons, IM_ARRAYSIZE(aimbotButtons));
            ImGui::SameLine();
            ImGui::Text("  Toggle ");

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Text(" Smoothness");

            ImGui::Dummy(ImVec2(smallPadding));

            ImGui::SliderFloat("##sliderSmooth", &options::smoothness, 1.0f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Draw FOV", &options::bFovEnabled);

            ImGui::Dummy(ImVec2(smallPadding));

            ImGui::SliderFloat("##sliderRadiusFOV", &options::radiusFOV, 0.0f, 300.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::Dummy(ImVec2(10, 0));

            ImGui::SameLine();

            ImGui::BeginChild("triggerbotFrame", ImVec2(rightPane_size.x * 0.5f - 10, rightPane_size.y - 25), true);
            ImGui::Indent(15);

            ImVec2 triggerbotFrame_pos = ImGui::GetWindowPos();
            ImVec2 triggerbotFrame_size = ImGui::GetWindowSize();

            drawList->AddRectFilled(
                triggerbotFrame_pos,
                ImVec2(triggerbotFrame_pos.x + triggerbotFrame_size.x, triggerbotFrame_pos.y + triggerbotFrame_size.y),
                colors::child_bg
            );

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Triggerbot Box");
            ImGui::Dummy(ImVec2(0, 25));

            drawList->AddRectFilled(
                triggerbotFrame_pos,
                ImVec2(triggerbotFrame_pos.x + triggerbotFrame_size.x, triggerbotFrame_pos.y + 40),
                colors::child_bg
            );

            drawList->AddRect(
                triggerbotFrame_pos,
                ImVec2(triggerbotFrame_pos.x + triggerbotFrame_size.x, triggerbotFrame_pos.y + 40),
                colors::gui_end,
                0.0f,
                0,
                1.5f
            );

            ImGui::Checkbox(" Enable ", &options::bTriggerbotEnabled);
            ImGui::SameLine();
            keyBindButton("##triggerbotButt", options::triggerbotButton);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::SliderInt("##triggerbotDelay ", &options::triggerbotDelay, 1, 1000, "%d ms", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SameLine();
            ImGui::Text(" Delay");

            ImGui::EndChild();
            break;
        }

        case selected::Render:
        {
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

            ImGui::BeginChild("renderFrame", ImVec2(rightPane_size.x - 20, rightPane_size.y - 25), true);
            ImGui::Indent(15);

            ImVec2 frame_pos = ImGui::GetWindowPos();
            ImVec2 frame_size = ImGui::GetWindowSize();

            drawList->AddRectFilled(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + frame_size.y),
                colors::child_bg
            );

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("ESP");
            ImGui::Dummy(ImVec2(0, 25));

            drawList->AddRectFilled(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + 40),
                colors::child_bg
            );

            drawList->AddRect(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + 40),
                colors::gui_end,
                0.0f,
                0,
                1.5f
            );

            ImGui::BeginChild("renderFrame_child1", ImVec2(rightPane_size.x / 2 - 35, rightPane_size.y - 94), true);
            ImGui::Indent(15);

            ImGui::Dummy(ImVec2(smallPadding));

            ImGui::Checkbox(" Enable  ", &options::bEspEnabled);
            ImGui::SameLine();
            keyBindButton("##espButt", options::espButton);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Preview", &options::bPreviewEnabled);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Snaplines  ", &options::bSnaplinesEnabled);

            //ImGui::Dummy(ImVec2(defPadding));

            //ImGui::Checkbox(" Team Visible  ", &options::bTeamVisibleEnabled);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Names", &options::bNamesEnabled);

            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(10, 0));
            ImGui::SameLine();

            ImGui::BeginChild("renderFrame_child_", ImVec2(rightPane_size.x / 2 - 35, rightPane_size.y - 94), true);
            ImGui::Indent(15);

            ImGui::Dummy(ImVec2(smallPadding));

            ImGui::Checkbox(" 2D Box  ", &options::b2DBoxEnabled);
            ImGui::SameLine();
            ImGui::PushFont(iconsFont);
            if (ImGui::Button(ICON_FA_COG, ImVec2(20, 20))) box2dsettings = !box2dsettings;
            ImGui::PopFont();
            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            if (box2dsettings) {
                ImGui::Begin("2D box settings", nullptr, ImGuiWindowFlags_NoResize);
                ImGui::SetWindowSize(ImVec2(200, 85), ImGuiCond_Always);
                ImGui::Indent(15);

                ImVec2 box_pos = ImGui::GetWindowPos();
                ImVec2 box_size = ImGui::GetWindowSize();

                ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                    box_pos,
                    ImVec2(box_pos.x + box_size.x, box_pos.y + box_size.y),
                    colors::gui_end, colors::gui_mid, colors::gui_end, colors::gui_mid
                );

                ImGui::Dummy(ImVec2(smallPadding));

                ImGui::SliderFloat("##boxesThickness", &options::boxesThickness, 0.0f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Dummy(ImVec2(smallPadding));

                ImGui::SliderFloat("##boxesRounding", &options::boxesRounding, 0.0f, 20.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Dummy(ImVec2(smallPadding));

                ImGui::End();
            }

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Players  ", &options::bPlayerBoxEnabled);
            ImGui::SameLine();
            colorPickerButton(&options::playersColor, 3);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Troops  ", &options::bTroopBoxEnabled);
            ImGui::SameLine();
            colorPickerButton(&options::troopsColor, 4);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" NPC  ", &options::bNPCBoxEnabled);
            ImGui::SameLine();
            colorPickerButton(&options::npcsColor, 5);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Game Items", &options::bGameItemsEnabled);

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::Checkbox(" Items  ", &options::bItemBoxEnabled);
            ImGui::SameLine();
            colorPickerButton(&options::itemsColor, 7);

            ImGui::Dummy(ImVec2(defPadding));
            ImGui::Checkbox(" Health   ", &options::bHealthEnabled);
           
            ImGui::EndChild();

            ImGui::Dummy(ImVec2(defPadding));

            ImGui::EndChild();

            ImGui::PopStyleColor(3);
            break;
        }

        case selected::Credits:
        {
            ImGui::BeginChild("creditsFrame", ImVec2(rightPane_size.x - 20, rightPane_size.y - 25), true);
            ImGui::Indent(15);

            ImVec2 frame_pos = ImGui::GetWindowPos();
            ImVec2 frame_size = ImGui::GetWindowSize();

            drawList->AddRectFilled(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + frame_size.y),
                colors::child_bg
            );

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Credits");
            ImGui::Dummy(ImVec2(0, 25));

            drawList->AddRectFilled(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + 40),
                colors::child_bg
            );

            drawList->AddRect(
                frame_pos,
                ImVec2(frame_pos.x + frame_size.x, frame_pos.y + 40),
                colors::gui_end,
                0.0f,
                0,
                1.5f
            );

            ImGui::Text("Cubus - External cheat for stalcube");
            ImGui::Dummy(smallPadding);
            ImGui::Text("Made by lityrgia");
            ImGui::Dummy(smallPadding);
            ImGui::Text("Github: github.com/lityrgia");
            ImGui::Dummy(smallPadding);
            ImGui::Text("Yougame: yougame.biz/members/1277973");
            ImGui::Dummy(smallPadding);
            break;
        }

        default:
            ImGui::Text("You shouldn't see this");
            break;
        }

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::End();
}

void renderMain() {
    std::vector<CEntity> localCopy_ent;

    {
        std::lock_guard<std::mutex> lock(gameDataMutex);
        localCopy_ent = entities;
    }

    auto drawList = ImGui::GetBackgroundDrawList();

    for (auto& entity : localCopy_ent) {

        ImVec4 color;
        bool shouldSkip = false;

        switch (entity.type) {
        case CEntity::Type::Player:
            shouldSkip = !options::bPlayerBoxEnabled;
            color = options::playersColor;
            break;
        case CEntity::Type::Troop:
            shouldSkip = !options::bTroopBoxEnabled;
            color = options::troopsColor;
            break;
        case CEntity::Type::Item:
            shouldSkip = !options::bItemBoxEnabled;
            color = options::itemsColor;
            break;
        case CEntity::Type::NPC:
            shouldSkip = !options::bNPCBoxEnabled;
            color = options::npcsColor;
            break;
        case CEntity::Type::GAME_ITEMS:
            shouldSkip = !options::bGameItemsEnabled;
            color = options::itemsColor;
            break;
        default:
            //не отображать остальных
            shouldSkip = true;
            color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        }

        if (shouldSkip)  continue;

        float boxCenterX = (entity.w2sbb.x + entity.w2sbb.z) / 2.f;
        float boxHeight = entity.w2sbb.x - entity.w2sbb.z;
        float boxWidth = entity.w2sbb.x - entity.w2sbb.y;

        if (entity.w2sbb.x <= 0 || entity.w2sbb.y <= 0) {
            continue;
        }

        if (options::bSnaplinesEnabled) {
            drawList->AddLine(ImVec2(options::monitorX / 2, options::monitorY), vec2(boxCenterX, entity.w2sbb.w), ImGui::ColorConvertFloat4ToU32(color), options::snaplinesThickness);
        }

        if (options::b2DBoxEnabled) {
            drawList->AddRect(vec2(entity.w2sbb.x, entity.w2sbb.y), vec2(entity.w2sbb.z, entity.w2sbb.w), ImGui::ColorConvertFloat4ToU32(color), options::boxesRounding, options::boxesThickness);
        }

        if (options::bHealthEnabled && entity.health > 0) {
            float healthRatio = std::clamp(entity.health / entity.maxHealth, 0.f, 1.f);

            const float barWidth = 2.0f;
            const float spacing = 0.5f;

            const float boxHeight = entity.w2sbb.w - entity.w2sbb.y;
            const float healthHeight = boxHeight * healthRatio;

            const ImVec2 start{
                entity.w2sbb.x - barWidth - spacing,
                entity.w2sbb.w - healthHeight
            };
            const ImVec2 end{
                entity.w2sbb.x - spacing,
                entity.w2sbb.w
            };

            drawList->AddRectFilled(start, end, ImColor(0, 255, 0, 210));  
        }


        if (options::bNamesEnabled) {
            ImFont* font = ImGui::GetFont();
            float fontSize = 14.0f;

            std::string nameUtf8 = u16_to_utf8(entity.name);
            ImVec2 textSize = ImGui::CalcTextSize(nameUtf8.c_str(), nullptr, false, fontSize);

            ImVec2 textPos = ImVec2(entity.w2sbb.x - textSize.x / 2.0f, entity.w2sbb.y - 15.0f);

            ImGui::GetBackgroundDrawList()->AddText(font, fontSize, textPos, ImColor(255, 255, 255, 255), nameUtf8.c_str());
        }
    }
}