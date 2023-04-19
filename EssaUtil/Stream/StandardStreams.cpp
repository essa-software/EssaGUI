#include "StandardStreams.hpp"

namespace Util {

ReadableFileStream& std_in() {
    static ReadableFileStream stream = ReadableFileStream::borrow_fd(0);
    return stream;
}

WritableFileStream& std_out() {
    static WritableFileStream stream = WritableFileStream::borrow_fd(1);
    return stream;
}

WritableFileStream& std_err() {
    static WritableFileStream stream = WritableFileStream::borrow_fd(2);
    return stream;
}

}
