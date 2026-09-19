interface Shape {
    void draw();
}

class Circle implements Shape {
    @Override
    public void draw() {
        System.out.println("Inside Circle::draw() method.");
    }
}

class Rectangle implements Shape {
    @Override
    public void draw() {
        System.out.println("Inside Rectangle::draw() method.");
    }
}

class Square implements Shape {
    @Override
    public void draw() {
        System.out.println("Inside Square::draw() method.");
    }
}

class ShapeFactory {
    // use getShape method to get object of type shape
    public Shape getShape(String shapeType) {
        if (shapeType == null) {
            return null;
        }
        if (shapeType.equalsIgnoreCase("CIRCLE")) {
            return new Circle();
        } else if (shapeType.equalsIgnoreCase("RECTANGLE")) {
            return new Rectangle();
        } else if (shapeType.equalsIgnoreCase("SQUARE")) {
            return new Square();
        }
        return null;
    }
}

class DrawShapeService {

    private ShapeFactory shapeFactory;

    public DrawShapeService(ShapeFactory shapeFactory) {
        this.shapeFactory = shapeFactory;
    }

    public void drawShape(String shapeType) {
        Shape shape = shapeFactory.getShape(shapeType);
        if (shape != null) {
            shape.draw();
        } else {
            System.out.println("Invalid shape type: " + shapeType);
        }
    }

}

public class FactoryPattern {
    public static void main(String[] args) {
        
        ShapeFactory shapeFactory = new ShapeFactory();
        DrawShapeService drawShapeService = new DrawShapeService(shapeFactory);

        drawShapeService.drawShape("CIRCLE");
        drawShapeService.drawShape("RECTANGLE");
        drawShapeService.drawShape("SQUARE");
        drawShapeService.drawShape("TRIANGLE"); // Invalid shape type
        
    }
}
