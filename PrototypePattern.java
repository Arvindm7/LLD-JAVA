import java.util.HashMap;
import java.util.Map;

// Defining the Prototype Interface
interface EmailTemplate extends Cloneable{
    EmailTemplate clone();//Deep copy recommended
    void setContent(String content);
    void sendEmail(String recipient);
}

//concrete class implementing the prototype interface
class WelcomeEmailTemplate implements EmailTemplate {
    private String subject;
    private String content;

    public WelcomeEmailTemplate(String subject, String content) {
        this.subject = "Welcome to Our Service!";
        this.content = "Hi, welcome to our service! We are glad to have you on board.";
    }

    @Override
    public WelcomeEmailTemplate clone() {
        try {
            return (WelcomeEmailTemplate) super.clone();
        } catch (CloneNotSupportedException e) {
            throw new AssertionError(); // Should never happen
        }
    }

    @Override
    public void setContent(String content) {
        this.content = content;
    }

    @Override
    public void sendEmail(String recipient) {
        System.out.println("Sending Welcome Email to " + recipient + " with content: " + content);
    }

}

//template registry to manage and provide access to the prototypes clones
class EmailTemplateRegistry {
    private static final Map<String, EmailTemplate> templates = new HashMap<>();

    // Static block to initialize the registry with default templates
    static {
        templates.put("welcome", new WelcomeEmailTemplate("Welcome to Our Service!", "Hi, welcome to our service! We are glad to have you on board."));
    }

    public static EmailTemplate getTemplate(String templateName) {
        EmailTemplate template = templates.get(templateName);
        if (template != null) {
            return template.clone();
        }
        return null;
    }
}


public class PrototypePattern {
    public static void main(String[] args) {
        // Get a clone of the welcome email template
        EmailTemplate welcomeEmail = EmailTemplateRegistry.getTemplate("welcome");
        if (welcomeEmail != null) {
            welcomeEmail.setContent("Hi John, welcome to our service! We are glad to have you on board.");
            welcomeEmail.sendEmail("john.doe@example.com");
        }

        // Get another clone of the welcome email template
        EmailTemplate anotherWelcomeEmail = EmailTemplateRegistry.getTemplate("welcome");  
        if (anotherWelcomeEmail != null) {
                anotherWelcomeEmail.setContent("Hi Jane, welcome to our service! We are glad to have you on board.");
                anotherWelcomeEmail.sendEmail("jane.doe@example.com");
        }
    }
}
