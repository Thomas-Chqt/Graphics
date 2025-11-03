/*
 * ---------------------------------------------------
 * QueueCapabilities.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/13 09:29:43
 * ---------------------------------------------------
 */

#ifndef QUEUECAPABILITIES_HPP
#define QUEUECAPABILITIES_HPP

#include <vector>

namespace gfx
{

class Surface;

struct QueueCapabilities
{
    bool graphics;
    bool compute;
    bool transfer;
    std::vector<Surface*> present;
};

}

#endif // QUEUECAPABILITIES_HPP
