#include "../../Includes/UserInterface/WindowController.h"

/* Copyright (c) 2020 [Rick de Bondt] - WindowController.cpp */

#undef MOUSE_MOVED

#include <cmath>

#include "../../Includes/UserInterface/NCursesKeys.h"
#include "../../Includes/UserInterface/NetworkingWindow.h"
#include "../../Includes/UserInterface/XLinkWindow.h"

Dimensions ScaleNetworkingWindow(const int& aMaxHeight, const int& aMaxWidth)
{
    return {0, 0, static_cast<int>(floor(aMaxHeight / 2.0)), aMaxWidth};
}

Dimensions ScaleXLinkWindow(const int& aMaxHeight, const int& aMaxWidth)
{
    return {static_cast<int>(floor(aMaxHeight / 2.0)), 0, static_cast<int>(floor(aMaxHeight / 2.0)), aMaxWidth};
}

Dimensions ScaleSSIDSelectWindow(const int& aMaxHeight, const int& aMaxWidth)
{
    return {10, 10, aMaxHeight - 20, aMaxWidth - 20};
}

WindowController::WindowController(WindowModel& aModel) :
    mMainCanvas{nullptr}, mHeight{0}, mWidth{0}, mDimensionsChanged{false}, mWindows{}, mExclusiveWindow{false},
    mWindowSelector{0, nullptr}, mModel{aModel}
{}

bool WindowController::SetUp()
{
    initscr();
    keypad(stdscr, true);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, true);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);
        init_pair(3, COLOR_BLACK, COLOR_CYAN);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_BLACK, COLOR_WHITE);
    }

    mMainCanvas = NCursesWindow(newwin(0, 0, 0, 0), [](WINDOW* aWin) { delwin(aWin); });

    getmaxyx(mMainCanvas.get(), mHeight, mWidth);

    mWindows.emplace_back(std::make_shared<NetworkingWindow>(
        mModel, "Networking pane:", [&] { return ScaleNetworkingWindow(mHeight, mWidth); }));

    mWindows.emplace_back(
        std::make_shared<XLinkWindow>(mModel, "XLink Kai pane:", [&] { return ScaleXLinkWindow(mHeight, mWidth); }));

    mWindows.emplace_back(std::make_shared<Window>(
        mModel, "SSID Selection:", [&] { return ScaleSSIDSelectWindow(mHeight, mWidth); }, false, false, false));

    mWindowSelector.first  = 0;
    mWindowSelector.second = mWindows.at(0);

    mWindowSelector.second->DeSelect();
    mWindowSelector.second->AdvanceSelectionVertical();

    return true;
}

void WindowController::AdvanceWindow()
{
    if (!mExclusiveWindow) {
        bool lEndLoop{false};
        int  lStartIndex{mWindowSelector.first};
        int  lIndex{lStartIndex};

        while (!lEndLoop) {
            lIndex++;
            if (lIndex >= mWindows.size()) {
                lIndex = 0;
            }

            if (mWindows.at(lIndex)->IsVisible()) {
                mWindowSelector.second->DeSelect();
                mWindowSelector = {lIndex, mWindows.at(lIndex)};
                mWindowSelector.second->DeSelect();
                mWindowSelector.second->AdvanceSelectionVertical();
                lEndLoop = true;
            } else if (lIndex == lStartIndex) {
                mWindowSelector = {lStartIndex, mWindowSelector.second};
                lEndLoop        = true;
            }
        }
    }
}

bool WindowController::Process()
{
    bool lReturn{true};
    int  mLastKeyPressed = getch();
    switch (mLastKeyPressed) {
        case 'q':
            lReturn = false;
            break;
        case cKeyTab:
            AdvanceWindow();
            break;
        default:
            mWindowSelector.second->HandleKey(mLastKeyPressed);
            break;
    }

    int lHeight{0};
    int lWidth{0};
    getmaxyx(mMainCanvas.get(), lHeight, lWidth);

    if ((lHeight != mHeight) || (lWidth != mWidth)) {
        mHeight            = lHeight;
        mWidth             = lWidth;
        mDimensionsChanged = true;
    }

    wrefresh(mMainCanvas.get());

    if (mExclusiveWindow) {
        if (!mWindowSelector.second->IsVisible() || !(mWindowSelector.second->IsExclusive())) {
            mExclusiveWindow = false;
        } else {
            if (mDimensionsChanged) {
                mWindowSelector.second->Scale();
            }
            mWindowSelector.second->Draw();
        }
    }

    if (!mExclusiveWindow) {
        int lIndex{0};
        for (auto& lWindow : mWindows) {
            if (lWindow->IsVisible()) {
                if (mDimensionsChanged) {
                    for (int lCount = 0; lCount < mHeight; lCount++) {
                        std::string lString{};
                        lString.resize(mWidth, ' ');
                        mvwaddstr(mMainCanvas.get(), lCount, 0, lString.c_str());
                    }
                    lWindow->Scale();
                }
                lWindow->Draw();
                if (lWindow->IsExclusive()) {
                    mWindowSelector.first  = lIndex;
                    mWindowSelector.second = lWindow;
                }
            }
            lIndex++;
        }
    }

    mDimensionsChanged = false;
    curs_set(0);

    return lReturn;
}

WindowController::~WindowController()
{
    endwin();
}