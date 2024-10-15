/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2024/10/15 14:16:28
 * ---------------------------------------------------
 */

#include "Graphics/Platform.hpp"
#include "nfd.hpp"
#include <iostream>

int main()
{
    gfx::Platform::init();
    {
        // auto-freeing memory
        NFD::UniquePath outPath;

        // prepare filters for the dialog
        nfdfilteritem_t filterItem[2] = {{"Source code", "c,cpp,cc"}, {"Headers", "h,hpp"}};

        // show the dialog
        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 2);
        if (result == NFD_OKAY) {
            std::cout << "Success!" << std::endl << outPath.get() << std::endl;
        } else if (result == NFD_CANCEL) {
            std::cout << "User pressed cancel." << std::endl;
        } else {
            std::cout << "Error: " << NFD::GetError() << std::endl;
        }
    }
    gfx::Platform::terminate();
}