package Types;

import java.math.BigDecimal;
import java.util.Objects;

public class JayObject<T> implements JayType {
    private Type type;
    private T value;

    public JayObject(Type type, T value) {
        this.type = type;
        this.value = value;
    }

    public static JayObject<BigDecimal> generateObject(double d) {
        return new JayObject<>(Type.DECIMAL, new BigDecimal(d));
    }

    public static JayObject<String> generateObject(String str) {
        return new JayObject<>(Type.STRING, str);
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

    public JayObject<?> add(JayObject<?> add) {
        switch (this.type) {
            case DECIMAL:
                if (add.type == Type.DECIMAL) {
                    return new JayObject<>(Type.DECIMAL, ((BigDecimal) this.value).add((BigDecimal) add.value));
                } else {
                    return new JayObject<>(Type.STRING, this.value.toString() + add.value.toString());
                }
            case STRING:
                return new JayObject<>(Type.STRING, this.value.toString() + add.value.toString());
        }
        throw new RuntimeException("Invalid type");
    }

    public JayObject<?> multiply(JayObject<?> mul) {
        switch (this.type) {
            case DECIMAL:
                switch (mul.type) {
                    case DECIMAL:
                        return new JayObject<>(Type.DECIMAL,
                                ((BigDecimal) this.value).multiply((BigDecimal) mul.value));
                    case STRING:
                        int times = ((BigDecimal) this.value).intValue();
                        StringBuilder result = new StringBuilder();
                        for (int i = 0; i < times; i++) {
                            result.append((String) mul.value);
                        }
                        return new JayObject<>(Type.STRING, result.toString());
                    default:
                        throw new RuntimeException("Invalid type for multiplication");
                }
            case STRING:
                switch (mul.type) {
                    case DECIMAL:
                        int times = ((BigDecimal) mul.value).intValue();
                        StringBuilder result = new StringBuilder();
                        for (int i = 0; i < times; i++) {
                            result.append((String) this.value);
                        }
                        return new JayObject<>(Type.STRING, result.toString());
                    default:
                        throw new RuntimeException("Invalid type for multiplication");
                }
            default:
                throw new RuntimeException("Invalid type for multiplication");
        }
    }

    public JayObject<?> subtract(JayObject<?> sub) {
        switch (this.type) {
            case DECIMAL:
                if (sub.type == Type.DECIMAL) {
                    return new JayObject<>(Type.DECIMAL, ((BigDecimal) this.value).subtract((BigDecimal) sub.value));
                }
                break;
            case STRING:
                if (sub.type == Type.STRING) {
                    String result = ((String) this.value).replaceFirst((String) sub.value, "");
                    return new JayObject<>(Type.STRING, result);
                }
                break;
        }
        throw new RuntimeException("Invalid type for subtraction");
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
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;

        JayObject<?> jayObject = (JayObject<?>) obj;

        if (type != jayObject.type)
            return false;
        return Objects.equals(value, jayObject.value);
    }

    @Override
    public String toString() {
        return value.toString();
    }
}
