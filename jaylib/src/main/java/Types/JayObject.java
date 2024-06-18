package Types;

import java.math.BigDecimal;
import java.util.Objects;

public class JayObject<T> implements JayType {
    private final Type type;
    private final T value;

    public JayObject(Type type, T value) {
        this.type = type;
        this.value = value;
    }

    public Type getType() {
        return type;
    }

    public T getValue() {
        return value;
    }

    @Override
    public boolean greaterThan(JayObject<?> object) {
        validateType(object);
        switch (this.type) {
            case DECIMAL:
                return ((BigDecimal) this.value).compareTo((BigDecimal) object.value) > 0;
            case STRING:
                return ((String) this.value).length() > ((String) object.value).length();
            default:
                throw new RuntimeException("Invalid type");
        }
    }

    @Override
    public boolean greaterThanEqual(JayObject<?> object) {
        validateType(object);
        return this.greaterThan(object) || this.equal(object);
    }

    @Override
    public boolean lessThan(JayObject<?> object) {
        validateType(object);
        switch (this.type) {
            case DECIMAL:
                return ((BigDecimal) this.value).compareTo((BigDecimal) object.value) < 0;
            case STRING:
                return ((String) this.value).length() < ((String) object.value).length();
            default:
                throw new RuntimeException("Invalid type");
        }
    }

    @Override
    public boolean lessThanEqual(JayObject<?> object) {
        validateType(object);
        return this.lessThan(object) || this.equal(object);
    }

    @Override
    public boolean equal(JayObject<?> object) {
        validateType(object);
        return this.value.equals(object.value);
    }

    @Override
    public boolean notEqual(JayObject<?> object) {
        return !this.equal(object);
    }

    private void validateType(JayObject<?> object) {
        if (this.type != object.type) {
            throw new RuntimeException("Invalid type");
        }
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;

        JayObject<?> jayObject = (JayObject<?>) obj;

        if (type != jayObject.type) return false;
        return Objects.equals(value, jayObject.value);
    }

}
