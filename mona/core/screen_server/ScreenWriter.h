#ifndef _MONAPI_TERMINAL_SCREEN_WRITER_
#define _MONAPI_TERMINAL_SCREEN_WRITER_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/Assert.h>
#include <monapi/terminal/Writer.h>

namespace MonAPI {
namespace terminal {

class ScreenWriter : public Writer
{
public:
    ScreenWriter();
    virtual ~ScreenWriter();

    virtual int clearScreen();
    virtual int moveCursor(uint32_t x, uint32_t y);
    virtual int moveCursorUp(uint32_t n);
    virtual int moveCursorDown(uint32_t n);
    virtual int moveCursorLeft(uint32_t n);
    virtual int moveCursorRight(uint32_t n);
    virtual int lineFeed();
    virtual int backSpace();
    virtual int write(uint8_t* buf, uint32_t length);
    virtual uint32_t getX() const { return x_; }
    virtual uint32_t getY() const { return y_; }

protected:
    uint32_t x_;
    uint32_t y_;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_SCREEN_WRITER_
