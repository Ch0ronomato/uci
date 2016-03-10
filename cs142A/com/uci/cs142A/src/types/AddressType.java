package types;

public class AddressType extends Type {
    
    private Type base;
    
    public AddressType(Type base)
    {
        this.base = base;
    }

    @Override
    public Type add(Type that) {
        if (!equivalent(that))
            return base.add(that);
        return new AddressType(base);
    }

    @Override
    public Type sub(Type that) {
        if (!equivalent(that))
            return base.sub(that);
        return new AddressType(base);
    }

    @Override
    public Type mul(Type that) {
        if (!equivalent(that))
            return base.mul(that);
        return new AddressType(base);
    }

    @Override
    public Type div(Type that) {
        if (!equivalent(that))
            return base.div(that);
        return new AddressType(base);
    }

    @Override
    public Type deref() {
        return base;
    }

    @Override
    public Type assign(Type source) {
        if (!base.equivalent(source))
            return super.assign(source);
        return getBaseType(base.toString());
    }

    @Override
    public Type index(Type that) {
        if (base instanceof ArrayType)
            return new AddressType(base.index(that));
        return super.index(that);
    }

    public Type base()
    {
        return base;
    }

    @Override
    public String toString()
    {
        return "Address(" + base + ")";
    }

    @Override
    public boolean equivalent(Type that) {
        if (that == null)
            return false;
        if (!(that instanceof AddressType))
            return false;
        
        AddressType aType = (AddressType)that;
        return this.base.equivalent(aType.base);
    }

    @Override
    public int numBytes() {
        return this.base.numBytes();
    }
}
