
// Singleton Pattern
//
// A singleton allows only one object of a class to exist and provides a
// global access point through getInstance(). This file demonstrates four
// common ways to implement it.

// Eager loading: the instance is created when the class is loaded.
// Thread-safe because class initialization is handled by the JVM.
class CompilerEager {

    private static final CompilerEager compiler = new CompilerEager();

    private CompilerEager() {
    }

    public static CompilerEager getInstance() {
        // Every call returns the same eagerly-created object.
        return compiler;
    }

}

// Lazy loading: create the object only when it is first requested.
// The synchronized method makes initialization thread-safe, but every call
// also pays the synchronization cost.
class CompilerLazy {

    private static CompilerLazy compilerLazy;

    private CompilerLazy() {
    }

    public static synchronized CompilerLazy getInstance() {
        // The null check ensures construction happens only once.
        if (compilerLazy == null) {
            compilerLazy = new CompilerLazy();
        }
        return compilerLazy;
    }

}

// Lazy loading with double-checked locking.
// Most calls avoid synchronization after the instance has been created.
// Note: a production implementation should make the shared field volatile.
class CompilerLazyDoubleChecked {

    private static CompilerLazyDoubleChecked compilerLazyDoubleChecked;

    private CompilerLazyDoubleChecked() {
    }

    public static CompilerLazyDoubleChecked getInstance() {
        // First check avoids locking when initialization is already complete.
        if (compilerLazyDoubleChecked == null) {
            synchronized (CompilerLazyDoubleChecked.class) {
                // The second check prevents two threads from creating objects.
                if (compilerLazyDoubleChecked == null) {
                    compilerLazyDoubleChecked = new CompilerLazyDoubleChecked();
                }
            }
        }
        return compilerLazyDoubleChecked;
    }

}

// Lazy loading with the Bill Pugh approach.
// The nested class is initialized only when getInstance() uses it, and JVM
// class initialization provides thread safety without explicit locking.
class CompilerBillPugh {

    private CompilerBillPugh() {
    }

    private static class SingletonHelper {
        private static final CompilerBillPugh compiler = new CompilerBillPugh();
    }

    public static CompilerBillPugh getInstance() {
        // Accessing the helper triggers lazy, thread-safe initialization.
        return SingletonHelper.compiler;
    }

}

public class SingletonPattern {
    public static void main(String[] args) {
        // Request each implementation twice to demonstrate that both calls
        // refer to the same singleton instance.
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

