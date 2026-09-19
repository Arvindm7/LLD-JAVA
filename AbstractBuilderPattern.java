
//------interfaces--------
interface PaymentGatewway{
    public void processPayment(double amount);
}

interface Invoice{
    public void generateInvoice();
}

//-----Abstract Factory Pattern Implementation-----
interface RegionFactory{
    PaymentGatewway createPaymentGateway(String gatewayType);
    Invoice createInvoice();
}

//-----Concrete Factory Classes-----
class IndiaFactory implements RegionFactory{

    @Override
    public PaymentGatewway createPaymentGateway(String gatewayType) {
        switch (gatewayType.toLowerCase()) {
            case "razorpay":
                return new RazorpayPaymentGateway();
            case "payu":
                return new PayUPaymentGateway();
            default:
                throw new IllegalArgumentException("Invalid payment gateway type: " + gatewayType);
        }
    }

    @Override
    public Invoice createInvoice() {
        return new GSTInvoice();
    }
}

class USFactory implements RegionFactory{

    @Override
    public PaymentGatewway createPaymentGateway(String gatewayType) {
        switch (gatewayType.toLowerCase()) {
            case "stripe":
                return new StripePaymentGateway();
            case "paypal":
                return new PayPalPaymentGateway();
            default:
                throw new IllegalArgumentException("Invalid payment gateway type: " + gatewayType);
        }
    }

    @Override
    public Invoice createInvoice() {
        return new USInvoice();
    }
}

//---india implementation of payment gateway and invoice classes

class RazorpayPaymentGateway implements PaymentGatewway{
    @Override
    public void processPayment(double amount) {
        System.out.println("Processing payment of amount: " + amount + " through Razorpay");
    }
}

class PayUPaymentGateway implements PaymentGatewway{
    @Override
    public void processPayment(double amount) {
        System.out.println("Processing payment of amount: " + amount + " through PayU");
    }
}

class GSTInvoice implements Invoice{
    @Override
    public void generateInvoice() {
        System.out.println("Generating GST invoice...");
    }
}

//----us implementation of payment gateway and invoice classes
class StripePaymentGateway implements PaymentGatewway{
    @Override
    public void processPayment(double amount) {
        System.out.println("Processing payment of amount: " + amount + " through Stripe");
    }
}
class PayPalPaymentGateway implements PaymentGatewway{
    @Override
    public void processPayment(double amount) {
        System.out.println("Processing payment of amount: " + amount + " through PayPal");
    }
}

class USInvoice implements Invoice{
    @Override
    public void generateInvoice() {
        System.out.println("Generating US invoice...");
    }
}

//-----checkout service class that uses the abstract factory to create payment gateway and invoice objects
class CheckoutService {
    private PaymentGatewway paymentGateway;
    private Invoice invoice;
    

    public CheckoutService(RegionFactory regionFactory,String gatewayType) {
        this.paymentGateway = regionFactory.createPaymentGateway(gatewayType);
        this.invoice = regionFactory.createInvoice();
    }

    public void checkout(double amount) {
        paymentGateway.processPayment(amount);
        invoice.generateInvoice();
    }
}

// if you want to add more region you can easily scale the code by creating a new factory class
// that implements the RegionFactory interface and providing the implementation for the payment
// gateway and invoice classes for that region.
// for example if you want to add a new region "UK" you can create a new class UKFactory that implements the RegionFactory interface and provide the implementation for the payment gateway and invoice classes for that region.
// you can also add more payment gateway and invoice classes for the existing regions by creating new classes that implement the PaymentGateway and Invoice interfaces respectively.
// create abstract factory pattern for checkout service that can be used to create payment gateway and invoice objects for different regions.
//  The abstract factory pattern provides an interface for creating families of related or dependent objects without specifying their concrete classes.
//  This allows for easy scalability and maintainability of the code.

public class AbstractBuilderPattern {
    public static void main(String[] args) {
        //create a checkout service for india region with razorpay payment gateway
        CheckoutService indiaCheckoutService = new CheckoutService(new IndiaFactory(), "razorpay");
        indiaCheckoutService.checkout(1000.0);

        //create a checkout service for us region with stripe payment gateway
        CheckoutService usCheckoutService = new CheckoutService(new USFactory(), "stripe");
        usCheckoutService.checkout(2000.0);
    }
    
}
