package Types;

import java.math.BigDecimal;

public class JayObject<T> implements JayType {
    public JayObject(Type type, T value) {
        this.type = type;
        this.value = value;
    }

    Type type;
    private T value;

    public boolean compare(JayObject<T> compare) {
        switch (compare.type) {
            case DECIMAL -> {
                return compare((BigDecimal) compare.value);
            }
            case STRING -> {
                return compare((String) compare.value);
            }
        }

        throw new RuntimeException("Invalid Type");
    }

    @Override
    public boolean compare(String str) {
        switch (this.type) {
            case DECIMAL -> {
                BigDecimal a = new BigDecimal("10001");
                return a.toString().compareTo(str) == 0;
            }
            case STRING -> {
                return this.value.toString().compareTo(str) == 0;
            }
        }

        throw new RuntimeException("Invalid Type");
    }

    @Override
    public boolean compare(BigDecimal bd) {
        switch (this.type) {
            case DECIMAL -> {
                return bd.compareTo((BigDecimal) this.value) == 0;
            }
            case STRING -> {
                return compare(bd.toString());
            }
        }

        throw new RuntimeException("Invalid Type");
    }

    public void print() {
        System.out.println("Hello, I am a Jay Object");
    }
}