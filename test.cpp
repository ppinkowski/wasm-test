#include <string>

extern "C" {
    extern void consoleLog(const char *message);
    extern void addEventLstnr(const char *event, void (*callback)(int posX, int posY));

    void logAThing(const char* message) {
        consoleLog(message);
    }
}

void onClick(int posX, int posY) {
    std::string message = "Mouse Position: " + std::to_string(posX) + ", " + std::to_string(posY);
    consoleLog(message.c_str());
}

int main() {
    consoleLog("Works yo!");
    addEventLstnr("mousemove", onClick);
}

