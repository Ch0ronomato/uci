package types;

import java.lang.reflect.Array;

public class ArrayType extends Type {
    
    private Type base;
    private int extent;
    
    public ArrayType(int extent, Type base)
    {
        this.extent = extent;
        this.base = base;
    }

    @Override
    public Type add(Type that) {
        if (!equivalent(that))
            return base.add(that);
        return new ArrayType(extent, base);
    }

    @Override
    public Type sub(Type that) {
        if (!equivalent(that))
            return base.sub(that);
        return new ArrayType(extent, base);
    }

    @Override
    public Type mul(Type that) {
        if (!equivalent(that))
            return base.mul(that);
        return new ArrayType(extent, base);
    }

    @Override
    public Type div(Type that) {
        if (!equivalent(that))
            return base.div(that);
        return new ArrayType(extent, base);
    }

    @Override
    public Type compare(Type that) {
        if (!equivalent(that))
            return base.compare(that);
        return new ArrayType(extent, base);
    }

    @Override
    public Type index(Type that) {
        if (!(that instanceof IntType))
            return super.index(that);
        return base;
    }

    @Override
    public Type assign(Type source) {
        if (!base.equivalent(source))
            return base.assign(source);
        return getBaseType(base.toString());
    }

    public int extent()
    {
        return extent;
    }
    
    public Type base()
    {
        if (base instanceof ArrayType)
            return ((ArrayType) base).base();
        return base;
    }
    
    @Override
    public String toString()
    {
        return "array[" + extent + "," + base + "]";
    }
    
    @Override
    public boolean equivalent(Type that)
    {
        if (that == null)
            return false;
        if (!(that instanceof IntType))
            return false;
        
        ArrayType aType = (ArrayType)that;
        return this.extent == aType.extent && base.equivalent(aType.base);
    }

    @Override
    public int numBytes() {
        return this.base.numBytes() * this.extent;
    }
}
