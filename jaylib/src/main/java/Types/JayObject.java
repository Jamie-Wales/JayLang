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

    public static JayObject<?> generateObject(Object obj) {
        if (obj instanceof Double || obj instanceof Integer || obj instanceof BigDecimal) {
            return new JayObject<>(Type.DECIMAL, new BigDecimal(obj.toString()));
        } else if (obj instanceof String) {
            return new JayObject<>(Type.STRING, (String) obj);
        } else if (obj instanceof JayObject) {
            return (JayObject<?>) obj;
        } else {
            return new JayObject<>(Type.OBJECT, obj);
        }
    }

    public static JayObject<BigDecimal> generateObject(double d) {
        return new JayObject<>(Type.DECIMAL, new BigDecimal(d));
    }

    public static JayObject<BigDecimal> generateObject(int i) {
        return new JayObject<>(Type.DECIMAL, new BigDecimal(i));
    }

    public static JayObject<String> generateObject(String str) {
        return new JayObject<>(Type.STRING, str);
    }
   public static JayObject<Boolean> generateObject(boolean b) {
       return new JayObject<>(Type.BOOLEAN, b);
   }


    public Type getType() {
        return type;
    }

    public T getValue() {
        return value;
    }

    public Object getJavaObject() {
        if (type == Type.DECIMAL) {
            if (value instanceof BigDecimal) {
                return ((BigDecimal) value).doubleValue();
            } else {
                return new BigDecimal(value.toString()).doubleValue();
            }
        } else if (type == Type.BOOLEAN) {
            return ((Boolean) value).booleanValue();
        } else {
            return value;
        }
    }

    public Number getNumber() {
        if (value instanceof Number) {
            return (Number) value;
        } else {
            throw new IllegalArgumentException("The contained value is not a number");
        }
    }

    @Override
    public boolean greaterThan(JayObject<?> object) {
        validateType(object);
        switch (this.type) {
            case DECIMAL:
                return ((BigDecimal) this.value).compareTo((BigDecimal) object.value) > 0;
            case STRING:
                return ((String) this.value).compareTo((String) object.value) > 0;
            default:
                throw new RuntimeException("Comparison not supported for this type");
        }
    }

    @Override
    public boolean greaterThanEqual(JayObject<?> object) {
        return this.greaterThan(object) || this.equal(object);
    }

    @Override
    public boolean lessThan(JayObject<?> object) {
        validateType(object);
        switch (this.type) {
            case DECIMAL:
                return ((BigDecimal) this.value).compareTo((BigDecimal) object.value) < 0;
            case STRING:
                return ((String) this.value).compareTo((String) object.value) < 0;
            default:
                throw new RuntimeException("Comparison not supported for this type");
        }
    }

    @Override
    public boolean lessThanEqual(JayObject<?> object) {
        return this.lessThan(object) || this.equal(object);
    }

    @Override
    public boolean equal(JayObject<?> object) {
        if (this.type != object.type)
            return false;
        return Objects.equals(this.value, object.value);
    }

    @Override
    public boolean notEqual(JayObject<?> object) {
        return !this.equal(object);
    }

    public JayObject<?> negate() {
        switch (this.type) {
            case DECIMAL:
                return new JayObject<>(Type.DECIMAL, ((BigDecimal) this.value).negate());
            case STRING:
                return new JayObject<>(Type.STRING, new StringBuilder((String) this.value).reverse().toString());
            default:
                throw new RuntimeException("Negation not supported for this type");
        }
    }

    public JayObject<?> add(JayObject<?> add) {
        switch (this.type) {
            case DECIMAL:
                if (add.type == Type.DECIMAL) {
                    return new JayObject<>(Type.DECIMAL, ((BigDecimal) this.value).add((BigDecimal) add.value));
                } else if (add.type == Type.STRING) {
                    return new JayObject<>(Type.STRING, this.value.toString() + add.value);
                }
                break;
            case STRING:
                return new JayObject<>(Type.STRING, this.value.toString() + add.value.toString());
        }
        throw new RuntimeException("Addition not supported for these types");
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
        throw new RuntimeException("Subtraction not supported for these types");
    }

    public JayObject<?> multiply(JayObject<?> mul) {
        switch (this.type) {
            case DECIMAL:
                if (mul.type == Type.DECIMAL) {
                    return new JayObject<>(Type.DECIMAL, ((BigDecimal) this.value).multiply((BigDecimal) mul.value));
                } else if (mul.type == Type.STRING) {
                    int times = ((BigDecimal) this.value).intValue();
                    return new JayObject<>(Type.STRING, ((String) mul.value).repeat(times));
                }
                break;
            case STRING:
                if (mul.type == Type.DECIMAL) {
                    int times = ((BigDecimal) mul.value).intValue();
                    return new JayObject<>(Type.STRING, ((String) this.value).repeat(times));
                }
                break;
        }
        throw new RuntimeException("Multiplication not supported for these types");
    }

    private void validateType(JayObject<?> object) {
        if (this.type != object.type) {
            throw new RuntimeException("Type mismatch");
        }
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        JayObject<?> jayObject = (JayObject<?>) obj;
        return type == jayObject.type && Objects.equals(value, jayObject.value);
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, value);
    }

    @Override
    public String toString() {
        return value.toString();
    }

    public enum Type {
        DECIMAL, STRING, OBJECT, BOOLEAN
    }
}
