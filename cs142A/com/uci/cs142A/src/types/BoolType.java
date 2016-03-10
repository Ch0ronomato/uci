package types;

public class BoolType extends Type {
    
    public BoolType()
    {

    }

    @Override
    public Type and(Type that) {
        if (!equivalent(that))
            return super.and(that);
        return new BoolType();
    }

    @Override
    public Type or(Type that) {
        if (!equivalent(that))
            return super.or(that);
        return new BoolType();
    }

    @Override
    public Type not() {
        return new BoolType();
    }

    @Override
    public String toString()
    {
        return "bool";
    }

    @Override
    public boolean equivalent(Type that)
    {
        if (that == null)
            return false;
        if (!(that instanceof BoolType))
            return false;
        
        return true;
    }

    @Override
    public int numBytes() {
        return 4;
    }
}    
