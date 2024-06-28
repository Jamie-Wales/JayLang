import java.lang.invoke.*;
import java.math.BigDecimal;

import Types.JayObject;
import Interop.JayInterop;

public class Test {
    public static void main(String[] args) {
        try {
            // Test Math.pow (static method)
            JayObject<BigDecimal> base = JayObject.generateObject(2.0);
            JayObject<BigDecimal> exponent = JayObject.generateObject(3.0);
            CallSite site = JayInterop.bootstrap(
                    MethodHandles.lookup(),
                    "pow",
                    MethodType.methodType(Object.class, Object.class, Object.class),
                    "java.lang.Math",
                    "pow"
            );

            Object result = site.dynamicInvoker().invoke(base, exponent);
            System.out.println("Result of Math.pow: " + result);

            // Test String.concat (instance method)
            JayObject<String> str1 = JayObject.generateObject("Hello");
            JayObject<String> str2 = JayObject.generateObject(" World");
            site = JayInterop.bootstrap(
                    MethodHandles.lookup(),
                    "concat",
                    MethodType.methodType(Object.class, Object.class, Object.class),
                    "java.lang.String",
                    "concat"
            );
            result = site.dynamicInvoker().invoke(str1, str2);
            System.out.println("Result of String.concat: " + result);
        } catch (Throwable t) {
            t.printStackTrace();
        }
    }
}