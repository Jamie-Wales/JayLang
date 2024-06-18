package Types;

public interface JayType {
    boolean greaterThan(JayObject<?> object);

    boolean greaterThanEqual(JayObject<?> object);

    boolean lessThan(JayObject<?> object);

    boolean lessThanEqual(JayObject<?> object);

    boolean equal(JayObject<?> object);

    boolean notEqual(JayObject<?> object);

}
