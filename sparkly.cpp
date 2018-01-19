#include <emscripten/bind.h>
#include <cstddef>
#include <cstdlib>
#include <math.h>
#include <string.h>
#include <algorithm>

extern "C" {
    extern void consoleLog(const char *message);
}

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba;

typedef struct
{
    double x;
    double y;
    int radius;
    double velocityX;
    double velocityY;
    double opacity;
    double colorOffset;
} element;

class Sparkly {

    private:

        const int MAX_ELEMENTS = 2000;

        int width;
        int height;
        uint8_t *buffer = nullptr;
        size_t bufferSize = 0;
        element **elements = nullptr;
        int currentElementIndex = 0;

        rgba createColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            rgba color;
            color.r = r; 
            color.g = g; 
            color.b = b; 
            color.a = a;
            return color;
        }

        void renderPixel(uint8_t *buffer, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            size_t bufferOffset = ((y * width) + x) * 4;
            if (bufferOffset > 0 && bufferOffset < (bufferSize - 4)) {
                buffer[bufferOffset + 0] = r;
                buffer[bufferOffset + 1] = g;
                buffer[bufferOffset + 2] = b;
                buffer[bufferOffset + 3] = a;
            }
        }

        void drawElement(uint8_t *buffer, element el) {
            int px = (int)round(el.x);
            int py = (int)round(el.y);
            for (int x = px - el.radius; x < px + el.radius; x++) {
                for (int y = py - el.radius; y < py + el.radius; y++) {
                    int dx = x - px;
                    int dy = y - py;
                    if ((dx * dx) + (dy * dy) <= (el.radius * el.radius)) {
                        uint8_t opacityOffset = (uint8_t)round(fmax(el.opacity * 255, 0));
                        uint8_t colorOffset = std::min((int)round(el.colorOffset * 255), 255);
                        renderPixel(buffer, x, y, 255, 255 - colorOffset, 0, opacityOffset);
                    }
                }
            }
        }

        void initialiseElement(element *el, double x, double y) {
            el->x = x;
            el->y = y;
            el->radius = 1 + (rand() % 10);
            el->velocityX = -5 + (rand() % 10);
            el->velocityY = -5 + (rand() % 10);
            el->opacity = 1;
            el->colorOffset = 0;
        }

        element* createElement(double x, double y) {
            element* el = (element *)malloc(sizeof(element));
            initialiseElement(el, x, y);
            return el;
        }

        void incrementPosition(element *el, double posX, double posY) {
            el->x += el->velocityX;
            el->y += el->velocityY;
            el->velocityY += 0.163333;
            el->opacity -= 0.005 * (10 / (1 + (rand() % 15)));
            el->colorOffset += 0.02;
            const double bounceMultiplier = 0.8;
            int rad = el->radius;
            if (el->opacity <= 0.1) {
                initialiseElement(el, posX, posY);
            } else {
                if ((el->y + rad) > height) {
                    el->y = height - rad;
                    el->velocityY = (-el->velocityY * bounceMultiplier);
                }
                if ((el->y - rad) < 0) {
                    el->y = rad;
                    el->velocityY = (-el->velocityY * bounceMultiplier);
                }
                if((el->x + rad) > width) {
                    el->x = width - rad;
                    el->velocityX = (-el->velocityX * bounceMultiplier);
                }
                if ((el->x - rad) < 0) {
                    el->x = rad;
                    el->velocityX = (-el->velocityX * bounceMultiplier);
                }
            }
        }

        void initElements(int startIndex, int endIndex, double posX, double posY) {
            if (elements == nullptr) {
                elements = (element **)malloc(sizeof(element*) * MAX_ELEMENTS);
            }
            if (startIndex < MAX_ELEMENTS) {
                for (int i = startIndex; i < endIndex; i++) {
                    elements[i] = createElement(posX, posY);
                }
            }
        }

    public:

        Sparkly(int width, int height): width(width), height(height) {
            bufferSize = width * height * 4;
            buffer = (uint8_t *)malloc(bufferSize);
        }

        ~Sparkly() {
            free(buffer);
            free(elements);
        }

        emscripten::val drawFrame(double posXRaw, double posYRaw) {
            
            memset(buffer, 0, bufferSize);

            int posX = std::min(posXRaw, (double)width);
            int posY = std::min(posYRaw, (double)height);

            double prevIndex = currentElementIndex;
            if (currentElementIndex < MAX_ELEMENTS) {
                currentElementIndex = std::min(MAX_ELEMENTS, currentElementIndex + (rand() % 10));
            }

            initElements(prevIndex, currentElementIndex, posX, posY);

            for (int i = 0; i < currentElementIndex; i++) {
                incrementPosition(elements[i], posX, posY);
                drawElement(buffer, *elements[i]);
            }

            return emscripten::val(emscripten::typed_memory_view(bufferSize, buffer));
        }
};

EMSCRIPTEN_BINDINGS(hello) {
  emscripten::class_<Sparkly>("Sparkly")
      .constructor<int, int>()
      .function("drawFrame", &Sparkly::drawFrame);
}