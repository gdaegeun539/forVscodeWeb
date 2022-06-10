package org.tukorea.pattern;

/*
 * 기본적인 component 추상 클래스 - 음료수 예
 * Component
 */
abstract class Beverage {
	String description = "제목 없음";

	public String getDescription() { return description; }
	public abstract double cost();
}

/*
 * 확장 기능을 가진 데코레이터 추상 클래스 - 음료수를 장식하는 예
 * Decorator
 */
abstract class CondimentDecorator extends Beverage {
	public abstract String getDescription();
}
/*
 * Espresso 기본적인 component
 * ConcreteComponent
 */
class Espresso extends Beverage {
    public Espresso() { description = "에스프레소"; }

    @Override
    public double cost() { return 3600; }
}
/*
 * HouseBlend 기본적인 component
 * ConcreteComponent
 */
class HouseBlend extends Beverage {
    public HouseBlend() { description = "하우스 블렌드 커피"; }

    @Override
    public double cost() { return 4100; }
}
/*
 * Espresso, HouseBlend 기본적인 component를 수정하지 않고 CondimentDecorator 데코레이터 클래스를 통해 확장 - Mocha 클래스
 * Espresso, HouseBlend 에 Mocha 를 추가
 * ConcreteDecorator
 */
class Mocha extends CondimentDecorator {
    Beverage beverage;

    public Mocha(Beverage beverage) {
        description = "모카";
        this.beverage = beverage;
    }

    @Override
    public double cost() {
        return 2000 + beverage.cost();
    }

	@Override
	public String getDescription() {
		return beverage.getDescription() + " + " + description;
	}
}

/**
 * PSVM
 */
public class DecoratorPattern {
	public static void main(String[] args) {  
		Beverage beverage1 = new Espresso();
		System.out.println(beverage1.getDescription() + " 가격: " +beverage1.cost());
		beverage1 = new Mocha(beverage1);
		System.out.println(beverage1.getDescription() + " 가격: " +beverage1.cost());
	
		Beverage beverage2 = new HouseBlend();
		System.out.println(beverage2.getDescription() + " 가격: " +beverage2.cost());
		beverage2 = new Mocha(beverage2);
		System.out.println(beverage2.getDescription() + " 가격: " +beverage2.cost());
	}
}
