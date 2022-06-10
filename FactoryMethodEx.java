package org.tukorea.pattern;

/*
 * Shape 추상 클래스
 * ProductInterface
 */
abstract class Shape {
    abstract void draw();
}
/*
 * Circle 구현 클래스
 * Product
 */
class Circle extends Shape {
    @Override
    void draw() {
        System.out.println("Circle - draw()");
    }
}
/*
 * Square 구현 클래스
 * Product
 */
class Square extends Shape {
    @Override
    void draw() {
        System.out.println("Square - draw()");
    }
}
/*
 * Shape  객체를 생성할 책임을 가진 ShapeFactory 클래스
 * ConcreteCreator
 */
class ShapeFactory {
    public Shape getShape(String shapeType) {
        switch (shapeType) {
            case "Circle":
                return new Circle();
            case "Square":
                return new Square();
            default:
                return null;
        }
    }
}

/**
 * Client
 * PSVM
*/
public class FactoryMethodEx {
	public static void main(String[] args) {
        ShapeFactory factory = new ShapeFactory();
        Shape circle = factory.getShape("Circle");
        Shape square = factory.getShape("Square");

        circle.draw();
        square.draw();
    }
}
