package Types;

import java.math.BigDecimal;
import java.util.Objects;

public interface JayType {

    boolean greaterThan(JayObject<?> object);

    boolean greaterThanEqual(JayObject<?> object);

    boolean lessThan(JayObject<?> object);

    boolean lessThanEqual(JayObject<?> object);

    boolean equal(JayObject<?> object);

    boolean notEqual(JayObject<?> object);

    boolean equals(Object obj);
}

