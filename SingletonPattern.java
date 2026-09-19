
//singleton pattern - Eager Loading
// thread safe - no synchronization needed
class CompilerEager {

    private static final CompilerEager compiler = new CompilerEager();

    private CompilerEager() {
    }

    public static CompilerEager getInstance() {
        return compiler;
    }

}

//Lazy Loading - Singleton Pattern
// thread safe - synchronized method
class CompilerLazy {

    private static CompilerLazy compilerLazy;

    private CompilerLazy() {
    }

    public static synchronized CompilerLazy getInstance() {
        if (compilerLazy == null) {
            compilerLazy = new CompilerLazy();
        }
        return compilerLazy;
    }

}

//Lazy Loading - double checked locking - Singleton Pattern
// thread safe - synchronized block
class CompilerLazyDoubleChecked {

    private static CompilerLazyDoubleChecked compilerLazyDoubleChecked;

    private CompilerLazyDoubleChecked() {
    }

    public static CompilerLazyDoubleChecked getInstance() {
        if (compilerLazyDoubleChecked == null) {
            synchronized (CompilerLazyDoubleChecked.class) {
                if (compilerLazyDoubleChecked == null) {
                    compilerLazyDoubleChecked = new CompilerLazyDoubleChecked();
                }
            }
        }
        return compilerLazyDoubleChecked;
    }

}

//lazy Loading - Bill Pugh Singleton Pattern
// thread safe - no synchronization needed
class CompilerBillPugh {

    private CompilerBillPugh() {
    }

    private static class SingletonHelper {
        private static final CompilerBillPugh compiler = new CompilerBillPugh();
    }

    public static CompilerBillPugh getInstance() {
        return SingletonHelper.compiler;
    }

}

public class SingletonPattern {
    public static void main(String[] args) {
        CompilerEager compilerEager = CompilerEager.getInstance();
        CompilerEager compilerEager2 = CompilerEager.getInstance();

        CompilerLazy compilerLazy = CompilerLazy.getInstance();
        CompilerLazy compilerLazy2 = CompilerLazy.getInstance();

        CompilerLazyDoubleChecked compilerLazyDoubleChecked = CompilerLazyDoubleChecked.getInstance();
        CompilerLazyDoubleChecked compilerLazyDoubleChecked2 = CompilerLazyDoubleChecked.getInstance();

        CompilerBillPugh compilerBillPugh = CompilerBillPugh.getInstance();
        CompilerBillPugh compilerBillPugh2 = CompilerBillPugh.getInstance();


        System.out.println("CompilerEager: " + compilerEager);
        System.out.println("CompilerEager2: " + compilerEager2);
        System.out.println("CompilerLazy: " + compilerLazy);
        System.out.println("CompilerLazy2: " + compilerLazy2);
        System.out.println("CompilerLazyDoubleChecked: " + compilerLazyDoubleChecked);
        System.out.println("CompilerLazyDoubleChecked2: " + compilerLazyDoubleChecked2);
        System.out.println("CompilerBillPugh: " + compilerBillPugh);
        System.out.println("CompilerBillPugh2: " + compilerBillPugh2);
    }
}

