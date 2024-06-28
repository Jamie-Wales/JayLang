package Interop;

import java.lang.invoke.*;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import Types.JayObject;

public class JayInterop {
    private static final MethodHandles.Lookup lookup = MethodHandles.lookup();

    public static CallSite bootstrap(MethodHandles.Lookup caller, String name, MethodType type, String className,
                                     String methodName) throws Throwable {
        MethodHandle target = lookup.findStatic(JayInterop.class, "callMethod",
                MethodType.methodType(Object.class, String.class, String.class, Object[].class));
        target = target.bindTo(className).bindTo(methodName);
        target = target.asVarargsCollector(Object[].class);
        return new ConstantCallSite(target.asType(type));
    }

    public static Object callMethod(String className, String methodName, Object... args)
            throws Throwable {
        Class<?> clazz = Class.forName(className);
        Object[] unwrappedArgs = new Object[args.length];
        Class<?>[] parameterTypes = new Class<?>[args.length];

        for (int i = 0; i < args.length; i++) {
            if (args[i] instanceof JayObject) {
                unwrappedArgs[i] = ((JayObject<?>) args[i]).getJavaObject();
            } else {
                unwrappedArgs[i] = args[i];
            }
            parameterTypes[i] = unwrappedArgs[i].getClass();
        }

        Method method = findBestMatchingMethod(clazz, methodName, parameterTypes);
        Object result;
        if (Modifier.isStatic(method.getModifiers())) {
            result = method.invoke(null, unwrappedArgs);
        } else {
            if (unwrappedArgs.length == 0) {
                throw new IllegalArgumentException("Instance method called with no arguments");
            }
            Object instance = unwrappedArgs[0];
            Object[] instanceArgs = new Object[unwrappedArgs.length - 1];
            System.arraycopy(unwrappedArgs, 1, instanceArgs, 0, instanceArgs.length);
            result = method.invoke(instance, instanceArgs);
        }

        return JayObject.generateObject(result);
    }

    private static Method findBestMatchingMethod(Class<?> clazz, String methodName, Class<?>[] parameterTypes)
            throws NoSuchMethodException {
        Method[] methods = clazz.getMethods();
        for (Method method : methods) {
            if (method.getName().equalsIgnoreCase(methodName)) {
                Class<?>[] methodParams = method.getParameterTypes();
                if (Modifier.isStatic(method.getModifiers())) {
                    if (isCompatible(methodParams, parameterTypes)) {
                        return method;
                    }
                } else {
                    if (parameterTypes.length > 0
                            && isCompatible(methodParams, getInstanceMethodParams(parameterTypes))) {
                        return method;
                    }
                }
            }
        }
        throw new NoSuchMethodException("No matching method found: " + methodName);
    }

    private static Class<?>[] getInstanceMethodParams(Class<?>[] parameterTypes) {
        Class<?>[] instanceParams = new Class<?>[parameterTypes.length - 1];
        System.arraycopy(parameterTypes, 1, instanceParams, 0, instanceParams.length);
        return instanceParams;
    }

    private static boolean isCompatible(Class<?>[] methodParams, Class<?>[] actualParams) {
        if (methodParams.length != actualParams.length) {
            return false;
        }
        for (int i = 0; i < methodParams.length; i++) {
            if (!methodParams[i].isAssignableFrom(actualParams[i])) {
                if (methodParams[i].isPrimitive() && actualParams[i] == getWrapperClass(methodParams[i])) {
                    continue;
                }
                return false;
            }
        }
        return true;
    }

    private static Class<?> getWrapperClass(Class<?> primitiveType) {
        if (primitiveType == int.class)
            return Integer.class;
        if (primitiveType == double.class)
            return Double.class;
        if (primitiveType == boolean.class)
            return Boolean.class;
        return primitiveType;
    }
}