#pragma once
#include "texture.h"

#include <memory>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE};

class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type) : _type(type) {        
    }

    void SetPosition(Point pos) {
        _pos = pos;
    }

    void SetSize(Size size) {
        _size = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        _texture = texture;
    }

    // Рисует фигуру на указанном изображении
	// В зависимости от типа фигуры должен рисоваться либо эллипс, либо прямоугольник
    // Пиксели фигуры, выходящие за пределы текстуры, а также в случае, когда текстура не задана,
    // должны отображаться с помощью символа точка '.'
    // Части фигуры, выходящие за границы объекта image, должны отбрасываться.
    void Draw(Image& image) const {
        (void) image;

        if (_type == ShapeType::RECTANGLE) {
            for (int y = 0; y < _size.height; ++y) {
                for (int x = 0; x < _size.width; ++x) {
                    image.at(y + _pos.y).at(x + _pos.x) = _texture.get()->GetPixelColor({x, y});
                }
            }
        }
        
    }

    private:
    ShapeType _type;
    Point _pos;
    Size _size;
    std::shared_ptr<Texture> _texture = nullptr;
}; 