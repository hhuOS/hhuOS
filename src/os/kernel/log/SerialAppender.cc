#include "SerialAppender.h"

void SerialAppender::append(const String &message) {

    serial.sendData((char*) message, message.length());
}

SerialAppender::SerialAppender(Serial &serial) : serial(serial) {

}
