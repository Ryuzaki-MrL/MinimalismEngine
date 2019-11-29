#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <string>

bool messagesLoad(const char* filename);

void messageSetLang(uint16_t id);
void messageSetLang(const std::string& id);
uint16_t messageGetLang();
const char* messageGetLangStr();

const char* messageGet(uint16_t id);
const char* messageGet(const std::string& id);
uint16_t messageGetIndex(const std::string& id);

#endif /* MESSAGE_H */
