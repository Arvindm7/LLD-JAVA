class BurgerMeal{
    //required parameters
    private final String burgerType;
    private final String pattyType;

    //optional parameters
    private final boolean cheese;
    private final boolean lettuce;
    private final boolean tomato;
    private final boolean onion;
    private final boolean pickles;
    private final boolean ketchup;
    private final boolean mustard;
    private final boolean mayo;
    private final boolean sides;
    private final boolean drink;

    //private constructor to enforce the use of the builder
    private BurgerMeal(BurgerMealBuilder builder) {
        this.burgerType = builder.burgerType;
        this.pattyType = builder.pattyType;
        this.cheese = builder.cheese;
        this.lettuce = builder.lettuce;
        this.tomato = builder.tomato;
        this.onion = builder.onion;
        this.pickles = builder.pickles;
        this.ketchup = builder.ketchup;
        this.mustard = builder.mustard;
        this.mayo = builder.mayo;
        this.sides = builder.sides;
        this.drink = builder.drink;
    }

    //getters for the parameters
    public String getBurgerType() {
        return burgerType;
    }

    public String getPattyType() {
        return pattyType;
    }

    public boolean hasCheese() {
        return cheese;
    }

    public boolean hasLettuce() {
        return lettuce;
    }

    public boolean hasTomato() {
        return tomato;
    }

    public boolean hasOnion() {
        return onion;
    }

    public boolean hasPickles() {
        return pickles;
    }

    public boolean hasKetchup() {
        return ketchup;
    }

    public boolean hasMustard() {
        return mustard;
    }

    public boolean hasMayo() {
        return mayo;
    }

    public boolean hasSides() {
        return sides;
    }

    public boolean hasDrink() {
        return drink;
    }


    //static nested builder class
    public static class BurgerMealBuilder {
        //required parameters
        private final String burgerType;
        private final String pattyType;

        //optional parameters - initialized to default values
        private boolean cheese = false;
        private boolean lettuce = false;
        private boolean tomato = false;
        private boolean onion = false;
        private boolean pickles = false;
        private boolean ketchup = false;
        private boolean mustard = false;
        private boolean mayo = false;
        private boolean sides = false;
        private boolean drink = false;

        public BurgerMealBuilder(String burgerType, String pattyType) {
            this.burgerType = burgerType;
            this.pattyType = pattyType;
        }

        public BurgerMealBuilder addCheese() {
            this.cheese = true;
            return this;
        }

        public BurgerMealBuilder addLettuce() {
            this.lettuce = true;
            return this;
        }

        public BurgerMealBuilder addTomato() {
            this.tomato = true;
            return this;
        }

        public BurgerMealBuilder addOnion() {
            this.onion = true;
            return this;
        }

        public BurgerMealBuilder addPickles() {
            this.pickles = true;
            return this;
        }

        public BurgerMealBuilder addKetchup() {
            this.ketchup = true;
            return this;
        }

        public BurgerMealBuilder addMustard() {
            this.mustard = true;
            return this;
        }

        //method to add mayo to the meal
        public BurgerMealBuilder addMayo() {
            this.mayo = true;
            return this;
        }

        //method to add sides to the meal
        public BurgerMealBuilder addSides() {
            this.sides = true;
            return this;
        }

        //method to add drink to the meal
        public BurgerMealBuilder addDrink() {
            this.drink = true;
            return this;
        }

        //build method to create the final object
        public BurgerMeal build() {
            return new BurgerMeal(this);
        }

    }
}


public class BuilderPattern {
    public static void main(String[] args) {
        //create a burger meal with cheese, lettuce, tomato, and onion
        BurgerMeal burgerMeal = new BurgerMeal.BurgerMealBuilder("Wheat", "Veg")
                .addCheese()
                .addLettuce()
                .addTomato()
                .addOnion()
                .build();

        System.out.println("Burger Meal created with the following options:");
        System.out.println("Burger Type: " + burgerMeal.getBurgerType());
        System.out.println("Patty Type: " + burgerMeal.getPattyType());
        System.out.println("Cheese: " + burgerMeal.hasCheese());
        System.out.println("Lettuce: " + burgerMeal.hasLettuce());
        System.out.println("Tomato: " + burgerMeal.hasTomato());
        System.out.println("Onion: " + burgerMeal.hasOnion());
        System.out.println("Pickles: " + burgerMeal.hasPickles());
        System.out.println("Ketchup: " + burgerMeal.hasKetchup());
        System.out.println("Mustard: " + burgerMeal.hasMustard());
        System.out.println("Mayo: " + burgerMeal.hasMayo());
        System.out.println("Sides: " + burgerMeal.hasSides());
        System.out.println("Drink: " + burgerMeal.hasDrink());
    }

    
}
